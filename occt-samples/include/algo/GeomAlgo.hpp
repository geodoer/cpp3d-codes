#pragma once
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <Geom_Plane.hxx>
#include <GeomPlate_Surface.hxx>

#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <Plate_Plate.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

namespace geodoer {

/**
 * \brief Geom相关算法
 */
class GeomAlgo
{
public:
	/**
	 * \brief BSplineSurface to TopoDS_Shape
	 * \param theSurface    Geom_Surface的子类即可（如Geom_BSplineSurface、Geom_BezierSurface）
	 * \return
	 * 测试通过
	 */
	static TopoDS_Shape asTopoDS(Handle(Geom_Surface) theSurface);

	/**
	 * \brief TopoDS_Face to Geom_Surface
	 * \param theShape
	 * \return
	 */
	static Handle(Geom_Surface) asGeom(const TopoDS_Shape& theShape);

	//普通曲线转化为NURBS曲线
	static Handle(Geom_BSplineCurve) asBSpline(Handle(Geom_Curve) ResCurve);

	static Handle(Geom_BSplineSurface) asBSpline(Handle(Geom_Surface) theSurface);

public:
	/**
	 * \brief 点云转Geom_BsplineSurface
	 * \param seqOfXYZ
	 * \return
	 */
	static Handle(Geom_BSplineSurface) buildBSplineSurface(const TColgp_SequenceOfXYZ& seqOfXYZ);
};

inline TopoDS_Shape GeomAlgo::asTopoDS(opencascade::handle<Geom_Surface> theBSurface)
{
	return BRepBuilderAPI_MakeFace(theBSurface, Precision::Confusion()).Face();
}

inline Handle(Geom_Surface) GeomAlgo::asGeom(const TopoDS_Shape& theShape)
{
	Handle_Geom_Surface currentSur;
	TopExp_Explorer Ex;
	Ex.Init(theShape, TopAbs_FACE);
	TopoDS_Face currentVt = TopoDS::Face(Ex.Current());
	currentSur = BRep_Tool::Surface(currentVt);
	return currentSur;
}

inline Handle(Geom_BSplineCurve) GeomAlgo::asBSpline(Handle(Geom_Curve) ResCurve)
{
	Handle(Geom_TrimmedCurve) myTrimmed = new Geom_TrimmedCurve(ResCurve, 0, 1);
	Handle(Geom_BSplineCurve) NurbsCurve = GeomConvert::CurveToBSplineCurve(myTrimmed); //必须指定曲线的类型如Geom_TrimmedCurve
	return NurbsCurve;
}

inline Handle(Geom_BSplineSurface) GeomAlgo::asBSpline(Handle(Geom_Surface) theSurface)
{
	return GeomConvert::SurfaceToBSplineSurface(theSurface);
}

inline opencascade::handle<Geom_BSplineSurface> GeomAlgo::buildBSplineSurface(
	const TColgp_SequenceOfXYZ& seqOfXYZ)
{
	// Build the surface:
	// points are projected on plane z = 0
	// the projection vector for each point is computed
	// These data give the input constraints loaded into plate algorithm

	const Standard_Integer nbPnt = seqOfXYZ.Length();
	Standard_Integer i;

	//Filling plate
	Plate_Plate myPlate;

	for(i = 1; i <= nbPnt; i += 4)
	{
		gp_Vec aVec(0., 0., seqOfXYZ.Value(i).Z());
		gp_XY  pntXY(seqOfXYZ.Value(i).X(), seqOfXYZ.Value(i).Y());
		Plate_PinpointConstraint PCst(pntXY, aVec.XYZ());
		myPlate.Load(PCst); // Load a pinpoint constraint
	}

	myPlate.SolveTI(2, 1.); // Solving plate equations

	if(!myPlate.IsDone())
	{
		return Handle(Geom_BSplineSurface)();
	}

	// Computation of plate surface
	gp_Pnt Or(0, 0, 0.);
	gp_Dir Norm(0., 0., 1.);
	Handle(Geom_Plane) myPlane = new Geom_Plane(Or, Norm);// Plane of normal Oz
	Handle(GeomPlate_Surface) myPlateSurf = new GeomPlate_Surface(myPlane, myPlate);//plate surface

	GeomPlate_MakeApprox aMKS(myPlateSurf, Precision::Approximation(), 4, 7, 0.001, 0);//bspline surface
	return aMKS.Surface();
}

}


/*参考文章
 *1. Open CasCade中的几种类型转换
 *  https://blog.csdn.net/qq_35097289/article/details/103816707
 */