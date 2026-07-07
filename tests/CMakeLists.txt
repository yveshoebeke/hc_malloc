# Create the test executable
add_executable(test_hc_malloc test_hc_malloc.c)

# Link the test runner to your library
# This automatically handles the include paths for you
target_link_libraries(test_hc_malloc PRIVATE hc_malloc)

# Register the executable as an official test
add_test(NAME MyLibTest COMMAND test_hc_malloc)

