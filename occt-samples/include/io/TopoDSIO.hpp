#pragma once
#include"geodoer/data/GTriangleMesh.hpp"
#include"geodoer/data/GPolyline.hpp"

#include<TopoDS.hxx>
#include<TopoDS_Shape.hxx>
#include<TopoDS_Face.hxx>
#include<TopExp_Explorer.hxx>

#include<Poly_Triangulation.hxx>

#include<BRepMesh_IncrementalMesh.hxx>
#include<BRep_Tool.hxx>
#include<BRepTools.hxx>
#include<BRepBuilderAPI_Sewing.hxx>
#include<BRep_Builder.hxx>
#include<BRepBuilderAPI_MakeVertex.hxx>
#include<BRepBuilderAPI_MakePolygon.hxx>
#include<BRepBuilderAPI_MakeFace.hxx>

#include<Bnd_Box.hxx>
#include<BRepBndLib.hxx>

#include<STEPControl_Writer.hxx>
#include<STEPControl_Reader.hxx>

#include<StlAPI_Writer.hxx>
#include<StlAPI.hxx>

#include <geodoer/io/ObjIOPlugin.hpp>
#include <BRepBuilderAPI_MakeEdge.hxx>

namespace geodoer {


/**
 * \brief OCC TopoDS与外部的IO
 */
class TopoDSIO
{
public: //文件IO
	/**
	 * \brief 
	 * \param theShape 
	 * \param path 如wire.brep
	 * \return 
	 */
	static bool write(const TopoDS_Shape& theShape, const Standard_CString path);
	/**
	 * \brief 保存成STEP格式
	 * \param theShape
	 * \param stepPath 如test.stp
	 * 可使用opencascade-7.5.0\samples\mfc\Import Export查看此文件
	 * 测试通过
	 */
	static void writeSTEP(const TopoDS_Shape& theShape, const Standard_CString stepPath, STEPControl_StepModelType type = STEPControl_AsIs);
	static TopoDS_Shape readSTEP(const Standard_CString stepPath);
	/**
	 * \brief 保存成Obj格式
	 * \param theShape
	 * \param objPath 如"test.obj"
	 * \return
	 * 测试通过
	 */
	static bool writeObj(const TopoDS_Shape& theShape, const std::string& objPath);

	static bool writeSTL(const TopoDS_Shape& theShape, const std::string& stlPath);
	static TopoDS_Shape readSTL(const std::string& stlPath);

public: //数据结构IO
	/**
	 * \brief TopoDS_Shape to TriangleMesh
	 * \param theShape
	 * \param theLinDeflection
	 * \return
	 * 测试通过
	 */
	static GTriangleMesh as(const TopoDS_Shape& theShape);

	/**
	 * \brief TriangleMesh to TopoDS_Shape
	 * \param theMesh
	 * \return
	 */
	static TopoDS_Shape as(const GTriangleMesh& theMesh);

	/**
	 * \brief TopoDS_Edge to Polygline
	 * \param theEdge
	 * \param deflection
	 * \return
	 */
	static GPolyline as(const TopoDS_Edge& theEdge, double deflection = 0.01);

	static std::vector<TopoDS_Edge> asEdges(const GMultiPolyline& lines);

	static gp_Pnt as(const GPoint& point);
};

inline TopoDS_Shape TopoDSIO::readSTEP(const Standard_CString stepPath)
{
	STEPControl_Reader reader;
	reader.ReadFile(stepPath);
	reader.TransferRoots();
	return reader.OneShape();
}

inline std::vector<TopoDS_Edge> TopoDSIO::asEdges(const GMultiPolyline& lines)
{
	std::vector<TopoDS_Edge> edges;

	for(const auto& line : lines)
	{
		for(int i=1,size= line.size(); i<size; ++i)
		{
			auto A = line[i - 1];
			auto B = line[i];

			auto edge = BRepBuilderAPI_MakeEdge(as(A), as(B));

			if(edge.IsDone())
			{
				edges.emplace_back(edge);
			}
		}
	}

	return edges;
}

inline gp_Pnt TopoDSIO::as(const GPoint& point)
{
	return gp_Pnt(point.x(), point.y(), point.z());
}

inline bool TopoDSIO::write(const TopoDS_Shape& theShape, const Standard_CString path)
{
	return BRepTools::Write(theShape, path);
}

inline void TopoDSIO::writeSTEP(const TopoDS_Shape& theShape, const Standard_CString stepPath, STEPControl_StepModelType type)
{
	STEPControl_Writer writer;
	writer.Transfer(theShape, type);
	writer.Write(stepPath);
}

inline bool TopoDSIO::writeObj(const TopoDS_Shape& theShape, const std::string& objPath)
{
	auto mesh = as(theShape);
	return geodoer::ObjIOPlugin::write(mesh, objPath);
}

inline bool TopoDSIO::writeSTL(const TopoDS_Shape& theShape, const std::string& stlPath)
{
	try
	{
		StlAPI_Writer writer;

		return writer.Write(theShape, stlPath.data());
	}
	catch (Standard_ConstructionError& e)
	{
		return false;
	}
}

inline TopoDS_Shape TopoDSIO::readSTL(const std::string& stlPath)
{
	TopoDS_Shape stlShape;
	StlAPI::Read(stlShape, stlPath.data());
	return stlShape;
}

inline GTriangleMesh TopoDSIO::as(const TopoDS_Shape& theShape)
{
	GTriangleMesh mesh;

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

inline TopoDS_Shape TopoDSIO::as(const GTriangleMesh& theMesh)
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

inline GPolyline TopoDSIO::as(const TopoDS_Edge& theEdge, double deflection)
{
	GPolyline line;

	BRepMesh_IncrementalMesh(theEdge, deflection);
	TopLoc_Location aLoc;
	auto polyline = BRep_Tool::Polygon3D(theEdge, aLoc);

	gp_Trsf aTrsf = aLoc.Transformation();

	for(int i = 1; i < polyline->NbNodes(); i++)
	{
		gp_Pnt aPnt = polyline->Nodes().Value(i);
		aPnt.Transform(aTrsf);

		line.emplace_back(GPoint({ aPnt.X(), aPnt.Y(), aPnt.Z() }));
	}

	return line;
}

}


/*参考文章
 *1. 解析OCC（TopoDS_Edge、Geom_Surface、TopoDS_Shape），生成OSG节点
 *      https://blog.csdn.net/weixin_43712770/article/details/100973602
 */