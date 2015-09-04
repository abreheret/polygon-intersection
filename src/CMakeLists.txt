project(PolygonIntersec)
cmake_minimum_required(VERSION 2.8)

if(WIN32) # if windows, insert your opencv path.
	set(OpenCV_DIR "C:/OpenCV/OpenCV2.4.10/opencv/build")
endif(WIN32)

find_package(OpenCV REQUIRED)

add_executable(polygon_intersec main.cpp utils.cpp)
target_link_libraries(polygon_intersec ${OpenCV_LIBS})
