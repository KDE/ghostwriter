add_executable(api_test
  cplusplus.cpp
  harness.c
  harness.h
  main.c
)
include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${PROJECT_BINARY_DIR}/extensions
)
if(CMARK_SHARED)
  target_link_libraries(api_test libcmark-gfm-extensions libcmark-gfm)
else()
  target_link_libraries(api_test libcmark-gfm-extensions_static libcmark-gfm_static)
endif()

# Compiler flags
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4706 /D_CRT_SECURE_NO_WARNINGS")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
elseif(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99 -pedantic")
endif()
