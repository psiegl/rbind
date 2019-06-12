# Copyright 2019 - , Dr.-Ing. Patrick Siegl
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.1)
project(rbind)

set(CMAKE_CXX_STANDARD 11)

include_directories(inc)

ENABLE_TESTING()

include(cmake/cppcheck.cmake)

add_subdirectory(tests)
