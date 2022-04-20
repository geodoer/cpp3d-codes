#pragma once
#include <float.h>
#include <array>

namespace geodoer {

class GBoundingBox
{
public:
	typedef double type;
	type mXMin, mYMin, mZMin;
	type mXMax, mYMax, mZMax;

public:
	GBoundingBox()
		: mXMin(FLT_MAX), mYMin(FLT_MAX), mZMin(FLT_MAX)
		, mXMax(-FLT_MAX), mYMax(-FLT_MAX), mZMax(-FLT_MAX)
	{}

	void init()
	{
		mXMin = mYMin = mZMin = FLT_MAX;
		mXMax = mYMax = mZMax = -FLT_MAX;
	}

	bool isValid() const
	{
		return mXMax >= mXMin && mYMax >= mYMin && mZMax >= mZMin;
	}

	std::array<type, 3> center() const
	{
		std::array<type, 3> _center;
		_center[0] = (mXMax + mXMin) * 0.5;
		_center[1] = (mYMax + mYMin) * 0.5;
		_center[2] = (mZMax + mZMin) * 0.5;
		return _center;
	}

	void expandBy(type x, type y, type z)
	{
		if(x < mXMin)
		{
			mXMin = x;
		}

		if(x > mXMax)
		{
			mXMax = x;
		}

		if(y < mYMin)
		{
			mYMin = y;
		}

		if(y > mYMax)
		{
			mYMax = y;
		}

		if(z < mZMin)
		{
			mZMin = z;
		}

		if(z > mZMax)
		{
			mZMax = z;
		}
	}

	void expandBy(const GBoundingBox& rhs)
	{
		if(!rhs.isValid())
		{
			return;
		}

		if(rhs.mXMin < this->mXMin)
		{
			this->mXMin = rhs.mXMin;
		}

		if(rhs.mXMax > this->mXMax)
		{
			this->mXMax = rhs.mXMax;
		}

		if(rhs.mYMin < this->mYMin)
		{
			this->mYMin = rhs.mYMin;
		}

		if(rhs.mYMax > this->mYMax)
		{
			this->mYMax = rhs.mYMax;
		}

		if(rhs.mZMin < this->mZMin)
		{
			this->mZMin = rhs.mZMin;
		}

		if(rhs.mZMax > this->mZMax)
		{
			this->mZMax = rhs.mZMax;
		}
	}

	bool contains(type x, type y, type z) const
	{
		return isValid() &&
			   (x >= mXMin && x <= mXMax) &&
			   (y >= mYMin && y <= mYMax) &&
			   (z >= mZMin && z <= mZMax);
	}

	bool contains(type x, type y, type z, type epsilon) const
	{
		return isValid() &&
			   ((x + epsilon) >= mXMin && (x - epsilon) <= mXMax) &&
			   ((y + epsilon) >= mYMin && (y - epsilon) <= mYMax) &&
			   ((z + epsilon) >= mZMin && (z - epsilon) <= mZMax);
	}
};

}