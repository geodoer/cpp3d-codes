#include"geodoer/data/GPolyline.hpp"
#include"geodoer/io/ObjIOPlugin.hpp"

#include "io/TopoDSIO.hpp"
#include "algo/TopoDSAlgo.hpp"
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

//测试失败。达不到底面倾斜的效果，角度变大就会build失败
void DraftAngleTest(TopoDS_Shape prism)
{
	//在形状上添加锥度的变换
	BRepOffsetAPI_DraftAngle adraft(prism);

	TopExp_Explorer Ex;

	for (Ex.Init(prism, TopAbs_FACE); Ex.More(); Ex.Next()) //遍历面
	{
		TopoDS_Face F = TopoDS::Face(Ex.Current());
		Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
		gp_Pln apln = surf->Pln();
		gp_Dir dirF = apln.Axis().Direction();

		if (dirF.IsNormal(gp_Dir(0., 0., -1.), Precision::Angular()))
		{
			adraft.Add(F, gp_Dir(0., 0., -1.), 5 * M_PI / 180, gp_Pln(gp::XOY()));
		}
	}

	TopoDS_Shape resultShape = adraft.Shape();
	geodoer::TopoDSIO::write(resultShape, "[Z-RiverModeling]DraftAngle.brep");
	geodoer::TopoDSIO::writeObj(resultShape, "[Z-RiverModeling]DraftAngle.obj");
}

int main()
{
	geodoer::GMultiPolyline lines;
	geodoer::ObjIOPlugin::read(R"(E:\cpp\cpp3d-codes\lines.obj)", lines);

	auto box = lines.getAABB();
	auto center = box.center();
	lines.offset({ center[0], center[1], center[2] });

	auto edges = geodoer::TopoDSIO::asEdges(lines);
	auto face = geodoer::TopoDSAlgo::buildFace(edges);

	if (face.IsNull())
	{
		std::cout << "Build Face error" << std::endl;
	}

	geodoer::TopoDSIO::write(face, "[Z-RiverModeling]edges_to_face.brep");

	auto pnt = lines[0][0];
	gp_Vec aPrismVec(0, 0, pnt.z()-500); //扫描的方向
	TopoDS_Shape prism = BRepPrimAPI_MakePrism(face, aPrismVec); //将aWire在aPrismVec方向、长度为aPrismVec的模进行扫描
	geodoer::TopoDSIO::write(prism, "[Z-RiverModeling]MakePrism.brep");
	//geodoer::TopoDSIO::writeObj(prism, "[Z-RiverModeling]MakePrism.obj");


	//效果不理想
	//DraftAngleTest(prism);

	//拿侧面；往里靠；倾斜的度数


	return 0;
}
