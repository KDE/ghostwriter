find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Concurrent
    Core
)

find_package(
    KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION}
    REQUIRED
    COMPONENTS
    CoreAddons
)

# Create a library for utility files
add_library(ghostwriter_io STATIC)

# Add source files from this directory
target_sources(ghostwriter_io PRIVATE
    localtextio.cpp
    netsharetextio.cpp
    textio.cpp
    textiofactory.cpp
)

# Add header files to the target
target_sources(ghostwriter_io PRIVATE
    localtextio.h
    netsharetextio.h
    textio.h
    textiofactory.h
)

# Set include directories
target_include_directories(ghostwriter_io PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..                 # For "util/header.h" includes (src/ as root)
    ${CMAKE_CURRENT_BINARY_DIR}                    # For generated convenience headers
)

target_link_libraries(ghostwriter_io PUBLIC
    ghostwriter_utils
    KF${KF_MAJOR_VERSION}::CoreAddons
    Qt6::Core
    Qt6::Concurrent
)

