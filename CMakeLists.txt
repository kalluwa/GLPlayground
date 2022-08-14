cmake_minimum_required (VERSION 3.8)

set(projname "LiverViewer")
project (${projname})

#共享目录
set(common_dir d:/doc/libs)
set(third_lib libs)

#cpp标准
set(CMAKE_CXX_STANDARD 20) 
#set(CMAKE_CXX_STANDARD_REQUIRED ON) 
#set(CMAKE_CXX_EXTENSIONS OFF) 

#头文件
file(GLOB src_headers "src/*.h")

#源代码
file(GLOB src_files "src/*.cpp")
file(GLOB soil2 libs/soil2/srcs/*.c)
file(GLOB fmt_src ${common_dir}/fmt-7.1.3/src/*)


set(src_files 
	${src_files}
	${common_dir}/glad/src/glad.c
	${soil2}
	${fmt_src}
	)


#项目入口点
add_executable (LiverViewer ${src_headers} ${src_files})


#包含目录
set(include_paths 
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${common_dir}/glfw-3.3.2.bin.WIN64/include #glfw
	${common_dir}/glad/include
	${common_dir}/glm/
	${common_dir}/fmt-7.1.3/include
	${third_lib}/soil2/incs
	)
#link目录
set(link_paths
	${common_dir}/glfw-3.3.2.bin.WIN64/lib-vc2019 #glfw
	)
#link文件
set(link_libs
	glfw3 glfw3dll #glfw
	opengl32 #opengl
	)

target_include_directories(${projname} PRIVATE ${include_paths})
target_link_directories(${projname} PRIVATE ${link_paths})
target_link_libraries(${projname} PRIVATE ${link_libs})