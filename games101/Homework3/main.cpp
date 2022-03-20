#include <iostream>
#include <opencv2/opencv.hpp>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.f;
    rotation << cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0,
              0, 2.5, 0, 0,
              0, 0, 2.5, 0,
              0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    return translate * rotation * scale;
}

//将之前实验中的投影矩阵拷贝到这里
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	zNear = -zNear;
    zFar = -zFar;

	//////////////////////////////////////////////////////////////////////////透视投影->正交投影的矩阵
	Eigen::Matrix4f persp_ortho;
	persp_ortho << zNear, 0, 0, 0,
				0, zNear, 0, 0,
				0, 0, zNear + zFar, -zNear * zFar,
				0, 0, 1, 0;

	//////////////////////////////////////////////////////////////////////////正交投影的矩阵
	float fovY_half = eye_fov * MY_PI / 180.0 / 2.0;

	//在相机坐标系中，yTop、xRight有以下关系
	float yTop = zNear * -std::tan(fovY_half);
	float xRight = aspect_ratio * yTop;

	//在相机坐标系中，b和t相反，l和r相反
	float xLeft = -xRight;
	float yButtom = -yTop;

	Eigen::Matrix4f ortho; //正交投影的矩阵
	ortho << 2 / (xRight - xLeft), 0, 0, -(xRight + xLeft) / 2,
		  0, 2 / (yTop - yButtom), 0, (yTop + yButtom) / 2,
		  0, 0, 2 / (zFar - zNear), -(zNear + zFar) / 2,
		  0, 0, 0, 1; //这里可以简化，因为xLeft = -xRight等等

	projection = ortho * persp_ortho;
	return projection;
}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

//光线
struct light
{
    Eigen::Vector3f position;   //光的位置（从代码上来看，这里应该是相机坐标系）
    Eigen::Vector3f intensity;  //光的强度
};

//@function: 应用纹理颜色
//@desc:    在Blinn-Phong的基础上，将纹理颜色视为公式中的kd，实现Texture shading fragment shader
Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    //从纹理中获取该像素的颜色
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        // return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());

        //双线性插值
        return_color = payload.texture->getColorBilinear(payload.tex_coords.x(), payload.tex_coords.y());
    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f; //用纹理颜色代替kd
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        //这部分和phong_fragment_shader中的一样
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        Eigen::Vector3f light_dir = light.position - point;    //光线方向
		Eigen::Vector3f view_dir = eye_pos - point;            //视线方向
		float r2 = light_dir.dot(light_dir);        //衰减因子（即半径的平方）

		// ambient
		Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);

		// diffuse
		Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2);
		Ld *= std::max(0.0f, normal.normalized().dot(light_dir.normalized()));

		// specular
        light_dir.normalize();
        view_dir.normalize();
	    Eigen::Vector3f h = (light_dir + view_dir).normalized();
		Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2);
		Ls *= std::pow(std::max(0.0f, normal.normalized().dot(h)), p);

		result_color += (La + Ld + Ls);
    }

    return result_color * 255.f;
}

//实现Blinn-Phong模型，计算Fragment color
//1. 求出光的方向、视角方向、衰减半径的平方
//2. 求环境光分量
//3. 求漫反射分量
//4. 求高光项
Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    //灯光位置 和 强度
    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2}; //两束光线
    Eigen::Vector3f amb_light_intensity{10, 10, 10};    //环境光的强度（是一个常数）
    Eigen::Vector3f eye_pos{0, 0, 10};  //相机位置（相机坐标系下的坐标）

    float p = 150;  //计算高光时的指数p

    Eigen::Vector3f color = payload.color;      //片元的颜色
    Eigen::Vector3f point = payload.view_pos;   //片元的坐标（相机坐标系下的坐标）
    Eigen::Vector3f normal = payload.normal;    //法向量

    // 视线方向
	Eigen::Vector3f view_dir = eye_pos - point;

    //遍历每一束光，计算 环境光、漫反射、高光
    //并把最终结果加到result_color上
    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        // 光的方向
		Eigen::Vector3f light_dir = light.position - point;
		// 衰减因子（半径的平方）
		float r2 = light_dir.dot(light_dir);

		// 环境光
		Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
		// 漫反射
		Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2);
		Ld *= std::max(0.0f, normal.normalized().dot(light_dir.normalized()));
		// 高光
        light_dir.normalize();
        view_dir.normalize();
		Eigen::Vector3f h = (light_dir + view_dir).normalized();    //半程向量
		Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2);
		Ls *= std::pow(std::max(0.0f, normal.normalized().dot(h)), p);

        result_color += (La + Ld + Ls); //叠加
    }

    return result_color * 255.f;
}

