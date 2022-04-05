#pragma once
#include"geodoer/data/GTriangle.hpp"

namespace geodoer {

class GTriangleMesh
{
protected:
	std::vector<GPoint>	m_vertices;	//顶点
	std::vector<int>	m_indices;	//面索引（从0开始）

public:
	int vertexSize() const;
	int addVertex(const double x, const double y, const double z);
	GPoint vertex(int index) const;
	GPoint& vertex(int index);

	int faceSize() const;
	int addFace(const int idx1,const int idx2,const int idx3);
	GTriangle face(int index) const;

	int indiceSize() const;
	int indice(int index) const;
	int& indice(int index);
};

inline int GTriangleMesh::vertexSize() const
{
	return m_vertices.size();
}

inline int GTriangleMesh::addVertex(const double x, const double y, const double z)
{
	m_vertices.emplace_back(GPoint({x,y,z}));
	return m_vertices.size() - 1;
}

inline GPoint GTriangleMesh::vertex(int index) const
{
	return m_vertices[index];
}

inline GPoint& GTriangleMesh::vertex(int index)
{
	return m_vertices[index];
}

inline int GTriangleMesh::faceSize() const
{
	return m_indices.size() / 3;
}

inline int GTriangleMesh::addFace(const int idx1, const int idx2, const int idx3)
{
	m_indices.emplace_back(idx1);
	m_indices.emplace_back(idx2);
	m_indices.emplace_back(idx3);
	return m_indices.size() / 3 - 1;
}

inline GTriangle GTriangleMesh::face(int index) const
{
	index *= 3;
	return GTriangle({
		m_vertices[m_indices[index++]],
		m_vertices[m_indices[index++]],
		m_vertices[m_indices[index]]
	});
}

inline int GTriangleMesh::indiceSize() const
{
	return m_indices.size();
}

inline int GTriangleMesh::indice(int index) const
{
	return m_indices[index];
}

inline int& GTriangleMesh::indice(int index)
{
	return m_indices[index];
}

}
