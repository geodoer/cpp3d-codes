#pragma once

#include "Vector.h"

//光线
class Light
{
public:
    Light(const Vector3f& p, const Vector3f& i)
        : position(p)
        , intensity(i)
    {}
    virtual ~Light() = default;
    Vector3f position;  //位置
    Vector3f intensity; //该光线，在单位时间内的能量（即一个立体角上的能量）
};
