cmake_minimum_required (VERSION 3.8)

set(projname "LiverViewer")
project (${projname})

#共享目录
set(common_dir d:/doc/libs/)

#头文件
#set(src_headers  
#	LiverViewer.h
#	RenderStruct.h
#	GeometryGenerator.h
#	renderitem.h
#	)
file(GLOB src_headers "src/*.h")
#源代码
#set(src_files
#	LiverViewer.cpp
#	${common_dir}/glad/src/glad.c
#	)
file(GLOB src_files "src/*.cpp")
set(src_files 
	${src_files}
	${common_dir}/glad/src/glad.c
	)


#项目入口点
add_executable (LiverViewer ${src_headers} ${src_files})




#包含目录
set(include_paths 
	${common_dir}glfw-3.3.2.bin.WIN64/include #glfw
	${common_dir}/glad/include
	${common_dir}/glm/
	)
#link目录
set(link_paths
	${common_dir}glfw-3.3.2.bin.WIN64/lib-vc2019 #glfw
	)
#link文件
set(link_libs
	glfw3 glfw3dll #glfw
	)

target_include_directories(${projname} PRIVATE ${include_paths})
target_link_directories(${projname} PRIVATE ${link_paths})
target_link_libraries(${projname} PRIVATE ${link_libs})