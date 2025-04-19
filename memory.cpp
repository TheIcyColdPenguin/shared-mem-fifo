#include <systemc.h>

#ifndef MEMORY
#define MEMORY

#define ADDR_WIDTH 8               // in bits
#define MEM_SIZE (1 << ADDR_WIDTH) // in words
#define WORD_WIDTH 8               // in bits

SC_MODULE(Memory)
{

    sc_in_clk read_clock;
    sc_in_clk write_clock;
    sc_in<bool> reset;
    sc_in<bool> write_enable;
    sc_in<sc_uint<ADDR_WIDTH>> address;
    sc_in<sc_uint<WORD_WIDTH>> data_in;

    sc_out<sc_uint<WORD_WIDTH>> data_out;

    sc_uint<WORD_WIDTH> data[MEM_SIZE];

    void run()
    {
        while (true)
        {
            wait(reset.posedge_event() | read_clock.posedge_event() | write_clock.posedge_event());

            if (reset.posedge())
            {
                cout << "RESETTING MEMORY\n";
                for (int i = 0; i < MEM_SIZE; i++)
                {
                    data[i] = 0;
                    data_out.write(0);
                }
            }
            else if (write_enable.read() == 0)
            {
                if (read_clock.posedge())
                {
                    cout << "READING FROM MEMORY\n";
                    data_out.write(data[address.read()]);
                }
            }
            else
            {
                if (write_clock.posedge())
                {
                    cout << "WRITING TO MEMORY\n";
                    data[address.read()] = data_in.read();
                }
            }
        }
    }

    SC_CTOR(Memory)
    {
        SC_THREAD(run);
    }
};

#endif