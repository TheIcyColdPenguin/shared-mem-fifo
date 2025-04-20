#include <systemc.h>
#include "memory.cpp"

#ifndef FIFO_CONTROLLER
#define FIFO_CONTROLLER

#define NUM_FIFOS 8

// TODO: busy meter
// TODO: ensure no more than one writes at a time

SC_MODULE(FifoController)
{

    sc_in<sc_bv<NUM_FIFOS>> write_clock;
    sc_in<sc_bv<NUM_FIFOS>> read_clock;
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

    void enque_deque()
    {

        while (true)
        {

            // cout << sc_time_stamp() << " | waiting for next event\n";
            wait(write_clock.value_changed_event() | read_clock.value_changed_event());

            // cout << sc_time_stamp() << " | write clock = " << write_clock.read() << endl;
            for (int i = 0; i < NUM_FIFOS; i++)
            {
                if (write_clock.read()[i] == 1)
                {
                    cout << sc_time_stamp() << " | TRIGGERED ENQUE with fifo " << i << endl;
                    // to enque,
                    // write data to where tail is located

                    // allocate memory
                    //      deque new address from free_fifo
                    //      enque new address into data_fifo

                    // increment tail

                    mem_address = data_fifos[i][data_tail[i] - 1];
                    mem_data_in = data_in[i];

                    mem_write_enable = 1;
                    mem_write_clock = 1;

                    wait(SC_ZERO_TIME);

                    mem_write_clock = 0;
                    mem_write_enable = 0;

                    sc_uint<ADDR_WIDTH> next_address = free_fifo[free_head++];
                    data_fifos[i][data_tail[i]++] = next_address;
                    break;
                }
            }

            // cout << sc_time_stamp() << " | read clock = " << read_clock.read() << endl;

            for (int i = 0; i < NUM_FIFOS; i++)
            {
                if (read_clock.read()[i] == 1)
                {

                    cout << sc_time_stamp() << " | TRIGGERED DEQUE with fifo " << i << endl;

                    // to deque,
                    // read data from head

                    // deallocate memory
                    //      deque the read address from data_fifo
                    //      enque the read address into free_fifo

                    // increment head

                    mem_address = data_fifos[i][data_head[i]];
                    mem_write_enable = 0;

                    mem_read_clock = 1;

                    wait(SC_ZERO_TIME);
                    wait(SC_ZERO_TIME);

                    cout << "value read: " << mem_data_out.read() << endl;
                    data_out[i].write(mem_data_out.read());
                    mem_read_clock = 0;

                    sc_uint<ADDR_WIDTH> freed_address = data_fifos[i][data_head[i]++];
                    free_fifo[free_tail++] = freed_address;

                    break;
                }
            }
        }
    }

    void reset_mem()
    {
        mem_reset = reset;
        if (reset.read())
        {
            for (int i = 0; i < NUM_FIFOS; i++)
            {
                data_head[i] = 0;
                data_tail[i] = 1;

                data_fifos[i][0] = i;

                for (int j = 1; j < MEM_SIZE; j++)
                {
                    data_fifos[i][j] = 0;
                }
            }

            free_head = 0;
            free_tail = MEM_SIZE - NUM_FIFOS;

            for (int i = 0; i < MEM_SIZE - NUM_FIFOS; i++)
            {
                free_fifo[i] = i + NUM_FIFOS;
            }
        }
    }

    void monitor_allocator()
    {
        if ((read_clock.read() == 0) && (write_clock.read() == 0))
        {
            return;
        }

        for (int i = 0; i < NUM_FIFOS; i++)
        {
            cout << "FIFO_" << i << " @ ";
            if (data_head[i] <= data_tail[i])
            {
                for (int j = data_head[i]; j < data_tail[i]; j++)
                {
                    cout << data_fifos[i][j] << ",";
                }
            }
            else
            {
                for (int j = data_head[i]; j < MEM_SIZE; j++)
                {
                    cout << data_fifos[i][j] << ",";
                }
                for (int j = 0; j < data_tail[i]; j++)
                {

                    cout << data_fifos[i][j] << ",";
                }
            }
            cout << endl;
        }

        cout << "FREE_FIFO @ ";
        if (free_head <= free_tail)
        {
            for (int j = free_head; j < free_tail; j++)
            {
                cout << free_fifo[j] << ",";
            }
        }
        else
        {
            for (int j = free_head; j < MEM_SIZE; j++)
            {
                cout << free_fifo[j] << ",";
            }
            for (int j = 0; j < free_tail; j++)
            {

                cout << free_fifo[j] << ",";
            }
        }
        cout << endl;
    }

    SC_CTOR(FifoController)
        : mem("MEMORY")
    {
        mem.reset(mem_reset);
        mem.address(mem_address);
        mem.write_enable(mem_write_enable);
        mem.data_in(mem_data_in);
        mem.data_out(mem_data_out);
        mem.write_clock(mem_write_clock);
        mem.read_clock(mem_read_clock);

        SC_THREAD(enque_deque);
        sensitive << write_clock << read_clock;
        // dont_initialize();

        SC_METHOD(reset_mem);
        sensitive << reset;

        // SC_METHOD(monitor_allocator);
        // sensitive << write_clock << read_clock;
    }
};

#endif