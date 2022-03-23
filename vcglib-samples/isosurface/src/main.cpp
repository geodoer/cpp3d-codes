#include <iostream>

#include"vcg/complex/complex.h"
#include"wrap/callback.h"

#include"vcg/complex/algorithms/create/mc_trivial_walker.h"
#include"vcg/complex/algorithms/create/marching_cubes.h"
#include"muParser.h"
#include <wrap/io_trimesh/export.h>

#include"meshlab/cmesh.h"
#include"meshlab/string_conversion.h"

using namespace vcg;
using namespace mu;

int main()
{
	CMeshO cm;

	SimpleVolume<SimpleVoxel<float>> volume;

	typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel<float>>> MyWalker;
	typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>                         MyMarchingCubes;
	MyWalker                                                                  walker;

	Box3f RangeBBox;
	RangeBBox.min[0] = -1; //par.getFloat("minX");
	RangeBBox.min[1] = -1; //par.getFloat("minY");
	RangeBBox.min[2] = -1; //par.getFloat("minZ");
	RangeBBox.max[0] = 1;  //par.getFloat("maxX");
	RangeBBox.max[1] = 1;  //par.getFloat("maxY");
	RangeBBox.max[2] = 1;  //par.getFloat("maxZ");
	double  step = 0.05;	//par.getFloat("voxelSize");
	Point3i siz = Point3i::Construct((RangeBBox.max - RangeBBox.min) * (1.0 / step));

	Parser p;
	double x, y, z;
	p.DefineVar(conversion::fromStringToWString("x"), &x);
	p.DefineVar(conversion::fromStringToWString("y"), &y);
	p.DefineVar(conversion::fromStringToWString("z"), &z);
	std::string expr = "x*x+y*y+z*z-0.5";
	p.SetExpr(conversion::fromStringToWString(expr));

	volume.Init(siz, RangeBBox);
	for (double i = 0; i < siz[0]; i++)
		for (double j = 0; j < siz[1]; j++)
			for (double k = 0; k < siz[2]; k++) {
				x = RangeBBox.min[0] + step * i;
				y = RangeBBox.min[1] + step * j;
				z = RangeBBox.min[2] + step * k;
				try {
					volume.Val(i, j, k) = p.Eval();
				}
				catch (Parser::exception_type& e) {
					throw std::exception(conversion::fromWStringToString(e.GetMsg()).c_str());
				}
			}

	// MARCHING CUBES
	MyMarchingCubes mc(cm, walker);
	walker.BuildMesh<MyMarchingCubes>(cm, volume, mc, 0);
	//    Matrix44m tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
	//    Matrix44m sc; sc.SetIdentity(); sc.SetScale(step,step,step);
	//    tr=tr*sc;

	//    tri::UpdatePosition<CMeshO>::Matrix(m.cm,tr);
	tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(cm);
	tri::UpdateBounding<CMeshO>::Box(cm); // updates bounding box


	// definition of type MyMesh
	vcg::tri::io::ExporterPLY<CMeshO>::Save(cm, "namefile_to_save.ply");

	return 0;
}