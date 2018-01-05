cmake_minimum_required(VERSION 2.8)

project(particles)

set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules/")

if (WIN32)
	option(GLFW_USE_STATIC_LIBS "Use static glfw lib (Windows only)" ON)
else()
	set(GLFW_USE_STATIC_LIBS 0)
endif()

if (MSVC)
	option(USE_MSVC_RT_LIB_STATIC "Use static msvc runtime lib" ON)
endif()

set(SOURCES
	particles/main.cpp
	particles/Camera.cpp
	particles/CameraController.cpp
	particles/tga/tga.c
	particles/utility/Clock.cpp
	particles/maths/Quaternion.cpp
	particles/gl/OpenGlLoader.cpp
	particles/gl/flextGLInit.cpp
	particles/gl/flextGL.cpp
)
	
if (USE_MSVC_RT_LIB_STATIC)
    foreach (flag CMAKE_C_FLAGS
						CMAKE_C_FLAGS_DEBUG
						CMAKE_C_FLAGS_RELEASE
						CMAKE_C_FLAGS_MINSIZEREL
						CMAKE_C_FLAGS_RELWITHDEBINFO
	)
        if (${flag} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
        endif()
        if (${flag} MATCHES "/MDd")
            string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
        endif()
    endforeach()
endif()

add_executable(particle-system ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)

set(LINK_LIBS ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARIES})
if (UNIX)
	list(APPEND LINK_LIBS "${CMAKE_DL_LIBS}")
endif()

target_link_libraries(particle-system "${LINK_LIBS}")
target_include_directories(particle-system PUBLIC ${GLFW_INCLUDE_DIRS})

if (WIN32)
	add_definitions(-DFHL_PLATFORM_WINDOWS)
	if (MSVC)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	endif()
	set_target_properties(particle-system PROPERTIES 
		LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS"
		LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS"
	)
elseif (APPLE)
	add_definitions(-DFHL_PLATFORM_APPLE)
endif()