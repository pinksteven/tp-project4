cmake_minimum_required(VERSION 3.15)
project(tp4 LANGUAGES CXX)

file(GLOB source src/*.cpp)
file(GLOB headers src/*.h)

add_executable(tp4 ${source} ${headers})
# Set Windows subsystem, it doesn't work without
set_target_properties(tp4 PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

# Add Gdiplus
target_link_libraries(tp4 Gdiplus)