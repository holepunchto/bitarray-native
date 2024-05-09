cmake_minimum_required(VERSION 3.25)

project(bitarray_native C)

include(bare)

if(NOT TARGET bitarray)
  add_subdirectory(vendor/libbitarray EXCLUDE_FROM_ALL)
endif()

add_bare_module(bitarray_native)

target_sources(
  ${bitarray_native}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bitarray_native}
  PUBLIC
    bitarray_static
)
