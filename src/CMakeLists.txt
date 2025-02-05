cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Поиск библиотеки Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.87 REQUIRED COMPONENTS system)

# Добавляем подпроекты
add_subdirectory(Common)
add_subdirectory(Client)
add_subdirectory(Server)