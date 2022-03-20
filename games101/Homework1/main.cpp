#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <Eigen/Core>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

//获得绕任意过原点的轴的旋转变换矩阵
Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle)
{
	float radian = angle / 180 * MY_PI;
	
	Eigen::AngleAxisf rotation_vec(radian, axis);
	Eigen::Matrix3f mat3x3 = rotation_vec.toRotationMatrix();

	Eigen::Matrix4f mat4x4 = Eigen::Matrix4f::Identity();
	for(int i=0; i<3; ++i)
		for(int j=0; j<3; ++j)
			mat4x4(i, j) = mat3x3(i, j);

	return mat4x4;
}

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -eye_pos[0],
			  0, 1, 0, -eye_pos[1],
			  0, 0, 1, -eye_pos[2],
			  0, 0, 0, 1;

	view = translate * view;

	return view;
}

//模型变换：物体的摆放
//在此函数中，你只需要实现三维中绕 z 轴旋转的变换矩阵，而不用处理平移与缩放
Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the model matrix for rotating the triangle around the Z axis.
	// Then return it.
	float radian = rotation_angle / 180.0*MY_PI;
	Eigen::Matrix4f rotate;
	rotate << std::cos(radian), -std::sin(radian), 0, 0,
		   std::sin(radian), std::cos(radian), 0, 0,
		   0, 0, 1, 0,
		   0, 0, 0, 1;

	//test function get_rotation
	// if(rotate == get_rotation(Eigen::Vector3f(0,0, 1) ,rotation_angle))
	// {
	// 	std::cout << "two rotation equal" << std::endl;
	// }

	model = rotate * model;
	return model;
}

//投影变换：三维物体映射到屏幕空间
//eye_fov垂直可视角度
//aspect_ratio屏幕高宽比
//zNear、zFar近远平面
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
									  float zNear, float zFar)
{
	// Students will implement this function

	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the projection matrix for the given parameters.
	// Then return it.
	//////////////////////////////////////////////////////////////////////////透视投影->正交投影的矩阵
	Eigen::Matrix4f persp_ortho;
	persp_ortho << zNear, 0, 0, 0,
				0, zNear, 0, 0,
				0, 0, zNear + zFar, -zNear * zFar,
				0, 0, 1, 0;

	//////////////////////////////////////////////////////////////////////////正交投影的矩阵
	float fovY_half = eye_fov * MY_PI / 180.0 / 2.0;
	float yTop = std::fabs(zNear) * std::tan(fovY_half);
	float xRight = aspect_ratio * yTop;

	float xLeft = -xRight;
	float yButtom = -yTop;

	Eigen::Matrix4f ortho; //正交投影的矩阵
	ortho << 2 / (xRight - xLeft), 0, 0, -(xRight + xLeft)/2,
		  0, 2 / (yTop - yButtom), 0, (yTop + yButtom) / 2,
		  0, 0, 2 / (zNear - zFar), - (zNear + zFar)/2,
		  0, 0, 0, 1;

	projection = ortho * persp_ortho;
	return projection;
}

//模拟了图形管线
int main(int argc, const char** argv)
{
	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if(argc >= 3)
	{
		command_line = true;
		angle = std::stof(argv[2]); // -r by default

		if(argc == 4)
		{
			filename = std::string(argv[3]);
		}
		else
		{
			return 0;
		}
	}

	rst::rasterizer r(700, 700); //定义光栅化器类的实例

	Eigen::Vector3f eye_pos = {0, 0, 5};

	std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

	std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

	//设置了其必要的变量
	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);

	int key = 0;
	int frame_count = 0;

	if(command_line)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);

		cv::imwrite(filename, image);

		return 0;
	}

	while(key != 27)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::imshow("image", image);
		key = cv::waitKey(10);

		std::cout << "frame count: " << frame_count++ << '\n';

		if(key == 'a')
		{
			angle += 10; //按'a'增加旋转角度
		}
		else if(key == 'd')
		{
			angle -= 10; //按'a'减少旋转角度
		}
	}

	return 0;
}
