find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMGenerateHeaders)

# Create a library for utility files
add_library(ghostwriter_utils STATIC)

# Add source files from this directory
target_sources(ghostwriter_utils PRIVATE
    logging.cpp  # Fixed: removed util/ prefix
)

# Add header files to the target
target_sources(ghostwriter_utils PRIVATE
    kerrorcode.h
    kresult.h
    logging.h
)

# Generate convenience headers using KDE's pattern
ecm_generate_headers(ghostwriter_utils_HEADERS
    HEADER_NAMES
        KErrorCode
        KResult
    HEADER_EXTENSION h
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/ghostwriter_utils
    PREFIX ghostwriter_utils
)

# Set include directories
target_include_directories(ghostwriter_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..                 # For "util/header.h" includes (src/ as root)
    ${CMAKE_CURRENT_BINARY_DIR}                    # For generated convenience headers
    ${CMAKE_CURRENT_BINARY_DIR}/ghostwriter_utils  # For the specific subdirectory where ECM puts headers
)

target_link_libraries(ghostwriter_utils PUBLIC
    Qt6::Core
)

# Install the convenience headers
install(FILES ${ghostwriter_utils_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ghostwriter_utils
        COMPONENT Devel)
