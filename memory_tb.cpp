#include <systemc.h>
#include "memory.cpp"

SC_MODULE(Memory_tb)
{
    sc_signal<bool> read_clock;
    sc_signal<bool> write_clock;
    sc_signal<bool> reset;
    sc_signal<bool> write_enable;
    sc_signal<sc_uint<ADDR_WIDTH>> address_read;
    sc_signal<sc_uint<ADDR_WIDTH>> address_write;
    sc_signal<sc_uint<WORD_WIDTH>> data_in;
    sc_signal<sc_uint<WORD_WIDTH>> data_out;

    Memory ram;

    void clock_gen()
    {
        while (true)
        {
            read_clock.write(0);
            write_clock.write(0);
            wait(5, SC_NS);
            read_clock.write(1);
            write_clock.write(1);
            wait(5, SC_NS);
        }
    }

    void data_gen()
    {
        wait(3, SC_NS);

        // reset
        reset.write(1);
        cout << sc_time_stamp() << " | set reset = 1" << endl;
        wait(10, SC_NS);
        reset.write(0);
        cout << sc_time_stamp() << " | set reset = 0" << endl;
        wait(10, SC_NS);

        // write 0x14 to 0x12
        write_enable.write(1);
        data_in.write(0x14);
        address_write.write(0x12);
        cout << sc_time_stamp() << " | set write_enable = 1" << endl;
        cout << sc_time_stamp() << " | set data_in = 0x14" << hex << endl;
        cout << sc_time_stamp() << " | set address = 0x12" << endl;
        wait(10, SC_NS);

        // read from 0x10
        write_enable.write(0);
        address_read.write(0x10);
        cout << sc_time_stamp() << " | set write_enable = 0" << endl;
        cout << sc_time_stamp() << " | set address = 0x10" << endl;
        wait(10, SC_NS);

        // read from 0x12
        write_enable.write(0);
        address_read.write(0x12);
        cout << sc_time_stamp() << " | set write_enable = 0" << endl;
        cout << sc_time_stamp() << " | set address = 0x12" << endl;
        wait(10, SC_NS);

        sc_stop();
    }

    void monitor()
    {
        cout << sc_time_stamp() << " | data_out = " << data_out.read() << endl;
    }

    SC_CTOR(Memory_tb) : ram("RAM")
    {

        ram.read_clock(read_clock);
        ram.write_clock(write_clock);
        ram.reset(reset);
        ram.write_enable(write_enable);
        ram.address_read(address_read);
        ram.address_write(address_write);
        ram.data_in(data_in);
        ram.data_out(data_out);

        SC_THREAD(clock_gen);
        SC_THREAD(data_gen);

        SC_METHOD(monitor);
        dont_initialize();
        sensitive << data_out;
    }
};

// int sc_main(int argc, char *argv[])
// {
//     Memory_tb tb("MemoryTB");

//     sc_trace_file *tf = sc_create_vcd_trace_file("waveform");

//     sc_trace(tf, tb.read_clock, "read_clock");
//     sc_trace(tf, tb.write_clock, "write_clock");
//     sc_trace(tf, tb.reset, "reset");
//     sc_trace(tf, tb.write_enable, "write_enable");
//     sc_trace(tf, tb.address_read, "address_read");
//     sc_trace(tf, tb.address_write, "address_write");
//     sc_trace(tf, tb.data_in, "data_in");
//     sc_trace(tf, tb.data_out, "data_out");

//     sc_start();

//     sc_close_vcd_trace_file(tf);

//     return 0;
// }