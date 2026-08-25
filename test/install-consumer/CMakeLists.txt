# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)
project(morpheus_consumer LANGUAGES C)

find_package(Morpheus 0.1 CONFIG REQUIRED)

add_executable(morpheus_consumer main.c)
target_link_libraries(morpheus_consumer PRIVATE Morpheus::morpheus)
