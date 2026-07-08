cmake_minimum_required(VERSION 3.15)
project(hc_malloc VERSION 1.0.0 LANGUAGES C)

# Set the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define the static library from your single source file
add_library(hc_malloc STATIC src/hc_malloc.c)

# Specify the public include directories
target_include_directories(hc_malloc PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Optional: Setup 'make install' commands for the client
install(TARGETS hc_malloc DESTINATION lib COMPONENT SystemFiles)
install(DIRECTORY include/hc_malloc DESTINATION include COMPONENT SystemFiles)

# Enable testing tools
enable_testing()

# Include the tests subdirectory
add_subdirectory(tests)

# Install the mylib.a binary to /usr/local/lib
install(TARGETS hc_malloc DESTINATION lib)

include(GNUInstallDirs)
# Install the public header directory to /usr/local/include/mylib/
install(DIRECTORY include/hc_malloc DESTINATION include COMPONENT SystemFiles)

# Auxiliaries locations
set(DOC_DEST /usr/local/share/hc_malloc/doc)
set(HTML_DOC_DEST ${DOC_DEST}/html)
set(PDF_DOC_DEST ${DOC_DEST}/pdf)
set(LICENSE_DEST ${DOC_DEST}/license)
set(MAN_DEST /usr/local/share/man/man3)
set(CUSTOMER_HOME "$ENV{HOME}")
set(TEST_DEST "${CUSTOMER_HOME}/.local/share/hc_malloc/tests")

# Install auxiliaries - documentation.
install(FILES doc/README.md DESTINATION ${DOC_DEST} COMPONENT SystemFiles)
install(FILES doc/html/hc_malloc.html DESTINATION ${HTML_DOC_DEST} COMPONENT SystemFiles)
install(FILES doc/pdf/hc_malloc.pdf DESTINATION ${PDF_DOC_DEST} COMPONENT SystemFiles)
# Install auxiliaries - license.
install(FILES LICENSE DESTINATION ${LICENSE_DEST} COMPONENT SystemFiles)
# Install auxiliaries - man page.
install(DIRECTORY doc/man/ DESTINATION ${MAN_DEST} COMPONENT SystemFiles)
# Install auxiliaries - test facility.
install(PROGRAMS "${CMAKE_BINARY_DIR}/tests/test_hc_malloc" 
	DESTINATION ${TEST_DEST} 
	COMPONENT UserFiles)

