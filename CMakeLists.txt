cmake_minimum_required(VERSION 3.10)
project(tomograf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Znajdź OpenCV na systemie
find_package(OpenCV REQUIRED)

# Znajdź DCMTK na systemie
find_package(DCMTK REQUIRED)

# Znajdź Qt5
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Powiedz CMake gdzie szukać nagłówków OpenCV i DCMTK
include_directories(${OpenCV_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIRS})

# Dodaj plik wykonywalny
add_executable(tomograf 
    src/main.cpp
    src/tomograph_window.cpp
    src/phantom_generator.cpp
    src/radon.cpp
    src/iradon.cpp
    src/ram_lak_filter.cpp
    src/dicom_handler.cpp
    src/patient_info_dialog.cpp
    )

# Zlinkuj z bibliotekami OpenCV, Qt5 i DCMTK
target_link_libraries(tomograf 
    ${OpenCV_LIBS}
    ${DCMTK_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    )
