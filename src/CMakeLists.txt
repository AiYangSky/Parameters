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


