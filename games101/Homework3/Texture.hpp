//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>

class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
		if(u<0)u=0;
        if(u>1)u=1;
        if(v<0)v=0;
        if(v>1)v=1;

        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

	//根据uv坐标获取颜色时，使用双线性插值
    Eigen::Vector3f getColorBilinear(float u, float v)
	{
		if(u<0)u=0;
        if(u>1)u=1;
        if(v<0)v=0;
        if(v>1)v=1;

		auto get_color = [this](float x, float y)->Eigen::Vector3f
		{
			auto c = this->image_data.at<cv::Vec3b>(x, y);
			return Eigen::Vector3f(c[0], c[1], c[2]);
		};

		//线性插值
		auto lerp = [](float coefficient, Eigen::Vector3f color_a, Eigen::Vector3f color_b)->Eigen::Vector3f
		{
			return color_a + coefficient * (color_b - color_a);
		};

		//uv坐标的范围是[0, 1]，这里将uv坐标映射到到图片坐标中
		auto u_img = u * width;
		auto v_img = (1 - v) * height;

		float u_img_min = std::floor(u_img);
		float u_img_max = std::min((float)width, std::ceil(u_img)); //防止超出范围
		float v_img_min = std::floor(v_img);
		float v_img_max = std::min((float)height, std::ceil(v_img));

		//去图片中获取颜色
		auto u00 = get_color(v_img_min, u_img_min);	//左下角
		auto u01 = get_color(v_img_max, u_img_min); //左上角
		auto u10 = get_color(v_img_min, u_img_max); //右下角
		auto u11 = get_color(v_img_max, u_img_max);	//右上角
        
		//距离左下角的距离（要做归一）
		float s = (u_img - u_img_min) / (u_img_max - u_img_min);
		float t = (v_img - v_img_max) / (v_img_min - v_img_max);

		//插值
        auto u0 = lerp(s,u00,u10);
        auto u1 = lerp(s,u01,u11);
        auto color = lerp(t,u0,u1);

        return Eigen::Vector3f(color[0], color[1], color[2]);
	}
};
#endif //RASTERIZER_TEXTURE_H
