#pragma once
#include"geodoer/data/GTriangleMesh.hpp"
#include"geodoer/data/GPolyline.hpp"

#include<string>
#include<sstream>
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
	static bool write(const GMultiPolyline& polylines, const std::string& objPath);	//ok
	static bool write(const GTriangleMesh& mesh, const std::string& objPath);		//ok

public:
	static bool read(const std::string& objPath, GPoints& points);
	static bool read(const std::string& objPath, GMultiPolyline& polylines);	//ok
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
	return true;
}

inline bool ObjIOPlugin::write(const GMultiPolyline& polylines, const std::string& objPath)
{
	std::ofstream ofile(objPath);

	if (!ofile.is_open())
	{
		return false;
	}

	int vCnt = 0;

	for(const auto& line : polylines)
	{
		//point
		for(const auto& pnt : line)
		{
			ofile << "v " << pnt.x() << " " << pnt.y() << " " << pnt.z() << std::endl;
		}

		//segment indices
		//Meshlab不支持Polyline，只支持线段
		for(int i=1; i<line.size(); ++i)
		{
			ofile << "l " << i + vCnt << " " << i + vCnt + 1 << std::endl; //obj的下标从1开始
		}

		vCnt += line.size();
	}

	ofile.close();
	return true;
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

inline bool ObjIOPlugin::read(const std::string& objPath, GMultiPolyline& polylines)
{
	std::ifstream in(objPath);

	if (!in.is_open())
	{
		return false;
	}
	
	//read points
	GPoints points;

	for (std::string tmp; getline(in, tmp); )
	{
		std::istringstream lineIn(tmp);//转化成数据流

		std::string flag;
		lineIn >> flag;

		if (flag != "v")
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

	//read liens
	in.clear();
	in.seekg(0, std::ios::beg); //beg(begin)。到达文件尾巴后，再调用seekg无效。所以要先in.clear()

	for (std::string tmp; getline(in, tmp); )
	{
		std::istringstream lineIn(tmp);//转化成数据流

		std::string flag;
		lineIn >> flag;

		if (flag[0] != 'l')
		{
			continue;
		}

		GPolyline line;
		while(!lineIn.eof())
		{
			int idx;
			lineIn >> idx;
			line.emplace_back(points[idx-1]);
		}

		polylines.emplace_back(line);
	}

	in.close();
	return true;
}

}
