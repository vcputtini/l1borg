cmake_minimum_required(VERSION 3.16)

project(l1borg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# To boost log
# add_compile_options(-Wall -O2 -DBOOST_LOG_DYN_LINK)
add_compile_options(-Wall -O2)

find_package(tinyxml2 REQUIRED)
find_package(Boost REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_executable(l1borg command.cc envconfig.cc l1borg.cc)

# To compile Boost: log
# Define in the program #define BOOST_LOG_DYN_LINK
# Use the libraries below to link
#  -lboost_log -lboost_log_setup -lboost_system -lboost_thread -lpthread -DBOOST_LOG_DYN_LINK
# target_link_libraries(l1borg PRIVATE tinyxml2 -lboost_log -lboost_log_setup -lboost_system -lboost_thread -lpthread)

target_link_libraries(l1borg PRIVATE tinyxml2 -lboost_system -lboost_thread -lpthread)
