#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <ElSLib.hxx>
#include <GccAna_Lin2d2Tan.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <STEPControl_Writer.hxx>

#include "algo/GeomAlgo.hpp"
#include "io/TopoDSIO.hpp"
#include <geodoer/io/ObjIOPlugin.hpp>

/**
 * \brief 创建BSpline曲面
 */
void CreateBSplineSurface()
{
	//为BSpline表面定义一个4x4的网格点
	TColgp_Array2OfPnt aBSplinePnts(1, 4, 1, 4);

	for(Standard_Integer i = 1; i <= 4; ++i)
	{
		gp_Pnt aPnt;
		aPnt.SetX(5.0 * i);

		for(Standard_Integer j = 1; j <= 4; ++j)
		{
			aPnt.SetY(5.0 * j);

			if(1 < i && i < 4 && 1 < j && j < 4)
			{
				aPnt.SetZ(5.0); //内部点z=5.0
			}
			else
			{
				aPnt.SetZ(0.0); //边界点z=0
			}

			aBSplinePnts.SetValue(i, j, aPnt);
		}
	}

	// Define BSpline weights.
	TColStd_Array2OfReal aBSplineWeights(1, 4, 1, 4);

	for(Standard_Integer i = 1; i <= 4; ++i)
	{
		for(Standard_Integer j = 1; j <= 4; ++j)
		{
			if(1 < i && i < 4 && 1 < j && j < 4)
			{
				aBSplineWeights.SetValue(i, j, 0.5);
			}
			else
			{
				aBSplineWeights.SetValue(i, j, 1.0);
			}
		}
	}

	// Define knots.
	TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
	aUKnots.SetValue(1, 0.0);
	aUKnots.SetValue(2, 1.0);
	aVKnots.SetValue(1, 0.0);
	aVKnots.SetValue(2, 1.0);

	// Define multiplicities.
	TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
	aUMults.SetValue(1, 4);
	aUMults.SetValue(2, 4);
	aVMults.SetValue(1, 4);
	aVMults.SetValue(2, 4);

	// Define BSpline degree and periodicity.
	Standard_Integer aUDegree = 3;
	Standard_Integer aVDegree = 3;
	Standard_Boolean aUPeriodic = Standard_False;
	Standard_Boolean aVPeriodic = Standard_False;

	// Create a BSpline surface.
	Handle(Geom_BSplineSurface) aBSplineSurf = new Geom_BSplineSurface(
		aBSplinePnts, aBSplineWeights, aUKnots, aVKnots,
		aUMults, aVMults, aUDegree, aVDegree, aUPeriodic, aVPeriodic);

	//保存查看
	auto shape = geodoer::GeomAlgo::asTopoDS(aBSplineSurf);
	geodoer::TopoDSIO::writeObj(shape, "Geom_BSplineSurface.obj");
}

/**
 * \brief 创建Bezier曲面
 */
void CreateBezierSurface()
{
	// Define a 4x4 grid of points for Bezier surface.
	TColgp_Array2OfPnt aBezierPnts(1, 4, 1, 4);

	for(Standard_Integer i = 1; i <= 4; ++i)
	{
		gp_Pnt aPnt;
		aPnt.SetX(20.0 + 5.0 * i);

		for(Standard_Integer j = 1; j <= 4; ++j)
		{
			aPnt.SetY(20.0 + 5.0 * j);

			if(1 < i && i < 4 && 1 < j && j < 4)
			{
				aPnt.SetZ(5.0);
			}
			else
			{
				aPnt.SetZ(0.0);
			}

			aBezierPnts.SetValue(i, j, aPnt);
		}
	}

	// Define BSpline weights.
	TColStd_Array2OfReal aBezierWeights(1, 4, 1, 4);

	for(Standard_Integer i = 1; i <= 4; ++i)
	{
		for(Standard_Integer j = 1; j <= 4; ++j)
		{
			if(1 < i && i < 4 && 1 < j && j < 4)
			{
				aBezierWeights.SetValue(i, j, 1.5);
			}
			else
			{
				aBezierWeights.SetValue(i, j, 0.5);
			}
		}
	}

	// Create a Bezier surface.
	Handle(Geom_BezierSurface) aBezierSurf = new Geom_BezierSurface(aBezierPnts, aBezierWeights);

	//保存查看
	auto shape = geodoer::GeomAlgo::asTopoDS(aBezierSurf);
	geodoer::TopoDSIO::writeObj(shape, "aBezierSurf.obj");
}

void BuildBezierFromPoints()
{
	//网格点
	geodoer::GPoint pnts[][3] =
	{
		{
			{-200,200,0},
			{0,100,0},
			{200,200,0}
		},
		{
			{-200,0,0},
			{0,0,-100},
			{200,0,0}
		},
		{
			{-200,-200,0},
			{0,-200,0},
			{200,-200,0}
		}
	};

	int rowSize = sizeof(pnts) / sizeof(pnts[0]);
	int colSize = sizeof(pnts[0]) / sizeof(geodoer::GPoint);

	TColgp_Array2OfPnt aBezierPnts(1, rowSize, 1, colSize);

	for(Standard_Integer i = 1; i <= rowSize; ++i)
	{
		for(Standard_Integer j = 1; j <= colSize; ++j)
		{
			auto pnt = pnts[i - 1][j - 1];
			gp_Pnt aPnt(pnt.x(), pnt.y(), pnt.z());
			aBezierPnts.SetValue(i, j, aPnt);
		}
	}

	// Define BSpline weights.
	TColStd_Array2OfReal aBezierWeights(1, rowSize, 1, colSize);

	for(Standard_Integer i = 1; i <= rowSize; ++i)
	{
		for(Standard_Integer j = 1; j <= colSize; ++j)
		{
			if(1 < i && i < rowSize && 1 < j && j < rowSize)
			{
				aBezierWeights.SetValue(i, j, 2); //中间点设置为1.5
			}
			else
			{
				aBezierWeights.SetValue(i, j, 1); //周围点设置为0.5
			}
		}
	}

	Handle(Geom_BezierSurface) aBezierSurf = new Geom_BezierSurface(aBezierPnts, aBezierWeights);
	auto shape = geodoer::GeomAlgo::asTopoDS(aBezierSurf);
	geodoer::TopoDSIO::writeObj(shape, "tmp.obj");
}

int main()
{
	CreateBSplineSurface();
	CreateBezierSurface();

	BuildBezierFromPoints();
	return 0;
}
