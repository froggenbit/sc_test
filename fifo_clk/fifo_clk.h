#pragma once

#include "systemc.h"
#include <typeinfo>
#include "fifo_clk_ifs.h"
#include "axis_fifo.h"

template<class T> class fifo_clk
: public fifo_clk_if<T>,
  public sc_channel
{
public:
    sc_in_clk clock;
    axis_fifo<T> fifo;

    //constructors
    SC_HAS_PROCESS(fifo_clk);
    explicit fifo_clk(unsigned size = 16) : sc_channel("fifo_clk"),
        clock("clock"),
        fifo("fifo",size)
    {
        m_reader = 0;
        m_writer = 0;
    }

    explicit fifo_clk(const sc_module_name name, unsigned size = 16) : sc_channel(name),
        clock("clock"),
        fifo("fifo", size)
    {
        m_reader = 0;
        m_writer = 0;
    }

    virtual int num_free() const{ return fifo.num_free(); }
    virtual const sc_event& data_read_event() const { return fifo.data_read_event(); }
    virtual int num_available() const { return fifo.num_available(); }
    virtual const sc_event& data_written_event() const { return fifo.data_written_event(); }
    virtual const sc_event_or_list data_available_event() const { return fifo.data_written_event(); }
    virtual void read(T& x) { fifo.read(x); }
    virtual T read(){ return fifo.read(); }
    virtual bool prefetch(T& x) const { return fifo.prefetch(x); }
    virtual bool nb_read(T& x) { return fifo.nb_read(x); }
    virtual void write( const T& x ) { fifo.write(x); }
    virtual bool nb_write( const T& x ) { return fifo.nb_write(x); }
    virtual void register_port( sc_port_base&, const char*);
    operator T () { T tmp; sc_assert(prefetch(tmp)); return tmp; }
    fifo_clk<T>& operator = ( const T& a ) { write( a ); return *this; }
    void trace( sc_trace_file* tf ) const{ fifo.trace(tf); }
    virtual void print( std::ostream& os = std::cout ) const { fifo.print(os); }
    virtual void dump( std::ostream& os = std::cout ) const { fifo.dump(os); }
    virtual const char* kind() const { return "fifo_clk"; }
protected:
    sc_port_base* m_reader;	// used for static design rule checking
    sc_port_base* m_writer;	// used for static design rule checking
private:
    // disabled
    fifo_clk( const fifo_clk<T>& );
    fifo_clk& operator = ( const fifo_clk<T>& );
};

template <class T>
inline void fifo_clk<T>::register_port( sc_port_base& port_, const char* if_typename_ )
{
    std::string nm( if_typename_ );
    if( nm == typeid( fifo_clk_in_if<T> ).name() ||
        //nm == typeid( fifo_clk_if<T> ).name() ||
        nm == typeid( sc_fifo_in_if<T> ).name() ||
        nm == typeid( sc_fifo_blocking_in_if<T> ).name()
    ) {
        // only one reader can be connected
        if( m_reader != 0 ) {
            SC_REPORT_ERROR( SC_ID_MORE_THAN_ONE_FIFO_READER_, 0 );
            // may continue, if suppressed
        }
        m_reader = &port_;
    } else if( nm == typeid( fifo_clk_out_if<T> ).name() ||
               nm == typeid( sc_fifo_out_if<T> ).name() ||
               nm == typeid( sc_fifo_blocking_out_if<T> ).name()
    ) {
        // only one writer can be connected
        if( m_writer != 0 ) {
            SC_REPORT_ERROR( SC_ID_MORE_THAN_ONE_FIFO_WRITER_, 0 );
            // may continue, if suppressed
        }
        m_writer = &port_;
    } else if ( nm == typeid( fifo_clk_if<T> ).name() ) {
        // only one reader and one writer can be connected
        if( m_reader != 0 )
            SC_REPORT_ERROR( SC_ID_MORE_THAN_ONE_FIFO_READER_, 0 );
        m_reader = &port_;
        if( m_writer != 0 )
            SC_REPORT_ERROR( SC_ID_MORE_THAN_ONE_FIFO_WRITER_, 0 );
        m_writer = &port_;

    } else
    {
        SC_REPORT_ERROR( SC_ID_BIND_IF_TO_PORT_,
                         "fifo_clk<T> port not recognized" );
        // may continue, if suppressed
    }
}

template<class T, unsigned N>
SC_MODULE(simple_fifo)
{
    sc_in_clk                clk;
    sc_port<fifo_clk_in_if<T>> in_port;
    sc_export<fifo_clk_if<T>> out_port;

    fifo_clk<T> fifo;

    SC_CTOR(simple_fifo):
        clk("clk"),
        in_port("in_port"),
        out_port("out_port"),
        fifo("out_fifo",N)
    {
        fifo.clock(clk);
        out_port(fifo);

        SC_THREAD(pipe_action);
    }

    void pipe_action()
    {
        using namespace std;
        while(true){
            wait(clk.posedge_event());
            if(in_port->num_available()==0 || fifo.num_free()==0) continue;
            sc_assert(fifo.nb_write(in_port->read()));
        }
    }
};

