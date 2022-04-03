#pragma once
#include"data/UTriangleMesh.hpp"
#include <string>

#include <fstream>

namespace tu
{
class UTriangleMeshIO
{
public:
	/**
	 * \brief 将Mesh写成Obj的格式
	 * \param mesh 
	 * \param objPath 
	 * \return 
	 * 测试通过
	 */
	static bool writeObj(const UTriangleMesh& mesh, const std::string& objPath);
};

inline bool UTriangleMeshIO::writeObj(const UTriangleMesh& mesh, const std::string& objPath)
{
	std::ofstream objFile(objPath);

	if(!objFile.is_open())
	{
		return false;
	}

	for(auto size=mesh.vertexSize(), i=0; i<size; ++i)
	{
		auto pnt = mesh.vertex(i);
		objFile << "v " << pnt[0] << " " << pnt[1] << " " << pnt[2] << std::endl;
	}

	for(auto size=mesh.indiceSize(), i=0; i<size; )
	{
		const auto idx1 = mesh.indice(i++);
		const auto idx2 = mesh.indice(i++);
		const auto idx3 = mesh.indice(i++);

		//obj中的face索引从1开始
		objFile << "f " <<  idx1+1 << " " << idx2+1 << " " << idx3+1 << std::endl;
	}

	objFile.close();
	return true;
}

}