//在实现Bump mapping的基础上，实现displacement mapping
Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    float x = normal.x();
	float y = normal.y();
	float z = normal.z();

    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
	Eigen::Vector3f t{ x*y / std::sqrt(x*x + z * z), std::sqrt(x*x + z * z), z*y / std::sqrt(x*x + z * z) };

    // Vector b = n cross product t
	Eigen::Vector3f b = normal.cross(t);

    // Matrix TBN = [t b n]
    Eigen::Matrix3f TBN;
	TBN << t.x(), b.x(), normal.x(),
		t.y(), b.y(), normal.y(),
		t.z(), b.z(), normal.z();

    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    float u = payload.tex_coords.x();
	float v = payload.tex_coords.y();
	float w = payload.texture->width;
	float h = payload.texture->height;

    float color_u1_v = payload.texture->getColor(u + 1.0f / w , v).norm();
    float color_u_v1 = payload.texture->getColor(u, v + 1.0f / h).norm();
    float color_u_v =  payload.texture->getColor(u, v).norm();

	float dU = kh * kn * (color_u1_v - color_u_v);
	float dV = kh * kn * (color_u_v1 - color_u_v);

    // Vector ln = (-dU, -dV, 1)
	Eigen::Vector3f ln{ -dU,-dV,1.0f };

    // Position p = p + kn * n * h(u,v)
	point += (kn * normal * payload.texture->getColor(u , v).norm());

    // Normal n = normalize(TBN * ln)
	normal = TBN * ln;
	normal = normal.normalized();

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f light_dir = light.position - point;
		Eigen::Vector3f view_dir = eye_pos - point;
		float r2 = light_dir.dot(light_dir);

		// ambient
		Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
		// diffuse
		Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2);
		Ld *= std::max(0.0f, normal.dot(light_dir.normalized()));
		// specular
        light_dir.normalize();
        view_dir.normalize();
		Eigen::Vector3f h = (light_dir + view_dir).normalized();
		Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2);
		Ls *= std::pow(std::max(0.0f, normal.dot(h)), p);

		result_color += (La + Ld + Ls);
    }

    return result_color * 255.f;
}

//在实现Blinn-Phong的基础上，实现Bump mapping（凹凸贴图）
Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    float x = normal.x();
	float y = normal.y();
	float z = normal.z();
    
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
	Eigen::Vector3f t = Eigen::Vector3f(
        x * y / std::sqrt(x * x + z * z), 
        std::sqrt(x * x + z * z), 
        z*y / std::sqrt(x * x + z * z)
    );
    
    // Vector b = n cross product t
	Eigen::Vector3f b = normal.cross(t);

    // Matrix TBN = [t b n]
    Eigen::Matrix3f TBN;
	TBN << t.x(), b.x(), normal.x(),
		t.y(), b.y(), normal.y(),
		t.z(), b.z(), normal.z();

    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    float u = payload.tex_coords.x();
	float v = payload.tex_coords.y();
    float w = payload.texture->width;
	float h = payload.texture->height;

    float color_u1_v = payload.texture->getColor(u + 1.0f / w , v).norm();
    float color_u_v1 = payload.texture->getColor(u, v + 1.0f / h).norm();
    float color_u_v =  payload.texture->getColor(u, v).norm();

	float dU = kh * kn * (color_u1_v - color_u_v);
	float dV = kh * kn * (color_u_v1 - color_u_v);

    // Vector ln = (-dU, -dV, 1)
    Eigen::Vector3f ln = Eigen::Vector3f(-dU, -dV, 1.0f).normalized();

    // Normal n = normalize(TBN * ln)
    Eigen::Vector3f n = TBN * ln;

	Eigen::Vector3f result_color = n.normalized();
    return result_color * 255.f;
}

int main(int argc, const char** argv)
{
    std::vector<Triangle*> TriangleList;

    float angle = 140.0;
    bool command_line = false;

    std::string filename = "output.png"; 
    objl::Loader Loader;

    std::string obj_path = "E:/cpp/cpp3d-codes/data/spot/";
    auto texture_path = "hmap.jpg";
    auto texture2_path = "spot_texture.png"; 
    std::string obj_fp = "E:/cpp/cpp3d-codes/data/spot/spot_triangulated_good.obj";

    //另外一个模型路径
    // std::string obj_path = "E:/cpp/cpp3d-codes/data/spot/";
    // auto texture_path = "hmap.jpg";
    // auto texture2_path = "spot_texture.png"; 
    // std::string obj_fp = "E:/cpp/cpp3d-codes/data/pig.obj";

    // Load .obj File
    bool loadout = Loader.LoadFile(obj_fp);
    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            Triangle* t = new Triangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700);
    r.set_texture(Texture(obj_path + texture_path)); //纹理文件路径

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

    if (argc >= 2)
    {
        command_line = true;
        filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            r.set_texture(Texture(obj_path + texture2_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,10};

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a' )
        {
            angle -= 0.1;
        }
        else if (key == 'd')
        {
            angle += 0.1;
        }

    }
    return 0;
}
