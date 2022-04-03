#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <ElSLib.hxx>
#include <GccAna_Lin2d2Tan.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <STEPControl_Writer.hxx>

int main()
{
	//为BSpline表面定义一个4x4的网格点
	// Define a 4x4 grid of points for BSpline surface.
	TColgp_Array2OfPnt aBSplinePnts(1, 4, 1, 4);
		//TColgp_Array2OfPnt Point类型的二维数组
		//传入四个参数：row的范围；col的范围

	for(Standard_Integer i = 1; i <= 4; ++i)
	{
		gp_Pnt aPnt;
		aPnt.SetX(5.0 * i);

		for(Standard_Integer j = 1; j <= 4; ++j)
		{
			aPnt.SetY(5.0 * j);

			if(1 < i && i < 4 && 1 < j && j < 4)
			{
				aPnt.SetZ(5.0);
			}
			else
			{
				aPnt.SetZ(0.0);
			}

			aBSplinePnts.SetValue(i, j, aPnt);
		}
	}

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
	std::cout << "Geom_BSplineSurface was created in red" << std::endl;

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
	std::cout << "Geom_BezierSurface was created in green" << std::endl;

	return 0;
}
