#pragma once
#include"geodoer/data/GTriangleMesh.hpp"
#include"geodoer/data/GPolyline.hpp"

#include<string>
#include<fstream>

namespace geodoer
{

/**
 * \brief Obj格式读写插件
 *		可使用Meshlab打开
 */
class ObjIOPlugin
{
public:
	static bool write(const GPoints& points, const std::string& objPath);

	static bool write(const GPolyline& line, const std::string& objPath);
	
	//测试通过
	static bool write(const GTriangleMesh& mesh, const std::string& objPath);

public:
	static bool read(const std::string& objPath, GPoints& points);
};

inline bool ObjIOPlugin::write(const GPoints& points, const std::string& objPath)
{
	std::ofstream objFile(objPath);

	if (!objFile.is_open())
	{
		return false;
	}

	for(auto it = points.begin(); it != points.end(); ++it)
	{
		objFile << "v " << (*it)[0] << " " << (*it)[1] << " " << (*it)[2] << std::endl;
	}

	objFile.close();
	return true;
}

inline bool ObjIOPlugin::write(const GPolyline& line, const std::string& objPath)
{
	std::ofstream ofile(objPath);

	if (!ofile.is_open())
	{
		return false;
	}

	for (auto it = line.begin(); it != line.end(); ++it)
	{
		ofile << "v " << it->x() << " " << it->y() << " " << it->z() << std::endl;
	}

	for (int i = 1; i < line.size(); ++i)
	{
		ofile << "l " << i << " " << i + 1 << std::endl;
	}

	ofile << "l " << line.size() << " 1" << std::endl;

	ofile.close();
}

inline bool ObjIOPlugin::write(const GTriangleMesh& mesh, const std::string& objPath)
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

inline bool ObjIOPlugin::read(const std::string& objPath, GPoints& points)
{
	std::ifstream in(objPath);

	if (!in.is_open())
	{
		return false;
	}
	
	for (std::string tmp; getline(in, tmp); )
	{
		std::istringstream lineIn(tmp);//转化成数据流

		std::string flag;
		lineIn >> flag;

		if (flag[0] != 'v')
		{
			continue;
		}

		GPoint pnt;
		for (int i = 0; i<3 && !lineIn.eof(); ++i)
		{
			lineIn >> pnt[i];
		}

		points.emplace_back(pnt);
	}

	in.close();
	return true;
}

}
