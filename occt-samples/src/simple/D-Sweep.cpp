
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>

#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>

#include "io/TopoDSIO.hpp"
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>

//一、根据锥角使模型产生形变
void BRepOffsetAPI_DraftAngle_Sample()
{
	//一个盒子
	TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.);

	//在形状上添加锥度的变换
	BRepOffsetAPI_DraftAngle adraft(S);

	TopExp_Explorer Ex;

	for(Ex.Init(S, TopAbs_FACE); Ex.More(); Ex.Next()) //遍历面
	{
		TopoDS_Face F = TopoDS::Face(Ex.Current());
		Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
		gp_Pln apln = surf->Pln();
		gp_Dir dirF = apln.Axis().Direction();

		if(dirF.IsNormal(gp_Dir(0., 0., 1.), Precision::Angular()))
		{
			adraft.Add(F, gp_Dir(0., 0., 1.), 15.*M_PI / 180, gp_Pln(gp::XOY()));
		}
	}

	TopoDS_Shape resultShape = adraft.Shape();
	geodoer::TopoDSIO::writeObj(resultShape, "D-Sweep-DraftAngle.obj");
}

//二、沿方向拉伸，生成线性扫掠
//1. TopoDS_Wire(线)拉伸成Face
void BRepPrimAPI_MakePrism_Wire_Sample()
{
	gp_Pnt aPnt1(0, 1, 0);
	gp_Pnt aPnt2(1, 0, 0);
	gp_Pnt aPnt3(3, 2., 0);

	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt2, aPnt3);

	TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aSegment2);

	TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2);

	gp_Vec aPrismVec(10, 50, 0); //扫描的方向
	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(aWire, aPrismVec); //将aWire在aPrismVec方向、长度为aPrismVec的模进行扫描

	geodoer::TopoDSIO::writeObj(myBody, "D-Sweep-WireToFace.obj");
}
//2. TopoDS_Face（面）拉伸成Solid
void BRepPrimAPI_MakePrism_Face_Sample()
{
	//控制点
	gp_Pnt aPnt1(0, 1, 0);
	gp_Pnt aPnt2(1, 0, 0);
	gp_Pnt aPnt3(3, 2., 0);

	//线段Geometry
	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt2, aPnt3);
	Handle(Geom_TrimmedCurve) aSegment3 = GC_MakeSegment(aPnt1, aPnt3);

	//拓扑Shape
	TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aSegment2);
	TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment3);

	//线框
	TopoDS_Wire aProfile = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

	//生成面
	TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aProfile);

	gp_Vec aPrismVec(1, 0, 10); //扫描的方向
	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(aFace, aPrismVec); //将aWire在aPrismVec方向、长度为aPrismVec的模进行扫描
	geodoer::TopoDSIO::writeObj(myBody, "D-Sweep-FaceToSolid.obj");
}

//三、沿路径拉伸
//1. 管道
void BRepOffsetAPI_MakePipe_Sample()
{
	gp_Pnt aPnt1(0, 0, 0);
	gp_Pnt aPnt2(3, 0, 0);
	gp_Pnt aPnt3(3, 0, 2);
	gp_Pnt aPnt4(0, 0, 2);
	gp_Pnt aPnt5(5, 2., 0);
	gp_Pnt aPnt6(5, 2., 2);
	//路径
	TopoDS_Edge segEdge = BRepBuilderAPI_MakeEdge(aPnt1, aPnt2);
	TopoDS_Edge segEdge1 = BRepBuilderAPI_MakeEdge(aPnt2, aPnt5);
	TopoDS_Wire arcWire = BRepBuilderAPI_MakeWire(segEdge, segEdge1);
	//横截面
	gp_Pnt aPnt10(0, -1, -1);
	gp_Pnt aPnt11(0, 1, -1);
	gp_Pnt aPnt12(0, 1, 1);
	gp_Pnt aPnt13(0, -1, 1);
	TopoDS_Wire awire20 = BRepBuilderAPI_MakePolygon(aPnt10, aPnt11, aPnt12, aPnt13, true);
	TopoDS_Face aface20 = BRepBuilderAPI_MakeFace(awire20);

	TopoDS_Shape rstShape = BRepOffsetAPI_MakePipe(arcWire, aface20);
	geodoer::TopoDSIO::writeObj(rstShape, "D-Sweep-MakePipe.obj");
}

int main()
{
	BRepOffsetAPI_DraftAngle_Sample();
	BRepPrimAPI_MakePrism_Wire_Sample();
	BRepPrimAPI_MakePrism_Face_Sample();
	BRepOffsetAPI_MakePipe_Sample();

	return 0;
}