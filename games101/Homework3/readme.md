## 1. 参数插值

在`rasterize_triangle`中做了以下步骤

1. 遍历三角形内的每个像素，对每个像素进行处理

2. 根据三角形的三个顶点，插值出法向量、颜色、坐标等信息

3. 将状态打包，传入片元着色器，并获得结果

4. 保存着色结果与深度



 ## 2. Blinn-phong反射模型

在`phong_fragment_shader`中做了一下内容

1. 求光的方向、视角方向、衰减半径
2. 求环境光、漫反射、与高光

## 3. Texture mapping

在Blinn-Phong的基础上，将纹理颜色替换公式中的kd，实现了Texture shading fragment shader。



## 4. bump mapping

根据公式，实现了凹凸贴图。



## 5. Displacement mapping

在实现Bump mapping的基础上，实现了displacement mapping。



## 6. 尝试更多模型

找了一头猪，它有法向量，但没有纹理，所以就做了一个normal shader。



## 7. 完成双线纹理插值并调用

根据课程上的公式，实现了双线性插值。

对于边缘的细节，双线性插值方法比较好。