#include <systemc.h>
#include "memory.cpp"

#ifndef FIFO_CONTROLLER
#define FIFO_CONTROLLER

#define NUM_FIFOS 2

// TODO: busy meter
// TODO: ensure no more than one writes at a time

SC_MODULE(FifoController)
{

    sc_in_clk write_clock[NUM_FIFOS];
    sc_in_clk read_clock[NUM_FIFOS];
    sc_in<bool> reset;

    sc_in<sc_uint<WORD_WIDTH>> data_in[NUM_FIFOS];
    sc_out<sc_uint<WORD_WIDTH>> data_out[NUM_FIFOS];

    sc_uint<ADDR_WIDTH> data_fifos[NUM_FIFOS][MEM_SIZE];
    sc_uint<ADDR_WIDTH> free_fifo[MEM_SIZE];

    sc_uint<ADDR_WIDTH> data_head[NUM_FIFOS];
    sc_uint<ADDR_WIDTH> data_tail[NUM_FIFOS];

    sc_uint<ADDR_WIDTH> free_head;
    sc_uint<ADDR_WIDTH> free_tail;

    sc_signal<bool> mem_write_clock;
    sc_signal<bool> mem_read_clock;
    sc_signal<sc_uint<WORD_WIDTH>> mem_data_in;
    sc_signal<sc_uint<WORD_WIDTH>> mem_data_out;
    sc_signal<sc_uint<ADDR_WIDTH>> mem_address;
    sc_signal<bool> mem_write_enable;
    sc_signal<bool> mem_reset;

    Memory mem;

    void enque()
    {
        for (int i = 0; i < NUM_FIFOS; i++)
        {
            if (write_clock[i].event())
            {
                // to enque,
                // write data to where tail is located
                // deque new address from free_fifo
                // enque new address into data_fifo
                mem_address = data_fifos[i][data_tail[i]];
                mem_write_clock = write_clock[i];
                mem_data_in = data_in[i];
                mem_write_enable = 1;

                wait(mem_write_clock.posedge_event());

                mem_write_enable = 0;
            }
        }
    }

    void deque()
    {
        for (int i = 0; i < NUM_FIFOS; i++)
        {
            if (read_clock[i].event())
            {
            }
        }
    }

    void run()
    {
        mem_reset = reset;

        for (int i = 0; i < NUM_FIFOS; i++)
        {
            data_head[i] = 0;
            data_tail[i] = 0;

            data_fifos[i][0] = i;

            for (int j = 1; j < MEM_SIZE; j++)
            {
                data_fifos[i][j] = 0;
            }
        }

        free_head = 0;
        free_tail = NUM_FIFOS;

        for (int i = 0; i < MEM_SIZE - NUM_FIFOS; i++)
        {
            free_fifo[i] = i + NUM_FIFOS;
        }
    }

    SC_CTOR(FifoController) : mem("MEMORY")
    {

        mem.reset(mem_reset);
        mem.address(mem_address);
        mem.write_enable(mem_write_enable);
        mem.data_in(mem_data_in);
        mem.data_out(mem_data_out);
        mem.write_clock(mem_write_clock);
        mem.read_clock(mem_read_clock);

        SC_METHOD(enque);
        sensitive << write_clock;

        SC_METHOD(deque);
        sensitive << read_clock;

        SC_METHOD(run);
        sensitive << reset;
    }
};

#endif