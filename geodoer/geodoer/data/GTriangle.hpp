#pragma once
#include"geodoer/data/GPoint.hpp"

namespace geodoer
{

class GTriangle : public std::array<GPoint, 3>
{
public:
	GTriangle(const GPoint& a, const GPoint& b, const GPoint& c) : array({ a, b, c }){}
};

}