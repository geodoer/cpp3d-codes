//
// Created by goksu on 4/6/19.
//

#pragma once

#include <Eigen/Eigen>

#include <algorithm>
#include "global.hpp"
#include "Triangle.hpp"
using namespace Eigen;

namespace rst
{
    //缓存类型
    enum class Buffers
    {
        Color = 1,  //颜色
        Depth = 2   //深度
    };

    inline Buffers operator|(Buffers a, Buffers b)
    {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b)
    {
        return Buffers((int)a & (int)b);
    }

    //图元类型
    enum class Primitive
    {
        Line,       //线框
        Triangle    //三角形
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    class rasterizer
    {
    public:
        rasterizer(int w, int h);
		//加载数据
        pos_buf_id load_positions(const std::vector<Eigen::Vector3f>& positions);
        ind_buf_id load_indices(const std::vector<Eigen::Vector3i>& indices);
        col_buf_id load_colors(const std::vector<Eigen::Vector3f>& colors);
		//设置矩阵
        void set_model(const Eigen::Matrix4f& m);
        void set_view(const Eigen::Matrix4f& v);
        void set_projection(const Eigen::Matrix4f& p);
        //设置像素
        void set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);
        //传入buff的类型，清空buff
        void clear(Buffers buff);
        //绘制三角形！
		//pos_buffer：点所在缓存的ID
		//ind_buffer：图元所在缓存的ID
		//col_buffer：颜色所在缓存的ID
        void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);
        //帧图像
        std::vector<Eigen::Vector3f>& frame_buffer() { return frame_buf; }

    private:
        //画线
        void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);
        //绘制三角形
		//三角形t已经是屏幕坐标,[0, width],[0,heigh],z<0表示深度
        void rasterize_triangle(const Triangle& t);

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:
        //模型变换矩阵
        Eigen::Matrix4f model;
        //视图变换
        Eigen::Matrix4f view;
        //投影变换
        Eigen::Matrix4f projection;

		//buf: 存储数据，由一个id映射
        //顶点缓存块
		//key:id；value:该ID所对应的顶点缓存，这个缓存中存有许多顶点
        std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
        //图元
        std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
        //颜色
        std::map<int, std::vector<Eigen::Vector3f>> col_buf;

		//渲染结果
        std::vector<Eigen::Vector3f> frame_buf;
        //深度图
        std::vector<float> depth_buf;
	public:
		//屏幕(x,y)处，在数组中的下标
        int get_index(int x, int y);
		//屏幕的宽高
        int width, height;
	private:
        //该类的唯一ID
        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}
