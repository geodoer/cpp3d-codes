// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <math.h>
#include <float.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
	auto id = get_next_id();
	pos_buf.emplace(id, positions);

	return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
	auto id = get_next_id();
	ind_buf.emplace(id, indices);

	return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
	auto id = get_next_id();
	col_buf.emplace(id, cols);

	return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
	return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

//判断点pnt是否在三角形tri中
static bool insideTriangle(const Eigen::Vector2f& pnt, const Eigen::Vector2f* tri)
{
	//二维向量叉乘，获得叉乘结果z轴的坐标值
	auto _cross = [](const Eigen::Vector2f& u, const Eigen::Vector2f& v)->double
	{
		return u.x() * v.y() - u.y() * v.x();
	};
	
	auto a = _cross(tri[0] - pnt, tri[0] - tri[1]);	//A-B
	auto b = _cross(tri[1] - pnt, tri[1] - tri[2]);	//B-C
	auto c = _cross(tri[2] - pnt, tri[2] - tri[0]);	//C-A

	if (a > 0 && b > 0 && c > 0)
	{
		return true;
	}
	else if (a < 0 && b < 0 && c < 0)
	{
		return true;
	}

	return false;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
	float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
	float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
	float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
	return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
	//取出对应的数据块
	auto& buf = pos_buf[pos_buffer.pos_id];
	auto& ind = ind_buf[ind_buffer.ind_id];
	auto& col = col_buf[col_buffer.col_id];

	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;

	Eigen::Matrix4f mvp = projection * view * model;

	for(auto& i : ind)
	{
		//构建三角形
		Triangle t;

		//对图元做MVP变换，做完之后，物体会集中在一个[-1,1]^3的经典立方体中
		Eigen::Vector4f v[] =
		{
			mvp * to_vec4(buf[i[0]], 1.0f),
			mvp * to_vec4(buf[i[1]], 1.0f),
			mvp * to_vec4(buf[i[2]], 1.0f)
		};

		//Homogeneous division（使第四维为1）
		for(auto& vec : v)
		{
			vec /= vec.w();
		}

		//Viewport transformation（视口变换）
		//将标准立方体转换到屏幕空间上，即[0, width],[0, height],注意z<0
		for(auto & vert : v)
		{
			vert.x() = 0.5*width*(vert.x()+1.0);
			vert.y() = 0.5*height*(vert.y()+1.0);
			vert.z() = vert.z() * f1 + f2;
		}

		//设置顶点
		for(int i = 0; i < 3; ++i)
		{
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
		}

		//设置颜色
		auto col_x = col[i[0]];
		auto col_y = col[i[1]];
		auto col_z = col[i[2]];

		t.setColor(0, col_x[0], col_x[1], col_x[2]);
		t.setColor(1, col_y[0], col_y[1], col_y[2]);
		t.setColor(2, col_z[0], col_z[1], col_z[2]);

		//光栅化这个三角形
		rasterize_triangle(t);
	}
}

#define USE_MSAA
//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t)
{
	//三角形的三个点
	const auto& tri = t.v;
	Eigen::Vector2f triXY[3] =
	{
		tri[0].head(2),
		tri[1].head(2),
		tri[2].head(2)
	};
	auto triVec4 = t.toVector4();

	// TODO : Find out the bounding box of current triangle.（找出当前三角形的边界框）
	// iterate through the pixel and find if the current pixel is inside the triangle（遍历像素，查找当前像素是否在三角形内）
	float x_min = FLT_MAX, x_max = FLT_MIN; //#include <float.h>
	float y_min = FLT_MAX, y_max = FLT_MIN;

	for(const auto& pnt : t.v)
	{
		x_max = std::max(pnt.x(), x_max);
		x_min = std::min(pnt.x(), x_min);
		y_max = std::max(pnt.y(), y_max);
		y_min = std::min(pnt.y(), y_min);
	}

	//处理当前像素
	auto _process = [&](int x, int y)
	{
		if (false == insideTriangle({ x + 0.5, y + 0.5 }, triXY))
		{
			return;
		}

		float alpha, beta, gamma;
		std::tie(alpha, beta, gamma) = computeBarycentric2D(x, y, tri);
		float w_reciprocal = 1.0 / (alpha / triVec4[0].w() + beta / triVec4[1].w() + gamma / triVec4[2].w());
		float z_interpolated = alpha * triVec4[0].z() / triVec4[0].w() + beta * triVec4[1].z() / triVec4[1].w() + gamma * triVec4[2].z() / triVec4[2].w();
		z_interpolated *= w_reciprocal;

		int idx = get_index(x, y);

		if(z_interpolated < depth_buf[idx])
		{
			depth_buf[idx] = z_interpolated;
			frame_buf[idx] = t.getColor();
		}
	};
	
	//使用MASS方法处理每个像素
	auto _process_by_msaa = [&](int x, int y)
	{
		//采样点的偏移值
		std::array<Eigen::Vector2f, 4> offsets = 
		{
			Vector2f(0.25f, 0.25f),	//左下角
			{0.75,0.25},			//右下角
			{0.25,0.75},			//左上角
			{0.75,0.75}				//右上角
		};

		float minDepth = FLT_MAX;
		int cnt = 0;

		//遍历每个采样点
		for (const auto& offset : offsets)
		{
			auto pnt = offset + Vector2f(x, y); //采样点
			
			if (false == insideTriangle(pnt, triXY))
			{
				continue;
			}

			//该采样点在三角形内，则插值出z值
			float alpha, beta, gamma;
			std::tie(alpha, beta, gamma) = computeBarycentric2D(pnt.x(), pnt.y(), tri);
			float w_reciprocal = 1.0 / (alpha / triVec4[0].w() + beta / triVec4[1].w() + gamma / triVec4[2].w());
			float z_interpolated = alpha * triVec4[0].z() / triVec4[0].w() + beta * triVec4[1].z() / triVec4[1].w() + gamma * triVec4[2].z() / triVec4[2].w();
			z_interpolated *= w_reciprocal;

			//depth_buf存的是正数
			minDepth = std::min(minDepth, z_interpolated);
			++cnt;
		}

		if (cnt != 0)
		{
			auto idx = get_index(x, y);

			if (depth_buf[idx] > minDepth)
			{
				depth_buf[idx] = minDepth;
				frame_buf[idx] = t.getColor() * cnt / offsets.size();
			}
		}
	};
		
	int xmin = std::floor(x_min);	//向下取整
	int ymin = std::floor(y_min);
	int xmax = std::ceil(x_max);	//向上取整
	int ymax = std::ceil(y_max);

	for (int x = xmin; x <= xmax; ++x)
	{
		for (int y = ymin; y <= ymax; ++y)
		{
#ifdef USE_MSAA
			_process_by_msaa(x ,y);
#else
			_process(x, y);
#endif // USE_MASS
		}
	}
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
	model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
	view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
	projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
	if((buff & rst::Buffers::Color) == rst::Buffers::Color)
	{
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
	}

	if((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
	{
		//深度buf初始化时，赋值的是最大值，说明深度buf存的是正数
		std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
	}
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
	frame_buf.resize(w * h);
	depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
	//opencv图像的坐标系原点在左上角，+y向下，+x向右
	return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
	//old index: auto ind = point.y() + point.x() * width;
	auto ind = (height-1-point.y())*width + point.x();
	frame_buf[ind] = color;
}

// clang-format on