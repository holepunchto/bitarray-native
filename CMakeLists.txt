cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)
find_package(cmake-napi REQUIRED PATHS node_modules/cmake-napi)
find_package(cmake-npm REQUIRED PATHS node_modules/cmake-npm)

project(bitarray_native C)

fetch_package("github:holepunchto/libbitarray#cc4b3eb")
fetch_package("github:holepunchto/libquickbit#9e92d1c")

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
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
  PUBLIC
    bitarray
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
    $<TARGET_OBJECTS:quickbit>
    $<TARGET_OBJECTS:simdle>
  PUBLIC
    bitarray
    quickbit
    simdle
)

resolve_node_module(bare-compat-napi compat)

target_include_directories(
  ${bitarray_native_node}
  PRIVATE
    "${compat}/include"
)
