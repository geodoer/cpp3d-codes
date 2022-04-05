//from: https://github.com/lvk88/OccTutorial.git
#include <iostream>
#include <iomanip>
#include "BRepPrimAPI_MakeCylinder.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"

#include "io/TopoDSIO.hpp"

#include "geodoer/io/ObjIOPlugin.hpp"

//创建Box、圆柱；Cut；计算Shape的属性
void Create_Cut_Property()
{
	//创建一个简单的Box
	gp_Pnt lowerLeftCornerOfBox(-50.0, -50.0, 0.0);			//Box的中点
	BRepPrimAPI_MakeBox boxMaker(lowerLeftCornerOfBox, 100, 100, 50);	//100x100x50的大小
	TopoDS_Shape box = boxMaker.Shape();

	//创建一个圆柱
	BRepPrimAPI_MakeCylinder cylinderMaker(25.0, 50.0);	//半径25，高度50
	TopoDS_Shape cylinder = cylinderMaker.Shape();

	//布尔运算：box - cylinder
	BRepAlgoAPI_Cut cutMaker(box, cylinder);
	TopoDS_Shape boxWithHole = cutMaker.Shape();

	//将TopoDS_Shape写出
	geodoer::TopoDSIO::writeSTEP(boxWithHole, "boxWithHole.stp");
	std::cout << "Created box with hole, file is written to boxWithHole.stp" << std::endl;

	//输出成OBJ
	auto mesh = geodoer::TopoDSIO::as(boxWithHole);
	geodoer::ObjIOPlugin::write(mesh, "test.obj");

	//为Shape计算属性
	//We compute some volumetric properties of the resulting shape
	GProp_GProps volumeProperties;
	BRepGProp::VolumeProperties(boxWithHole, volumeProperties);

	//计算模型体积
	std::cout << std::setprecision(14) << "Volume of the model is: " << volumeProperties.Mass() << std::endl;

	//计算质心
	std::cout << "Center of mass is: " << volumeProperties.CentreOfMass().X() << " " << volumeProperties.CentreOfMass().Y() << " " << volumeProperties.CentreOfMass().Z() << std::endl;

	//计算惯性矩阵
	//Compute the matrix of inertia
	gp_Mat inertiaMatrix = volumeProperties.MatrixOfInertia();
	std::cout << "Matrix of inertia: " << std::endl;
	for (int i = 1; i <= 3; ++i) {
		for (int j = 1; j <= 3; ++j) {
			std::cout << inertiaMatrix(i, j) << "\t";
		}
		std::cout << std::endl;
	}
}

int main(int argc, char *argv[])
{
	Create_Cut_Property();

	return 0;
}