cmake_minimum_required(VERSION 3.7.0)

project(radar_simulator VERSION 20200329 DESCRIPTION "For VC16")


set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include)

set(LIB_DIR ${GLFW_LIB_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib)

set(GLFW_LIB glfw3.lib)
set(GLEW_LIBS libglew32d.lib)
set(OPENGL_LIB opengl32.lib)

set(LODEPNG_INC_DIR lib)
set(LODEPNG_LIB_DIR lib/lodepng/x64/Debug)
set(LODEPNG_LIB lodepng.lib)

set(LIBS ${GLFW_LIB} ${GLEW_LIBS} ${OPENGL_LIB} ${LODEPNG_LIB})

set(COMMON_CPP_FILES
	src/common/clock.cpp
	src/common/miscel.cpp
	src/common/gl_util.cpp
	src/common/math.cpp
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
	src/module/image_tracker.cpp
)

set(MODULE_HPP_FILES
	src/module/module.hpp
	src/module/rsim_test.hpp
	src/module/time_sync.hpp
	src/module/glfw_window.hpp
	src/module/image_reader.hpp
	src/module/image_tracker.hpp
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
	src/module_base.cpp
)

set(HPP_FILES
	${COMMON_HPP_FILES}
	${MODULE_HPP_FILES}
	${MEMORY_HPP_FILES}
	src/command.hpp
	src/board.hpp
	src/module_base.hpp
)

include_directories(${INC_DIR})

link_directories(${LIB_DIR})

add_executable(rsim_server src/server.cpp ${CPP_FILES} ${HPP_FILES})
target_link_libraries(rsim_server ${LIBS})


add_executable(rsim src/client.cpp ${CPP_FILES} ${HPP_FILES})
target_link_libraries(rsim ${LIBS})
