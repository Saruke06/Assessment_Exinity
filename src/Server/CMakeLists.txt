add_executable(Server main.cpp server.cpp numberstore.cpp dumper.cpp)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Аналогично исправляем для сервера
target_link_libraries(Server PRIVATE Common Boost::system Ws2_32)