cmake_minimum_required(VERSION 3.5.0)
# We need C++ 20 for designated initialisers, 
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cnpproc VERSION 0.1.0 LANGUAGES C CXX)

add_library(procon procon.cpp)
target_compile_features(procon PUBLIC cxx_std_20)

# Use libcoro
include(FetchContent)
FetchContent_Declare(
    libcoro
    GIT_REPOSITORY https://github.com/jbaldwin/libcoro.git
    GIT_TAG v0.12.1
)
FetchContent_MakeAvailable(libcoro)
target_link_libraries(procon PUBLIC libcoro)
target_include_directories(procon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# include(CTest)
# enable_testing()
