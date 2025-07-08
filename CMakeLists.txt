cmake_minimum_required(VERSION 3.15)
project(pair_trading_backtest LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

find_package(fmt REQUIRED)
add_executable(pair_trading_backtest src/main.cpp)

target_include_directories(pair_trading_backtest
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GLFW_INCLUDE_DIRS}
		${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(pair_trading_backtest PRIVATE fmt::fmt)

