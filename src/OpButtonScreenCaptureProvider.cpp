#include "OpButton/OpButtonScreenCaptureProvider.h"

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonScreenCaptureProvider::~OpButtonScreenCaptureProvider()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonCapturedFrame::OpButtonCapturedFrame()
        : data()
        , dataSize()
        , width()
        , height()
        , sourceWidth()
        , sourceHeight()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonCapturedFrame::~OpButtonCapturedFrame()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonCapturedFrame::OpButtonCapturedFrame( const uint8_t * _data, size_t _dataSize, int _width, int _height, int _sourceWidth, int _sourceHeight )
        : data( _data )
        , dataSize( _dataSize )
        , width( _width )
        , height( _height )
        , sourceWidth( _sourceWidth )
        , sourceHeight( _sourceHeight )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonScreenCaptureProvider::OpButtonScreenCaptureProvider()
    {
    }
} // namespace OpButton
