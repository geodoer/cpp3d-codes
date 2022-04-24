#include <igl/opengl/glfw/Viewer.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <map>

int main(int argc, char * argv[])
{
	igl::opengl::glfw::Viewer viewer;

	//多个名称
	const auto names =
	{"cube.obj","sphere.obj","xcylinder.obj","ycylinder.obj","zcylinder.obj"};

	std::map<int, Eigen::RowVector3d> colors; //颜色表。MeshID，颜色
	int last_selected = -1;

	for(const auto & name : names)
	{
		viewer.load_mesh_from_file(std::string(TUTORIAL_SHARED_PATH) + "/" + name); //加载一个Mesh到viewer中
		auto id = viewer.data().id; //获得当前Mesh的ID
		colors.emplace(id, 0.5*Eigen::RowVector3d::Random().array() + 0.5); //随机一个颜色
	}

	//键盘回调
	viewer.callback_key_down =
		[&](igl::opengl::glfw::Viewer &, unsigned int key, int mod)
	{
		if(key == GLFW_KEY_BACKSPACE) //退格键
		{
			int old_id = viewer.data().id;				//当前Mesh的ID
			size_t index = viewer.selected_data_index;	//当前Mesh在data_list中的下标

			if(viewer.erase_mesh(index))
			{
				colors.erase(old_id);
				last_selected = -1;
			}

			return true;
		}

		return false;
	};

	//刷新网格颜色
	// Refresh selected mesh colors
	viewer.callback_pre_draw =
		[&](igl::opengl::glfw::Viewer &)
	{
		if(last_selected != viewer.selected_data_index)
		{
			for(auto &data : viewer.data_list) //viewer的数据列表
			{
				data.set_colors(colors[data.id]); //设置颜色
			}

			viewer.data_list[viewer.selected_data_index].set_colors(Eigen::RowVector3d(0.9,0.1,0.1));
			last_selected = viewer.selected_data_index;
		}

		return false;
	};

	viewer.launch();
	return EXIT_SUCCESS;
}
