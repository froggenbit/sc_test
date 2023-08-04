#pragma once

#include "systemc.h"

template <class T>
class fifo_clk_out_if : public sc_fifo_out_if<T> {
protected:
    // constructor
    fifo_clk_out_if() {
    }
private:
    // disabled
    fifo_clk_out_if( const fifo_clk_out_if<T>& );
    fifo_clk_out_if<T>& operator = ( const fifo_clk_out_if<T>& );
};

template <class T>
class fifo_clk_in_if : public sc_fifo_in_if<T> {
public:
    virtual bool prefetch( T& val_ ) const = 0;
    virtual const sc_event_or_list data_available_event() const = 0;
protected:
    // constructor
    fifo_clk_in_if() {
    }
private:
    // disabled
    fifo_clk_in_if( const fifo_clk_in_if<T>& );
    fifo_clk_in_if<T>& operator = ( const fifo_clk_in_if<T>& );
};

template <class T>
class fifo_clk_if : public fifo_clk_in_if<T>, public fifo_clk_out_if<T> {
protected:
    // constructor
    fifo_clk_if(){
    }
private:
    // disabled
    fifo_clk_if( const fifo_clk_if<T>& );
    fifo_clk_if<T>& operator = ( const fifo_clk_if<T>& );
};