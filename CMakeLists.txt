cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-napi REQUIRED PATHS node_modules/cmake-napi)
find_package(cmake-npm REQUIRED PATHS node_modules/cmake-npm)

project(bitarray_native C)

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
  PRIVATE
    $<TARGET_OBJECTS:bitarray>
    $<TARGET_OBJECTS:intrusive>
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
  PUBLIC
    bitarray
    intrusive
    quickbit
    simdle
)

add_napi_module(bitarray_native_node)

target_sources(
  ${bitarray_native_node}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bitarray_native_node}
  PRIVATE
    $<TARGET_OBJECTS:bitarray>
    $<TARGET_OBJECTS:intrusive>
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
  PUBLIC
    bitarray
    intrusive
    quickbit
    simdle
)

resolve_node_module(bare-compat-napi compat)

target_include_directories(
  ${bitarray_native_node}
  PRIVATE
    "${compat}/include"
)
