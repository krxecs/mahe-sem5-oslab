add_library(osl-vec STATIC vec.c)
target_include_directories(osl-vec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(osl-helper STATIC helper.c)
target_include_directories(osl-helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
