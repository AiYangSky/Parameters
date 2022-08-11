 project(parameters_example)
 
 add_library(parameters)

file(GLOB LIB_FILE ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

target_sources(parameters
    PRIVATE
    ${LIB_FILE}
)

target_include_directories(parameters
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PROJECT_NAME}  example/parameters_example.c)

target_link_libraries(${PROJECT_NAME}
    parameters
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    "${PROJECT_BINARY_DIR}"
)
