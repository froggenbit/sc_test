#include "systemc.h"
#include "fifo_clk.h"
using namespace std;

SC_MODULE(fifo_clk_top)
{
    sc_in_clk clock;
    sc_signal<unsigned> din, dout;
    fifo_clk<unsigned> fifo;

    SC_CTOR(fifo_clk_top):clock("clock"), din("din"), dout("dout"), fifo("fifo",2)
    {
        fifo.clock(clock);
        SC_THREAD(wr_fifo);
        SC_THREAD(rd_fifo);
    }

    void wr_fifo(void)
    {
        // delay 1 or 2 cycle
        unsigned delay_num = rand()%2 + 1;
        unsigned data = 0;
        // assign din = data
        din.write(data);
        cout << sc_time_stamp() << ": first wr delay : "<< --delay_num << "; first wr data : " << data << endl;
        while(true){
            // fifo wr at clk posedge
            wait(clock->posedge_event());
            if(delay_num-- != 0) continue;
            // write begin at any time
            // cout << sc_time_stamp() << ": wr begin data : "<< din.read() << endl;
            // cout << sc_time_stamp() << ": before write, #free/#available=" << fifo.num_free() << "/" << fifo.num_available() << endl;
            // blocking write, mean = in verilog
            // fifo.write(din.read());
            // non-blocking write, means <= in verilog
            if(fifo.nb_write(din.read())) {
                // write done until ~fifo_full(if the fifo is full, write() shall suspend until the data-read event is notified.)
                cout << sc_time_stamp() << ": wr successed data : " << din.read() << endl;
                data ++;
            }
            else {
                cout << sc_time_stamp() << ": wr data failed : " << din.read() << endl;
            }
            // new write delay
            delay_num = rand() % 2 + 1;
            din.write(data);
        }
    }
    void rd_fifo(void)
    {
        // delay 1,2,3 or 4 cycle
        unsigned delay_num = rand() % 4 + 1;
        unsigned data = 0;
        dout.write(0);
        cout << sc_time_stamp() << ": first rd delay : "<< --delay_num << endl;
        while (true){
            wait(clock->posedge_event());
            if(delay_num-- != 0) continue;
            // read begin at any time
            // cout << sc_time_stamp() << ": rd begin data"<< endl;
            // blocking read, mean = in verilog
            // read done until ~fifo_empty(if the fifo is empty, shall suspend until the data-written event is notified)
            // fifo.read(data);
            // cout << sc_time_stamp() << ": rd done data : "<< data << endl;
            // non-blocking read, mean <= in verilog
            if(fifo.nb_read(data)) {
                // write done until ~fifo_full
                cout << sc_time_stamp() << ": rd successed data : " << data << endl;
            }
            else {
                cout << sc_time_stamp() << ": rd data failed " << endl;
            }
            dout.write(data);
            // new rd delay
            delay_num = rand() % 4 + 1;
        }
    }
};
int sc_main(int, char **)
{
    sc_clock clock("clock",2,SC_NS,0.5,0,SC_SEC, false);
    fifo_clk_top top("top");
    top.clock(clock);
    sc_start(100, SC_NS);
    return 0;
}
