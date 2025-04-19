cmake_minimum_required(VERSION 3.10.0)

project(SharedMemoryFIFO)

# Set the SystemC include and library directories
set(SYSTEMC_INCLUDE_DIR $ENV{SYSTEMC_HOME}/include)
set(SYSTEMC_LIBRARY_DIR $ENV{SYSTEMC_HOME}/lib)

# Add the executable
add_executable(learn_systemc memory.cpp memory_tb.cpp fifo_controller.cpp fifo_controller_tb.cpp)
# add_executable(learn_systemc memory.cpp memory_tb.cpp)

# Link against SystemC
target_include_directories(learn_systemc PRIVATE ${SYSTEMC_INCLUDE_DIR})
target_link_libraries(learn_systemc ${SYSTEMC_LIBRARY_DIR}/libsystemc.so)