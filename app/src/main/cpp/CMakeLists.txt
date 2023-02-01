cmake_minimum_required(VERSION 3.22.1)

project("jniannotationtest")

add_library(jniannotationtest
            SHARED
            jniannotationtest.cpp)

target_link_libraries(jniannotationtest log)
