cmake_minimum_required(VERSION 3.20)
project(reSTEM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# ── Find Qt6 ─────────────────────────────────────────────────────────
#  Linux Fedora : sudo dnf install qt6-qtbase-devel cmake gcc-c++
#  macOS Homebrew: cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

qt_add_executable(reSTEM main.cpp)

target_link_libraries(reSTEM PRIVATE Qt6::Widgets Qt6::Core)

if(APPLE)
    set_target_properties(reSTEM PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.reSTEM.app"
        MACOSX_BUNDLE_BUNDLE_NAME "reSTEM"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
endif()

if(UNIX AND NOT APPLE)
    install(TARGETS reSTEM DESTINATION bin)
endif()
