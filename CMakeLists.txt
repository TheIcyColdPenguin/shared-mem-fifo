cmake_minimum_required(VERSION 3.10.0)

project(SharedMemoryFIFO)

# Set the SystemC include and library directories
set(SYSTEMC_INCLUDE_DIR $ENV{SYSTEMC_HOME}/include)
set(SYSTEMC_LIBRARY_DIR $ENV{SYSTEMC_HOME}/lib)

# Add the executable
add_executable(shared_memory_fifo memory.cpp memory_tb.cpp fifo_controller.cpp fifo_controller_tb.cpp)
# add_executable(shared_memory_fifo memory.cpp memory_tb.cpp)

# Link against SystemC
target_include_directories(shared_memory_fifo PRIVATE ${SYSTEMC_INCLUDE_DIR})
target_link_libraries(shared_memory_fifo ${SYSTEMC_LIBRARY_DIR}/libsystemc.so)