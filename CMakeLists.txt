cmake_minimum_required(VERSION 3.7.0)

project(radar_simulator VERSION 20200329)


set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib)

set(GLFW_DEBUG_LIB debug glfw3.lib)
set(GLFW_RELEASE_LIB optimized glfw3.lib)

set(GLEW_DEBUG_LIBS debug libglew32d.lib debug glew32d.lib)
set(GLEW_RELEASE_LIBS optimized libglew32.lib optimized glew32.lib)

set(OPENGL_DEBUG_LIB debug opengl32.lib)
set(OPENGL_RELEAE_LIB optimized opengl32.lib)

set(LODEPNG_DEBUG_LIB debug lodepng.lib)
set(LODEPNG_RELEASE_LIB optimized lodepng.lib)

set(DEBUG_LIBS  ${GLFW_DEBUG_LIB} ${GLEW_DEBUG_LIBS} ${LODEPNG_DEBUG_LIB} ${OPENGL_DEBUG_LIB})
set(RELEASE_LIBS  ${GLFW_RELEASE_LIB} ${GLEW_RELEASE_LIBS} ${LODEPNG_RELEASE_LIB} ${OPENGL_RELEASE_LIB})

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
target_link_libraries(rsim_server ${DEBUG_LIBS} ${RELEASE_LIBS})


add_executable(rsim src/client.cpp ${CPP_FILES} ${HPP_FILES})
target_link_libraries(rsim ${DEBUG_LIBS} ${RELEASE_LIBS})
