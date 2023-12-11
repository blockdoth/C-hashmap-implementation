cmake_minimum_required(VERSION 3.22)
project(C_HashMap C)

set(CMAKE_C_STANDARD 99)

# Set flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-value -Wno-unused-result -Wno-unused-label")

# Add your source files
set(SOURCE_FILES test.c gest.c solution.c)

# Add the executable
add_executable(C_HashMap ${SOURCE_FILES})

# Set flags to disable the warning about deprecated function declarations without prototypes
target_compile_options(C_HashMap PRIVATE "-Wno-deprecated-declarations")

# Link libraries
target_link_libraries(C_HashMap pthread rt)

# Define features
target_compile_definitions(C_HashMap PRIVATE SEQUENTIAL)

# Optionally, you can add a custom target for valgrind
add_custom_target(valgrind COMMAND valgrind --tool=memcheck  --leak-check=yes  ./C_HashMap)

# Optionally, you can add a clean target
# add_custom_target(clean COMMAND rm -f *.o C_HashMap)
