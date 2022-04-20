#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/subdivision_method_3.h>
#include <CGAL/Timer.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
typedef CGAL::Simple_cartesian<double>          Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3>     PolygonMesh;
using namespace std;
using namespace CGAL;
namespace params = CGAL::parameters;
int main(int argc, char **argv)
{
	const std::string in_file = R"(C:\Users\2107\Desktop\bunny.off)";       //输入Mesh(.off)
	int d = 1;                              //细分深度the depth of the subdivision
	const char* out_file = R"(C:\Users\2107\Desktop\Loop_subdivision_result.off)";   //输出Mesh(.off)

	PolygonMesh pmesh;
	std::ifstream in(in_file);

	if(in.fail())
	{
		std::cerr << "Could not open input file " << in_file << std::endl;
		return 1;
	}

	in >> pmesh;
	Timer t;
	t.start();
	Subdivision_method_3::Loop_subdivision(pmesh, params::number_of_iterations(d));
	std::cerr << "Done (" << t.time() << " s)" << std::endl;
	std::ofstream out(out_file);
	out << pmesh;
	return 0;
}