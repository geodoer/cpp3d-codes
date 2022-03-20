



1. `static bool insideTriangle(const Eigen::Vector2f& pnt, const Eigen::Vector2f* tri)` 判断点pnt是否在三角形tri中
2. `void rst::rasterizer::rasterize_triangle(const Triangle& t)`
   1. 执行三角形栅格化算法
   2. 使用MSAA 4x方法抗锯齿 
