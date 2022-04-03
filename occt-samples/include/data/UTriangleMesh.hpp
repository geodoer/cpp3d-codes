#pragma once
#include<vector>

#include"data/UTriangle.hpp"

namespace tu {

class UTriangleMesh
{
protected:
	std::vector<UPoint>	m_vertices;	//顶点
	std::vector<int>	m_indices;	//面索引（从0开始）

public:
	int vertexSize() const;
	int addVertex(const double x, const double y, const double z);
	UPoint vertex(int index) const;
	UPoint& vertex(int index);

	int faceSize() const;
	int addFace(const int idx1,const int idx2,const int idx3);
	UTriangle face(int index) const;

	int indiceSize() const;
	int indice(int index) const;
	int& indice(int index);
};

inline int UTriangleMesh::vertexSize() const
{
	return m_vertices.size();
}

inline int UTriangleMesh::addVertex(const double x, const double y, const double z)
{
	m_vertices.emplace_back(UPoint({x,y,z}));
	return m_vertices.size() - 1;
}

inline UPoint UTriangleMesh::vertex(int index) const
{
	return m_vertices[index];
}

inline UPoint& UTriangleMesh::vertex(int index)
{
	return m_vertices[index];
}

inline int UTriangleMesh::faceSize() const
{
	return m_indices.size() / 3;
}

inline int UTriangleMesh::addFace(const int idx1, const int idx2, const int idx3)
{
	m_indices.emplace_back(idx1);
	m_indices.emplace_back(idx2);
	m_indices.emplace_back(idx3);
	return m_indices.size() / 3 - 1;
}

inline UTriangle UTriangleMesh::face(int index) const
{
	index *= 3;
	return UTriangle({
		m_vertices[m_indices[index++]],
		m_vertices[m_indices[index++]],
		m_vertices[m_indices[index]]
	});
}

inline int UTriangleMesh::indiceSize() const
{
	return m_indices.size();
}

inline int UTriangleMesh::indice(int index) const
{
	return m_indices[index];
}

inline int& UTriangleMesh::indice(int index)
{
	return m_indices[index];
}

}
