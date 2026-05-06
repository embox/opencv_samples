# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Define project name
project(haar_cnn_demo_project)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Print OpenCV info
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")

# Add all source files
add_executable(haar_cnn_demo 
    haar_cnn_demo.cpp
    cnn_inference.cpp
)

# Include the directory with weight headers
target_include_directories(haar_cnn_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link with OpenCV
target_link_libraries(haar_cnn_demo PRIVATE ${OpenCV_LIBS})

# Optional: Set C++ standard
#set_target_properties(haar_cnn_demo PROPERTIES
#    CXX_STANDARD 11
#    CXX_STANDARD_REQUIRED ON
#)