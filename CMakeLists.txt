cmake_minimum_required(VERSION 3.15)
project(pair_trading_backtest LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

find_package(Eigen3 REQUIRED)
find_package(FastFloat REQUIRED)

file(GLOB_RECURSE PAIR_TRADING_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(pair_trading_backtest ${PAIR_TRADING_SRC})

target_include_directories(pair_trading_backtest
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GLFW_INCLUDE_DIRS}
		${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(pair_trading_backtest PRIVATE
    Eigen3::Eigen
    FastFloat::fast_float
)

target_compile_options(pair_trading_backtest PRIVATE
    -march=native
    -ffp-contract=fast
    -O3
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)
