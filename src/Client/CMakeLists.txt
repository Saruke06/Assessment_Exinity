add_executable(Client main.cpp client.cpp)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Правильно указываем зависимости
target_link_libraries(Client PRIVATE Common Boost::system Ws2_32)