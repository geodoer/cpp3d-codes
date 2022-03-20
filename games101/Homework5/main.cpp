#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Renderer.h"

// In the main function of the program, we create the scene (create objects and lights)
// as well as set the options for the render (image width and height, maximum recursion
// depth, field-of-view, etc.). We then call the render function().
int main()
{
    //创建场景
    Scene scene(1280, 960); //传入图像的宽度与高度

    //创建一个球
    auto sph1 = std::make_unique<Sphere>(Vector3f(-1, 0, -12), 2);
    sph1->materialType = DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3f(0.6, 0.7, 0.8);

    //创建第二个球
    auto sph2 = std::make_unique<Sphere>(Vector3f(0.5, -0.5, -8), 1.5);
    sph2->ior = 1.5;
    sph2->materialType = REFLECTION_AND_REFRACTION;

    //添加进场景中
    scene.Add(std::move(sph1));
    scene.Add(std::move(sph2));

    //三角网
    Vector3f verts[4] = {{-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16}}; //顶点
    uint32_t vertIndex[6] = {0, 1, 3, 1, 2, 3}; //三角形下标，有两个三角形
    Vector2f st[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
    mesh->materialType = DIFFUSE_AND_GLOSSY;

    //将TriMesh添加到场景中
    scene.Add(std::move(mesh));

    //添加光线
    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 0.5));
    scene.Add(std::make_unique<Light>(Vector3f(30, 50, -12), 0.5));    

    //渲染
    Renderer r;
    r.Render(scene);

    return 0;
}