cmake_minimum_required(VERSION 3.15)
project(pair_trading_backtest LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)

find_package(fmt REQUIRED)
find_package(fast-cpp-csv-parser REQUIRED)
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
    fmt::fmt
    fast-cpp-csv-parser::fast-cpp-csv-parser
    FastFloat::fast_float
)

target_compile_options(pair_trading_backtest PRIVATE
    -march=native
    -ffp-contract=fast
    -O3
)
