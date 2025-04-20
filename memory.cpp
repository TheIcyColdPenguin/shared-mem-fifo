#include <systemc.h>

#ifndef MEMORY
#define MEMORY

#define ADDR_WIDTH 10              // in bits
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
        if (reset.read() == 1)
        {
            cout << sc_time_stamp() << "  | RESETTING MEMORY\n";
            for (int i = 0; i < MEM_SIZE; i++)
            {
                data[i] = 0;
                data_out.write(0);
            }
        }
        else if (write_enable.read() == 0)
        {
            if (read_clock.read() == 1)
            {
                cout << sc_time_stamp() << " | READING FROM MEMORY value: " << data[address.read()] << " at location " << address.read() << endl;
                data_out.write(data[address.read()]);
            }
        }
        else
        {
            if (write_clock.read() == 1)
            {
                cout << sc_time_stamp() << " | WRITING TO MEMORY value: " << data_in.read() << " at location " << address.read() << endl;
                data[address.read()] = data_in.read();
            }
        }
    }

    SC_CTOR(Memory)
    {
        SC_METHOD(run);
        sensitive << reset.pos() << read_clock.pos() << write_clock.pos();
    }
};

#endif