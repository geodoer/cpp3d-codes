此工程模板来自：https://github.com/libigl/libigl-example-project

## Quick Start
如果你本地没有下载libigl，直接cmake构建

如果你本地下载了libigl
1. 打开`build.bat`，修改`FETCHCONTENT_SOURCE_DIR_LIBIGL`的值
2. 运行build.bat即可


## 问题
### 运行exe报错：找不到libgmp-10.dll
1. 在`./build/_deps`目录下搜索`libgmp-10.dll`
2. 拷贝到exe所在目录