cmake_minimum_required(VERSION 3.0.0)

project(parameters_example)
 
add_subdirectory(src)

add_executable(${PROJECT_NAME}  example/parameters_example.c)

target_link_libraries(${PROJECT_NAME}
    parameters
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    "${PROJECT_BINARY_DIR}"
)