#include <iostream>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel; //精确谓词、精确构造的内核
typedef Kernel::Point_3 CGPoint3;

using namespace std;

int main()
{
	double x = 2.21231224, y = 3.321443645, z = 4.12335465;
	CGPoint3 pnt(x, y, z);
	cout << pnt << endl;

	//double x1 = pnt.x();
	double x1 = CGAL::to_double(pnt.x());
	cout << x1 << endl;

	//to_interval

	return 0;
}