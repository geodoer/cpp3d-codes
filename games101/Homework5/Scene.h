#pragma once

#include <vector>
#include <memory>
#include "Vector.h"
#include "Object.h"
#include "Light.h"

//场景
class Scene
{
public:
    // setting up options
    int width = 1280;
    int height = 960;
    double fov = 90;    //垂直可视角度
    Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137);   //背景颜色
    int maxDepth = 5;           //递归的最大深度
    float epsilon = 0.00001;    //两数值相等的阈值

    Scene(int w, int h) : width(w), height(h)
    {}

    void Add(std::unique_ptr<Object> object) { objects.push_back(std::move(object)); }
    void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

    [[nodiscard]] const std::vector<std::unique_ptr<Object> >& get_objects() const { return objects; }
    [[nodiscard]] const std::vector<std::unique_ptr<Light> >&  get_lights() const { return lights; }

private:
    // creating the scene (adding objects and lights)
    std::vector<std::unique_ptr<Object> > objects;  //物体
    std::vector<std::unique_ptr<Light> > lights;    //光线
};