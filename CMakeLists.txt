cmake_minimum_required(VERSION 3.10)
project(GraphicsSuite CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WIN32_EXECUTABLE ON)

# Include both files so the compiler can find WinMain and DrawDDALine
add_executable(graphics_app
    main.cpp
    line/line.cpp
)

# Link Windows GDI system
target_link_libraries(graphics_app gdi32)
