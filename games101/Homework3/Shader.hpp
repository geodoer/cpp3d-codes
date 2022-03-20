//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <Eigen/Eigen>
#include "Texture.hpp"


struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        texture = nullptr;
    }

    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,const Eigen::Vector2f& tc, Texture* tex) :
         color(col), normal(nor), tex_coords(tc), texture(tex) 
    {
        //规范纹理坐标
        auto& u = tex_coords[0];
        auto& v = tex_coords[1];

        if (u < 0) u = 0;
		if (u > 1) u = 1;
		if (v < 0) v = 0;
		if (v > 1) v = 1;
    }

    Eigen::Vector3f view_pos;   //该像素的相机坐标 
    Eigen::Vector3f color;      //颜色
    Eigen::Vector3f normal;     //法向量
    Eigen::Vector2f tex_coords; //纹理坐标
    Texture* texture;           //纹理
};

struct vertex_shader_payload
{
    Eigen::Vector3f position;
};

#endif //RASTERIZER_SHADER_H
