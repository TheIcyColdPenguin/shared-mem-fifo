#include <systemc.h>

#ifndef MEMORY
#define MEMORY

#define ADDR_WIDTH 4               // in bits
#define MEM_SIZE (1 << ADDR_WIDTH) // in words
#define WORD_WIDTH 4               // in bits

SC_MODULE(Memory)
{

    sc_in_clk read_clock;
    sc_in_clk write_clock;
    sc_in<bool> reset;
    sc_in<bool> write_enable;
    sc_in<sc_uint<ADDR_WIDTH>> address_read;
    sc_in<sc_uint<ADDR_WIDTH>> address_write;
    sc_in<sc_uint<WORD_WIDTH>> data_in;

    sc_out<sc_uint<WORD_WIDTH>> data_out;

    sc_uint<WORD_WIDTH> data[MEM_SIZE];

    void read()
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
        else if (read_clock.read() == 1 && write_enable.read() == 0)
        {
            cout << sc_time_stamp() << " | READING FROM MEMORY value: " << data[address_read.read()] << " at location " << address_read.read() << endl;
            data_out.write(data[address_read.read()]);
        }
    }

    void write()
    {
        if (write_clock.read() == 1 && write_enable.read() == 1)
        {
            cout << sc_time_stamp() << " | WRITING TO MEMORY value: " << data_in.read() << " at location " << address_write.read() << endl;
            data[address_write.read()] = data_in.read();
        }
    }

    SC_CTOR(Memory)
    {
        SC_METHOD(read);
        sensitive << reset.pos() << read_clock.pos();

        SC_METHOD(write);
        sensitive << write_clock.pos();
    }
};

#endif