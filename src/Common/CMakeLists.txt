add_library(Common STATIC logger.cpp)

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})