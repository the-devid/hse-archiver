project(hse-archiver)

cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)


set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(
        archiver
        src/archiver_structs.cpp
        src/archiver.cpp
        src/binary_trie.cpp
        src/bit_reader.cpp
        src/bit_writer.cpp
        src/decoder.cpp
        src/encoder.cpp
        src/huffman_encoder.cpp
        )

add_executable(test_archiver_units
        lib/catch_main.cpp
        src/archiver_structs.cpp
        src/binary_trie.cpp
        src/bit_reader.cpp
        src/bit_writer.cpp
        src/encoder.cpp
        src/huffman_encoder.cpp
        src/unit_tests.cpp
        )
target_link_libraries(test_archiver_units Catch)
