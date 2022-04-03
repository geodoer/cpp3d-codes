#pragma once
#include"data/UTriangleMesh.hpp"
#include"data/UPolyline.hpp"
#include"io/UTriangleMeshIO.hpp"

#include<TopoDS.hxx>
#include<TopoDS_Shape.hxx>
#include<TopoDS_Face.hxx>
#include<TopExp_Explorer.hxx>

#include<Poly_Triangulation.hxx>

#include<BRepMesh_IncrementalMesh.hxx>
#include<BRep_Tool.hxx>
#include<BRepBuilderAPI_Sewing.hxx>
#include<BRep_Builder.hxx>
#include<BRepBuilderAPI_MakeVertex.hxx>
#include<BRepBuilderAPI_MakePolygon.hxx>
#include<BRepBuilderAPI_MakeFace.hxx>

#include<Bnd_Box.hxx>
#include<BRepBndLib.hxx>

namespace tu {

class TopoDSShapeIO
{
public:
	/**
	 * \brief
	 * \param theShape
	 * \param objPath
	 * \return
	 * 测试通过
	 */
	static bool writeObj(const TopoDS_Shape& theShape, const std::string& objPath);

public:
	/**
	 * \brief TopoDS_Shape to TriangleMesh
	 * \param theShape
	 * \param theLinDeflection
	 * \return
	 * 测试通过
	 */
	static UTriangleMesh as(const TopoDS_Shape& theShape);

	/**
	 * \brief TriangleMesh to TopoDS_Shape
	 * \param theMesh
	 * \return
	 */
	static TopoDS_Shape as(const UTriangleMesh& theMesh);

public:
	/**
	 * \brief TopoDS_Edge to Polygline
	 * \param theEdge
	 * \param deflection
	 * \return
	 */
	static UPolyline as(const TopoDS_Edge& theEdge, double deflection = 0.01);
};

inline bool TopoDSShapeIO::writeObj(const TopoDS_Shape& theShape, const std::string& objPath)
{
	return UTriangleMeshIO::writeObj(as(theShape), objPath);
}

inline UTriangleMesh TopoDSShapeIO::as(const TopoDS_Shape& theShape)
{
	UTriangleMesh mesh;

	Standard_Real aDeflection = 0.5;
	{
		Bnd_Box bb;
		BRepBndLib::Add(theShape, bb, Standard_False);
		Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
		bb.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
		aDeflection = std::max(aXmax - aXmin, std::max(aYmax - aYmin, aZmax - aZmin)) * 0.0004;
	}
	BRepMesh_IncrementalMesh(theShape, aDeflection);    //离散网格
	//BRepMesh::Mesh(aShape, 3);//v6.8.0后删除了

	Standard_Integer aNodeOffset = 0; //点的偏移值

	for(TopExp_Explorer faceExplorer(theShape, TopAbs_FACE); faceExplorer.More(); faceExplorer.Next())  //遍历每个面
	{
		TopoDS_Face aFace = TopoDS::Face(faceExplorer.Current());

		TopLoc_Location aLoc;
		Handle(Poly_Triangulation) aTriFace = BRep_Tool::Triangulation(aFace, aLoc);

		if(aTriFace.IsNull())
		{
			continue;
		}

		// copy nodes
		const TColgp_Array1OfPnt& aNodes = aTriFace->Nodes();
		gp_Trsf aTrsf = aLoc.Transformation();

		for(Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
		{
			gp_Pnt aPnt = aNodes(aNodeIter);
			aPnt.Transform(aTrsf);

			mesh.addVertex(aPnt.X(), aPnt.Y(), aPnt.Z());
		}

		// copy triangles
		auto aTriangles = aTriFace->Triangles();

		for(Standard_Integer aTriIter = aTriangles.Lower(); aTriIter <= aTriangles.Upper(); ++aTriIter)
		{
			Poly_Triangle aTri = aTriangles(aTriIter);

			Standard_Integer anId[3];
			aTri.Get(anId[0], anId[1], anId[2]);

			if(aFace.Orientation() == TopAbs_REVERSED)
			{
				auto aTmpIdx = anId[1];
				anId[1] = anId[2];
				anId[2] = aTmpIdx;
			}

			//anId是aTriFace中的索引
			//在Mesh中需要加上一个offset
			anId[0] += aNodeOffset;
			anId[1] += aNodeOffset;
			anId[2] += aNodeOffset;

			mesh.addFace(anId[0]-1, anId[1]-1, anId[2]-1); //occ的索引是从1开始的
		}

		aNodeOffset += aNodes.Size();
	}

	return mesh;
}

inline TopoDS_Shape TopoDSShapeIO::as(const UTriangleMesh& theMesh)
{
	TopoDS_Compound aComp;          //复合体
	BRep_Builder BuildTool;
	BuildTool.MakeCompound(aComp);

	for(auto size=theMesh.faceSize(), i(0); i<size; ++i)
	{
		////此三角形的三个点
		gp_Pnt pnts[3];

		auto aTri = theMesh.face(i);

		for(int j(0); j < 3; ++j)
		{
			pnts[j] = gp_Pnt(aTri[j][0], aTri[j][1], aTri[j][2]);
		}

		if(pnts[0].IsEqual(pnts[1], 0.0)
			|| pnts[0].IsEqual(pnts[2], 0.0)
			|| pnts[1].IsEqual(pnts[2], 0.0))
		{
			continue;
		}

		////构造顶点（点是几何的概念，顶点是拓扑的概念）
		TopoDS_Vertex aTriVertexes[3];

		for(int j(0); j < 3; ++j)
		{
			aTriVertexes[j] = BRepBuilderAPI_MakeVertex(pnts[j]);
		}

		////闭合线
		TopoDS_Wire aWire = BRepBuilderAPI_MakePolygon(aTriVertexes[0], aTriVertexes[1], aTriVertexes[2], Standard_True);

		if(aWire.IsNull())
		{
			continue;
		}

		// 通过闭合的线构造面
		TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire);

		if(aFace.IsNull())
		{
			continue;
		}

		BuildTool.Add(aComp, aFace); //将面加入到复合体中
	}

	BRepBuilderAPI_Sewing aSewingTool;
	aSewingTool.Init(1.0e-06, Standard_True);
	aSewingTool.Load(aComp);
	aSewingTool.Perform();

	return aSewingTool.SewedShape();
}

inline UPolyline TopoDSShapeIO::as(const TopoDS_Edge& theEdge, double deflection)
{
	UPolyline line;

	BRepMesh_IncrementalMesh(theEdge, deflection);
	TopLoc_Location aLoc;
	auto polyline = BRep_Tool::Polygon3D(theEdge, aLoc);

	gp_Trsf aTrsf = aLoc.Transformation();

	for(int i = 1; i < polyline->NbNodes(); i++)
	{
		gp_Pnt aPnt = polyline->Nodes().Value(i);
		aPnt.Transform(aTrsf);

		line.emplace_back(UPoint({aPnt.X(), aPnt.Y(), aPnt.Z()}));
	}

	return line;
}

}


/*参考文章
 *1. 解析OCC（TopoDS_Edge、Geom_Surface、TopoDS_Shape），生成OSG节点
 *      https://blog.csdn.net/weixin_43712770/article/details/100973602
 */