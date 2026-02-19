cmake_minimum_required(VERSION 3.16)
project(cram_solver LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -fomit-frame-pointer -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Og -march=native -fomit-frame-pointer -fdiagnostics-color=always -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/external/eigen
)

# Collect all .cpp files recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

# Add glad.c manually
#set(SRC_FILES ${SRC_FILES} ${CMAKE_SOURCE_DIR}/src/renderer/glad.c)

# Create the executable
add_executable(main ${SRC_FILES})

# Link libraries (same as -lglfw3 -lopengl32 -lm)
#target_link_libraries(main PRIVATE glfw3 opengl32 m)



###### Catch2 ######

find_package(Catch2 3 REQUIRED)

# Collect all .cpp files recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)
list(FILTER SRC_FILES EXCLUDE REGEX "src/main.cpp")

# These tests can use the Catch2-provided main
add_executable(tests ../tests/test.cpp ${SRC_FILES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# These tests need their own main
#add_executable(custom-main-tests ../tests/test.cpp ../tests/test-main.cpp)
#target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)