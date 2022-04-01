//from: https://github.com/lvk88/OccTutorial.git
#include "gp_Pnt.hxx"
#include "gp_Ax1.hxx"
#include "gp_Trsf.hxx"

#include<iostream>

//创建Point；旋转；镜像
void Point_Rotate_Mirrored()
{
	//创建两个Point类，具有(x,y,z)坐标
	gp_Pnt pnt1(0.0, 0.0, 0.0);
	gp_Pnt pnt2(1.0, 1.0, 1.0);

	//获取它们的坐标，与计算距离
	std::cout << "pnt1: " << pnt1.X() << " " << pnt1.Y() << " " << pnt1.Z() << std::endl;
	std::cout << "pnt2: " << pnt2.X() << " " << pnt2.Y() << " " << pnt2.Z() << std::endl;
	std::cout << "Distance between pnt1 and pnt2: " << pnt1.Distance(pnt2) << std::endl;

	//将pnt1绕着点point2旋转90度
	gp_Trsf rotation;
	gp_Ax1 rotationAxis(gp_Pnt(1.0, 1.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
	rotation.SetRotation(rotationAxis, 3.1415926 / 2.0);
	pnt1.Transform(rotation);
	std::cout << "pnt1 after rotation: " << pnt1.X() << " " << pnt1.Y() << " " << pnt1.Z() << std::endl;

	//计算pnt1在y轴上的镜像
	gp_Ax1 axisOfSymmetry(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
	gp_Pnt pnt3 = pnt1.Mirrored(axisOfSymmetry);
	std::cout << "pnt3 after mirroring: " << pnt3.X() << " " << pnt3.Y() << " " << pnt3.Z() << std::endl;

}

int main(int argc, char *argv[])
{
	Point_Rotate_Mirrored();

	return 0;
}
