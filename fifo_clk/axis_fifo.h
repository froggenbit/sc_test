#pragma once

// fifo with prefetch function & size can be 0

#include "systemc.h"

template <class T>
class axis_fifo
: public sc_fifo<T>
{
public:
    const bool zero_size;
    unsigned read_cnt;

    explicit axis_fifo( unsigned size_ = 16 )
        : sc_fifo<T>( size_ != 0 ? size_ : 1 ),
        zero_size(size_ == 0),
        read_cnt(0)
    {}

    explicit axis_fifo( const char* name_, unsigned size_ = 16 )
        : sc_fifo<T>( name_, size_ != 0 ? size_ : 1 ),
        zero_size(size_ == 0),
        read_cnt(0)
    {}

    virtual int num_free() const
    {
        if (zero_size)
            return read_cnt == 0 ? 0 : 1;
        else
            return sc_fifo<T>::num_free();
    }

    virtual void write( const T& val_ )
    {
        // if fifo_full, wait for read event
        while(sc_fifo<T>::num_free() == 0 ) {
            wait(sc_fifo<T>::m_data_read_event );
        }
        bool write_success = nb_write(val_);
        if (zero_size && write_success)
            wait( sc_core::sc_fifo<T>::m_data_read_event );
        sc_assert(write_success);
    }

    virtual bool nb_write( const T& val_ )
    {
        // if fifo_full, return false
        if( sc_fifo<T>::num_free() == 0 ) {
            return false;
        }
        bool write_success = sc_fifo<T>::buf_write( val_ );
        if( SC_LIKELY_(write_success) ) {
            sc_fifo<T>::m_num_written ++;
            sc_fifo<T>::request_update();
        }
        return write_success;
    }

    virtual void read( T& val_ )
    {
        read_cnt++;
        sc_fifo<T>::read(val_);
        read_cnt--;
    }

    virtual T read()
    {
        T tmp;
        read(tmp);
        return tmp;
    }

    virtual bool prefetch( T& val_ ) const
    {
        // if fifo_empty return false
        if( sc_fifo<T>::num_available() == 0)
            return false;
        val_ = sc_fifo<T>::m_buf[sc_fifo<T>::m_ri];
        return true;
    }

    operator T ()
	{
        T tmp;
        sc_assert(prefetch(tmp));
        return tmp;
    }

    axis_fifo<T>& operator = ( const T& a )
        { write( a ); return *this; }

private:

    // disabled
    axis_fifo( const axis_fifo<T>& );
    axis_fifo& operator = ( const axis_fifo<T>& );
};

