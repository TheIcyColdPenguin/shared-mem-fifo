#include <systemc.h>
#include "fifo_controller.cpp"

SC_MODULE(FifoController_tb)
{
    sc_signal<bool> reset;

    sc_signal<sc_bv<NUM_FIFOS>> write_clock;
    sc_signal<sc_bv<NUM_FIFOS>> read_clock;

    sc_signal<sc_uint<WORD_WIDTH>> data_in[NUM_FIFOS];
    sc_signal<sc_uint<WORD_WIDTH>> data_out[NUM_FIFOS];

public:
    FifoController fifoController;

    void data_gen()
    {
        cout << sc_time_stamp() << " || SIM: Reset" << endl;

        wait(3, SC_NS);

        // reset
        read_clock = "00";
        write_clock = "00";

        reset = 1;
        wait(10, SC_NS);
        reset = 0;
        wait(10, SC_NS);

        // write data one by one into all FIFOs

        cout << sc_time_stamp() << " || SIM: Write to all FIFOs\n\n";

        int i = 0;
        int test_count = 16;
        while (i < test_count)
        {
            for (int j = 0; j < NUM_FIFOS; j++)
            {
                cout << sc_time_stamp() << " || SIM: Write " << i << " to FIFO_" << j << endl;

                data_in[j] = i;
                wait(SC_ZERO_TIME);
                write_clock = 1 << j;
                wait(5, SC_NS);
                write_clock = 0;
                wait(5, SC_NS);
                i++;
            }
        }

        //  read back data from all fifos sequentially
        cout << sc_time_stamp() << " || SIM: Write to all FIFOs\n\n";

        for (int j = 0; j < NUM_FIFOS; j++)
        {
            cout << sc_time_stamp() << " || SIM: Read FIFO_" << j << endl;

            for (int i = 0; i < test_count / NUM_FIFOS; i++)
            {
                read_clock = 1 << j;
                wait(5, SC_NS);
                read_clock = 0;
                wait(5, SC_NS);
            }
        }

        wait(10, SC_NS);

        cout << sc_time_stamp() << " || SIM: Exit sim" << endl;
        sc_stop();
    }

    // void data_gen()
    // {
    //     cout << sc_time_stamp() << " || SIM: Reset" << endl;

    //     wait(3, SC_NS);

    //     // reset
    //     read_clock = "00";
    //     write_clock = "00";

    //     reset = 1;
    //     wait(10, SC_NS);
    //     reset = 0;
    //     wait(10, SC_NS);

    //     // TODO: test pattern

    //     // write 0xFF then 0x10 into fifo_1

    //     cout << sc_time_stamp() << " || SIM: Write 0xFF then 0x10" << endl;
    //     // wait(5, SC_NS);
    //     data_in[0] = 0xFF;
    //     wait(SC_ZERO_TIME);
    //     write_clock = "01";
    //     wait(5, SC_NS);
    //     write_clock = "00";
    //     wait(5, SC_NS);

    //     data_in[0] = 0x10;
    //     write_clock = "01";
    //     wait(5, SC_NS);
    //     write_clock = "00";
    //     wait(5, SC_NS);

    //     cout << sc_time_stamp() << " || SIM: Read 0xFF then 0x10" << endl;
    //     // read out 0xFF then 0x10 from fifo_1
    //     read_clock = "01";
    //     wait(5, SC_NS);
    //     read_clock = "00";
    //     wait(5, SC_NS);
    //     read_clock = "01";
    //     wait(5, SC_NS);
    //     read_clock = "00";
    //     wait(5, SC_NS);

    //     cout << sc_time_stamp() << " || SIM: Exit sim" << endl;
    //     sc_stop();
    // }

    SC_CTOR(FifoController_tb)
        : fifoController("FIFO_CONTROLLER")
    {
        fifoController.reset(reset);
        fifoController.write_clock(write_clock);
        fifoController.read_clock(read_clock);

        for (int i = 0; i < NUM_FIFOS; i++)
        {
            fifoController.data_in[i](data_in[i]);
            fifoController.data_out[i](data_out[i]);
        }

        SC_THREAD(data_gen);
    }
};

int sc_main(int argc, char *argv[])
{
    FifoController_tb tb("FifoControllerTB");

    sc_trace_file *tf = sc_create_vcd_trace_file("waveform");

    sc_trace(tf, tb.fifoController.read_clock, "read_clock");
    sc_trace(tf, tb.fifoController.write_clock, "write_clock");

    for (int i = 0; i < NUM_FIFOS; i++)
    {
        sc_trace(tf, tb.fifoController.data_in[i], "data_in_" + std::to_string(i));
        sc_trace(tf, tb.fifoController.data_out[i], "data_out_" + std::to_string(i));
    }

    sc_trace(tf, tb.reset, "reset");

    sc_start();

    sc_close_vcd_trace_file(tf);

    return 0;
}