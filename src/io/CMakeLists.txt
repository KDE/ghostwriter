find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# Create a library for utility files
add_library(ghostwriter_io STATIC)

# Add source files from this directory
target_sources(ghostwriter_io PRIVATE
    webdavwinio.cpp
)

# Add header files to the target
target_sources(ghostwriter_io PRIVATE
    textio.h
    webdavwinio.h
)

# Set include directories
target_include_directories(ghostwriter_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..                # For "util/header.h" includes (src/ as root)
    ${CMAKE_CURRENT_BINARY_DIR}                   # For generated convenience headers
    ${CMAKE_CURRENT_BINARY_DIR}/ghostwriterUtils  # For the specific subdirectory where ECM puts headers
)

target_link_libraries(ghostwriter_utils PUBLIC
    Qt6::Core
)

