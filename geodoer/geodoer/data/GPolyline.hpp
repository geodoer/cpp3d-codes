#pragma once
#include"geodoer/data/GPoint.hpp"
#include"geodoer/data/GBoundingBox.hpp"

namespace geodoer
{

class GPolyline : public std::vector<GPoint>
{
public:
	GBoundingBox getAABB() const;
	void offset(const GPoint& center);
};

class GMultiPolyline : public std::vector<GPolyline>
{
public:
	GBoundingBox getAABB() const;
	void offset(const GPoint& center);
};

inline void GPolyline::offset(const GPoint& center)
{
	for (auto& p : *this)
	{
		p = p - center;
	}
}

inline GBoundingBox GPolyline::getAABB() const
{
	GBoundingBox box;

	for (const auto& p : *this)
	{
		box.expandBy(p.x(), p.y(), p.z());
	}

	return box;
}

inline GBoundingBox GMultiPolyline::getAABB() const
{
	GBoundingBox box;
	
	for (const auto& poly : *this)
	{
		box.expandBy(poly.getAABB());
	}

	return box;
}

inline void GMultiPolyline::offset(const GPoint& center)
{
	for (auto& line : *this)
	{
		line.offset(center);
	}
}

}