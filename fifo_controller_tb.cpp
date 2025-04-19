#include <systemc.h>
#include "fifo_controller.cpp"

SC_MODULE(FifoController_tb)
{
    sc_signal<bool> reset;

    sc_signal<bool> write_clock[NUM_FIFOS];
    sc_signal<bool> read_clock[NUM_FIFOS];

    sc_signal<sc_uint<WORD_WIDTH>> data_in[NUM_FIFOS];
    sc_signal<sc_uint<WORD_WIDTH>> data_out[NUM_FIFOS];

    FifoController fifoController;

    void clock_gen()
    {
        while (true)
        {
            write_clock[0].write(0);
            read_clock[0].write(0);
            wait(5, SC_NS);
            read_clock[0].write(1);
            wait(5, SC_NS);
            read_clock[0].write(0);
            wait(5, SC_NS);
            read_clock[0].write(1);
            wait(5, SC_NS);
            read_clock[0].write(0);
            wait(5, SC_NS);
            write_clock[0].write(1);
            wait(5, SC_NS);
            write_clock[0].write(0);
            wait(5, SC_NS);
            write_clock[0].write(1);
            wait(5, SC_NS);
        }
    }

    void data_gen()
    {
        wait(3, SC_NS);

        // reset
        reset.write(1);
        wait(10, SC_NS);
        reset.write(0);
        wait(10, SC_NS);

        // TODO: test pattern
        read_clock[0].write(1);
        data_in[0].write(0xFF);
        read_clock[0].write(0);
        wait(10, SC_NS);

        sc_stop();
    }

    SC_CTOR(FifoController_tb) : fifoController("FIFO_CONTROLLER")
    {
        fifoController.reset(reset);
        for (int i = 0; i < NUM_FIFOS; i++)
        {
            fifoController.write_clock[i](write_clock[i]);
            fifoController.read_clock[i](read_clock[i]);
            fifoController.data_in[i](data_in[i]);
            fifoController.data_out[i](data_out[i]);
        }
        SC_THREAD(clock_gen);
        SC_THREAD(data_gen);
    }
};

int sc_main(int argc, char *argv[])
{
    FifoController_tb tb("FifoControllerTB");

    sc_trace_file *tf = sc_create_vcd_trace_file("waveform");

    for (int i = 0; i < NUM_FIFOS; i++)
    {
        sc_trace(tf, tb.read_clock[i], "read_clock_" + std::to_string(i));
        sc_trace(tf, tb.write_clock[i], "write_clock_" + std::to_string(i));
        sc_trace(tf, tb.data_in[i], "data_in_" + std::to_string(i));
        sc_trace(tf, tb.data_out[i], "data_out_" + std::to_string(i));
    }

    sc_trace(tf, tb.reset, "reset");

    sc_start();

    sc_close_vcd_trace_file(tf);

    return 0;
}