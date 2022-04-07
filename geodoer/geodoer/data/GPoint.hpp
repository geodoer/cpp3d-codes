#pragma once
#include<array>
#include<vector>

namespace geodoer
{

class GPoint : public std::array<double, 3>
{
public:
	GPoint() = default;
	GPoint(double a, double b, double c) : array({ a,b,c }){}
public:
	double x() const
	{
		return (*this)[0];
	}
	double& x()
	{
		return (*this)[0];
	}

	double y() const
	{
		return this->operator[](1);
	}
	double& y()
	{
		return this->operator[](1);
	}

	double z() const
	{
		return this->operator[](2);
	}
	double& z()
	{
		return this->operator[](2);
	}
};

class GPoints : public std::vector<GPoint>
{
public:
	GPoints(std::initializer_list<GPoint> l)
	{
		for (auto it = l.begin(); it != l.end(); ++it)
		{
			this->emplace_back(*it);
		}
	}
};

}