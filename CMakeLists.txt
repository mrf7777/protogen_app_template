cmake_minimum_required(VERSION 3.16)
project(protogen_app_test)

set(PROTOGEN_APP_ID "protogenapptest")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} MODULE ProtogenAppTest.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/cpp-httplib")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
configure_file("cmake_vars.h.in" "cmake_vars.h")

install(TARGETS ${PROJECT_NAME} DESTINATION "share/protogen/apps/${PROTOGEN_APP_ID}")
install(DIRECTORY resources DESTINATION "share/protogen/apps/${PROTOGEN_APP_ID}")