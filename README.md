# cpp3d-codes
C++图形学、几何方面的代码，考虑示例的简单性，主要以面向过程为主。

包括
1. 简单定义了自己的三维几何数据结构，便于与其他第三方库做IO，为你的移植工作提供示例

## 编译

### 使用Conan

Windows

1. 运行以下命令，使用conan安装依赖包

```
conanbuild.bat "Visual Studio 15 Win64"
```

2. 用Visual Studio 2017打开`build/*.sln`，并选`RelWithDebInfo`

## 工程说明

1. `.\geodoer\` 包含自定义的数据结构与IO等，内部全为hpp，避免链接等麻烦

