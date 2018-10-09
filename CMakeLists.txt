project(webserver)

cmake_minimum_required(VERSION 2.8)

aux_source_directory(. DIR_SRCS)

add_executable(webserver ${DIR_SRCS})

set(SOURCE_FILES server.c)
