#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

#include "io/TopoDSIO.hpp"

//一、根据锥角使模型产生形变
void BRepOffsetAPI_DraftAngle_Sample()
{
	//一个盒子
	TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.);

	//在形状上添加锥度的变换
	BRepOffsetAPI_DraftAngle adraft(S);

	TopExp_Explorer Ex;

	for (Ex.Init(S, TopAbs_FACE); Ex.More(); Ex.Next()) //遍历面
	{
		TopoDS_Face F = TopoDS::Face(Ex.Current());

		//计算法向量
		Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
		gp_Pln apln = surf->Pln();				//surf所在平面
		gp_Dir dirF = apln.Axis().Direction(); //法向量
		//if(F.Orientation() == TopAbs_REVERSED)
		//{
		//	dirF = -dirF;
		//}

		//dirF是(0,0,1)的法向量 => dirF与(0,0,1)是成90°
		if (dirF.IsNormal(gp_Dir(0., 0., 1.), Precision::Angular()))
		{
			adraft.Add(F, gp_Dir(0., 0., 1.), 15.*M_PI / 180, gp_Pln(gp::XOY()));
		}
	}

	TopoDS_Shape resultShape = adraft.Shape();
	geodoer::TopoDSIO::writeObj(resultShape, "D-Sweep-DraftAngle.obj");
}


int main()
{

	return 0;
}