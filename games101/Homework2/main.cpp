// clang-format off
#include <iostream>

#define USE_OPENCV
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif // USE_OPENCV

#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"

constexpr double MY_PI = 3.1415926;

//视图变换
//@param:eye_pos 摄像机的位置
//@Desc:    视图变换将完成摄像机的摆放
//			定义一个摄像机需要三个参数：相机位置；相机镜头的方向；相机的向上方向
//			由于这里只传入了摄像机的位置，说明只做了偏移，三个轴向的方向与世界坐标系相同
//			故
//			1. 相机镜头方向：世界坐标系的-z方向
//			2. 相机向上方向：世界坐标系的+y方向
//			3. 相机位置：eye_pos
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1,0,0,-eye_pos[0],
			  0,1,0,-eye_pos[1],
			  0,0,1,-eye_pos[2],
			  0,0,0,1;

	view = translate*view;

	return view;
}

//模型变换：物体的摆放
//在此作业中，设为单位矩阵就OK
Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	return model;
}

//投影变换：三维物体映射到屏幕空间
//@param:eye_fov        垂直可视角度
//@param:aspect_ratio   屏幕高宽比
//@param:zNear、zFar	近远平面的z值
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	//本程序为右手坐标系，故照相机是看向-z方向
	//而zNear、zFar为近远平面的z值，所以这里两个值是负值！
	//但main函数中传入的值为正的，不知道为什么要这样
	//这里确保zNear、zFar为负数，因为它们表示的是【坐标值】
	zNear = -fabs(zNear);
	zFar = -fabs(zFar);

	float n = fabs(zNear); //近平面与摄像机的距离

	//////////////////////////////////////////////////////////////////////////透视投影->正交投影的矩阵
	Eigen::Matrix4f persp_ortho;
	persp_ortho << n, 0, 0, 0,
				0, n, 0, 0,
				0, 0, n + zFar, -n * zFar,
				0, 0, 1, 0;
	//注意，这里的n是近平面与摄像机的距离，而不是坐标值；而zFar用的是坐标值

	//////////////////////////////////////////////////////////////////////////正交投影的矩阵
	float fovY_half = eye_fov * MY_PI / 180.0 / 2.0;

	//在相机坐标系中，yTop、xRight有以下关系
	float yTop = n * std::tan(fovY_half);
	float xRight = aspect_ratio * yTop;

	//在相机坐标系中，b和t相反，l和r相反
	float xLeft = -xRight;
	float yButtom = -yTop;

	Eigen::Matrix4f ortho; //正交投影的矩阵
	ortho << 2 / (xRight - xLeft), 0, 0, -(xRight + xLeft) / 2,
		  0, 2 / (yTop - yButtom), 0, (yTop + yButtom) / 2,
		  0, 0, 2 / (zNear - zFar), -(zNear + zFar) / 2,
		  0, 0, 0, 1; //这里可以简化，因为xLeft = -xRight等等
	//注意：这里的zNear、zFar是z轴上的坐标值！

	projection = ortho * persp_ortho;
	return projection;
}

int main(int argc, const char** argv)
{
	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if(argc == 2)
	{
		command_line = true;
		filename = std::string(argv[1]);
	}

	//光栅化类（并传入屏幕的长、宽）
	rst::rasterizer r(700, 700);

	//摄像机的位置
	Eigen::Vector3f eye_pos = {0,0,5};

	//顶点集
	std::vector<Eigen::Vector3f> pos
	{
		{2, 0, -2},
		{0, 2, -2},
		{-2, 0, -2},
		{3.5, -1, -5},
		{2.5, 1.5, -5},
		{-1, 0.5, -5}
	};

	//两个三角形（存储的是顶点的下标）
	std::vector<Eigen::Vector3i> ind
	{
		{0, 1, 2},
		{3, 4, 5}
	};

	//每个顶点所对应的颜色
	std::vector<Eigen::Vector3f> cols
	{
		{217.0, 238.0, 185.0},
		{217.0, 238.0, 185.0},
		{217.0, 238.0, 185.0},
		{185.0, 217.0, 238.0},
		{185.0, 217.0, 238.0},
		{185.0, 217.0, 238.0}
	};

	//将顶点、图元、颜色加入栅格器，并获得它们的ID
	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);
	auto col_id = r.load_colors(cols);

	int key = 0;
	int frame_count = 0;

	if(command_line)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

#ifdef USE_OPENCV
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imwrite(filename, image);
#endif // #ifdef USE_OPENCV

		return 0;
	}

	while(key != 27)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

#ifdef USE_OPENCV
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);
		key = cv::waitKey(10);
#endif // USE_OPENCV

		std::cout << "frame count: " << frame_count++ << '\n';
	}

	return 0;
}
// clang-format on