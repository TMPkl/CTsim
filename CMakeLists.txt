cmake_minimum_required(VERSION 3.10)
project(tomograf)

set(CMAKE_CXX_STANDARD 17)

# Znajdź OpenCV na systemie
find_package(OpenCV REQUIRED)

# Powiedz CMake gdzie szukać nagłówków OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Dodaj plik wykonywalny z pliku main.cpp
add_executable(tomograf 
    src/main.cpp
    src/phantom_generator.cpp
    src/radon.cpp
    src/iradon.cpp
    )


# Zlinkuj z bibliotekami OpenCV
target_link_libraries(tomograf ${OpenCV_LIBS})
