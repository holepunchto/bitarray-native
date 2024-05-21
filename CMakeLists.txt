cmake_minimum_required(VERSION 3.25)

project(bitarray_native C)

include(bare)
include(napi)

if(NOT TARGET bitarray)
  add_subdirectory(vendor/libbitarray EXCLUDE_FROM_ALL)
endif()

add_bare_module(bitarray_native_bare)

target_sources(
  ${bitarray_native_bare}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bitarray_native_bare}
  PUBLIC
    bitarray_static
)

add_napi_module(bitarray_native_node)

bare_include_directories(compat NAPI)

target_include_directories(
  ${bitarray_native_node}
  PRIVATE
    ${compat}
)

target_sources(
  ${bitarray_native_node}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bitarray_native_node}
  PUBLIC
    bitarray_static
)
