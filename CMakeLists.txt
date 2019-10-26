cmake_minimum_required(VERSION 2.8.4)

project(radar_simulator)

set(GLFW_INC_DIR C:/glfw-3.2.1/include)
set(GLFW_LIB_DIR C:/glfw-3.2.1/build/src/Debug)
set(GLFW_LIB glfw3.lib)

set(GLEW_INC_DIR C:/glew-2.0.0/include)
set(GLEW_LIB_DIR C:/glew-2.0.0/build/lib/Debug)
set(GLEW_LIBS glew32d.lib libglew32d.lib)


set(OPENGL_LIB opengl32.lib)

set(EIGEN_INC_DIR C:/Eigen)

set(LODEPNG_INC_DIR lib)
set(LODEPNG_LIB_DIR lib/lodepng/x64/Debug)
set(LODEPNG_LIB lodepng.lib)


set(INC_DIR ${GLFW_INC_DIR} ${GLEW_INC_DIR} ${EIGEN_INC_DIR} ${LODEPNG_INC_DIR})
set(LIB_DIR ${GLFW_LIB_DIR} ${GLEW_LIB_DIR} ${LODEPNG_LIB_DIR})
set(LIBS ${GLFW_LIB} ${GLEW_LIBS} ${OPENGL_LIB} ${LODEPNG_LIB})

set(COMMON_CPP_FILES
	src/common/clock.cpp
	src/common/miscel.cpp
	src/common/gl_util.cpp
)

set(COMMON_HPP_FILES
	src/common/clock.hpp
	src/common/math.hpp
	src/common/miscel.hpp
	src/common/gl_util.hpp
)

set(MODULE_CPP_FILES
	src/module/module.cpp
	src/module/rsim_test.cpp
	src/module/time_sync.cpp
	src/module/glfw_window.cpp
)

set(MODULE_HPP_FILES
	src/module/module.hpp
	src/module/rsim_test.hpp
	src/module/time_sync.hpp
	src/module/glfw_window.hpp
	src/module/image_reader.hpp
)

set(MEMORY_CPP_FILES
	src/memory/memory.cpp
)

set(MEMORY_HPP_FILES
	src/memory/memory.hpp
)

set(CPP_FILES
	${COMMON_CPP_FILES}
	${MODULE_CPP_FILES}
	${MEMORY_CPP_FILES}
	src/command.cpp
	src/board.cpp
)

set(HPP_FILES
	${COMMON_HPP_FILES}
	${MODULE_HPP_FILES}
	${MEMORY_HPP_FILES}
	src/command.hpp
	src/board.hpp
)

include_directories(${INC_DIR})

link_directories(${LIB_DIR})



add_executable(rsim_server src/server.cpp ${CPP_FILES} ${HPP_FILES})
target_link_libraries(rsim_server ${LIBS})


add_executable(rsim src/client.cpp ${CPP_FILES} ${HPP_FILES})
target_link_libraries(rsim ${LIBS})
