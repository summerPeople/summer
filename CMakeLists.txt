# 设置工程名
PROJECT(summer)

#设置要求的最低cmake版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#设置make的编译器
SET(CMAKE_CXX_COMPILER "/usr/bin/gcc")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
# ADD_SUBDIRECTORY(sub_source_dir [cmake输出文件目录])
#设置包含要编译源码的子目录和中间文件输出路径
ADD_SUBDIRECTORY(application ${PROJECT_SOURCE_DIR}/application/build)
ADD_SUBDIRECTORY(commodule ${PROJECT_SOURCE_DIR}/commodule/build)
ADD_SUBDIRECTORY(mempool ${PROJECT_SOURCE_DIR}/mempool/build)
ADD_SUBDIRECTORY(pager ${PROJECT_SOURCE_DIR}/pager/build)
#ADD_SUBDIRECTORY(sysconf ${PROJECT_SOURCE_DIR}/sysconf/build)
ADD_SUBDIRECTORY(test ${PROJECT_SOURCE_DIR}/test/build)
#ADD_SUBDIRECTORY(compiler ${PROJECT_SOURCE_DIR}/compiler/build)
ADD_SUBDIRECTORY(executer ${PROJECT_SOURCE_DIR}/executer/build)
ADD_SUBDIRECTORY(btree ${PROJECT_SOURCE_DIR}/btree/build)

