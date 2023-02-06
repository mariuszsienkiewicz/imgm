cmake_minimum_required(VERSION 3.14)
project(img-processing)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    src/Tools.cpp
    src/PgmImage.cpp
    src/BmpImage.cpp
)
set(LIBRARY_NAME engine)

# Compile all sources 
add_library(${LIBRARY_NAME} OBJECT ${SOURCES})

# Headers
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Executable 
add_executable(imgm app/main.cpp)
target_link_libraries(imgm PRIVATE ${LIBRARY_NAME})
