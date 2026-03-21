cmake_minimum_required(VERSION 3.20)
project(reSTEM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(QT_NO_CACHEGEN ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# ── Windows: embed .ico into the .exe via resource file ──────
if(WIN32)
    set(APP_SOURCES main.cpp app.rc)
else()
    set(APP_SOURCES main.cpp)
endif()

qt_add_executable(reSTEM ${APP_SOURCES})

target_link_libraries(reSTEM PRIVATE Qt6::Widgets Qt6::Core)

# ── macOS: bundle .icns inside the .app ──────────────────────
if(APPLE)
    set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/icons/reSTEM.icns")
    set_target_properties(reSTEM PROPERTIES
        MACOSX_BUNDLE                   TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER    "com.restem.app"
        MACOSX_BUNDLE_BUNDLE_NAME       "reSTEM"
        MACOSX_BUNDLE_BUNDLE_VERSION    ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_ICON_FILE         reSTEM
    )
    # Tell CMake to copy the .icns into the .app/Contents/Resources/
    set_source_files_properties(icons/reSTEM.icns
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(reSTEM PRIVATE icons/reSTEM.icns)
endif()

# ── Linux: install icons + .desktop file ─────────────────────
if(UNIX AND NOT APPLE)
    install(TARGETS reSTEM DESTINATION bin)

    # XDG icon sizes
    foreach(SIZE 16 32 48 64 128 256 512)
        install(
            FILES "icons/restem_${SIZE}.png"
            DESTINATION "share/icons/hicolor/${SIZE}x${SIZE}/apps"
            RENAME "restem.png"
        )
    endforeach()

    # .desktop file — this is what puts the app in the app menu
    install(
        FILES "restem.desktop"
        DESTINATION "share/applications"
    )
endif()