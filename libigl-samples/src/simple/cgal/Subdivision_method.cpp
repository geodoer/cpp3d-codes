#include <iostream>
#include <CGAL/Timer.h>

//CGAL Polyhedron_3
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>

//CGAL 细分算法
#include <CGAL/subdivision_method_3.h>

//igl工具
#include <igl/copyleft/cgal/polyhedron_to_mesh.h>
#include <igl/copyleft/cgal/mesh_to_polyhedron.h>

//igl IO
#include <igl/readOFF.h>
#include <igl/writeOFF.h>

using namespace std;
using namespace CGAL;
namespace params = CGAL::parameters;

int main(int argc, char **argv)
{
	const std::string in_file = R"(C:\Users\2107\Desktop\bunny.off)";					//输入Mesh(.off)
	int d = 1;																			//细分深度the depth of the subdivision
	const char* out_file = R"(C:\Users\2107\Desktop\Sqrt3_subdivision_result.off)";		//输出Mesh(.off)

	///off -> igl
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	igl::readOFF(in_file, V, F);

	///igl -> cgal
	CGAL::Polyhedron_3<
		CGAL::Simple_cartesian<double>,
		CGAL::Polyhedron_items_with_id_3,
		CGAL::HalfedgeDS_default,
		std::allocator<int> >  mesh;

	if (!igl::copyleft::cgal::mesh_to_polyhedron(V, F, mesh))
	{
		std::cout << "mesh_to_polyhedron fatal";
		return 0;
	}

	///使用CGAL的细分算法
	Timer t;
	t.start();
	Subdivision_method_3::Sqrt3_subdivision(mesh, params::number_of_iterations(d));
	std::cerr << "Done (" << t.time() << " s)" << std::endl;

	///cgal -> igl
	Eigen::MatrixXd V2;
	Eigen::MatrixXi F2;
	igl::copyleft::cgal::polyhedron_to_mesh(mesh, V2, F2);

	///igl -> off
	igl::writeOFF(out_file, V2, F2);
	return 0;
}