#pragma once
#include"geodoer/data/GPolyline.hpp"

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include<TopExp.hxx>

namespace geodoer
{

class TopoDSAlgo
{
public:
	//线段集 构 面
	static TopoDS_Shape buildFace(const std::vector<TopoDS_Edge>& anEdges); //ok

	//from: lvk88/OccTutorial extractStlManifoldBoundaries
	static TopoDS_Shape extractManifoldBoundaries(const TopoDS_Shape& shape);
};

inline TopoDS_Shape TopoDSAlgo::buildFace(const std::vector<TopoDS_Edge>& anEdges)
{
	TopTools_ListOfShape aOrderedEdges;

	for(auto& edge : anEdges)
	{
		aOrderedEdges.Append(edge);
	}

	BRepBuilderAPI_MakeWire aWireMaker;
	aWireMaker.Add(aOrderedEdges);
	const TopoDS_Wire& aWire = TopoDS::Wire(aWireMaker.Shape());
	TopoDS_Face face = BRepBuilderAPI_MakeFace(aWire);
	return face;
}

inline TopoDS_Shape TopoDSAlgo::extractManifoldBoundaries(const TopoDS_Shape& shape)
{
	//For every edge in the triangulation, we pick those that have only one parent face
	//We will store the resulting edges in boundaryEdges
	TopTools_ListOfShape boundaryEdges;
	TopExp_Explorer explorer;

	TopTools_IndexedDataMapOfShapeListOfShape mapOfEdges;
	TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, mapOfEdges);
	std::cout << mapOfEdges.Extent() << " edges found in STL file" << std::endl;

	std::cout << "Extracting manifold edges " << std::endl;
	for (explorer.Init(shape, TopAbs_EDGE, TopAbs_SHAPE); explorer.More(); explorer.Next())
	{
		TopoDS_Edge currentEdge = TopoDS::Edge(explorer.Current());
		const TopTools_ListOfShape& parentsOfCurrentEdge = mapOfEdges.FindFromKey(currentEdge);
		if (parentsOfCurrentEdge.Extent() == 1)
		{
			boundaryEdges.Append(currentEdge);
		}
	}

	std::cout << "Found " << boundaryEdges.Extent() << " boundary edges." << std::endl;

	TopoDS_Compound collectorOfEdges;
	TopoDS_Builder builder;
	builder.MakeCompound(collectorOfEdges);

	for (TopTools_ListIteratorOfListOfShape it(boundaryEdges); it.More(); it.Next())
	{
		builder.Add(collectorOfEdges, it.Value());
	}

	return collectorOfEdges;
}

}
