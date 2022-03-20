

1. 完成了De Casteljau 算法，结果：my_bezier_curve.png
   1. 打开了naive_bezier方法，结果：my_bezier_curve_with-naive-bezier.png
2. 完成了反走样，结果：my_bezier_curve_antialiasing.png



函数recursive_bezier

1. 通过递归的方法实现了de Casteljau算法，返回Bézier 曲线上对应点的坐标



函数bezier，两个版本

1. 一个普通版本，从0到1，位移量为0.001计算Bézier曲线上对应点
2. 一个反走样版本，考虑采样点周边的四个点，根据距离权重设置颜色

