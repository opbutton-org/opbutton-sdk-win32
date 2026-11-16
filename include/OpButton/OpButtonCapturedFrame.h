#pragma once

#include <cstddef>
#include <cstdint>

namespace OpButton
{
    struct OpButtonCapturedFrame
    {
    public:
        OpButtonCapturedFrame();
        ~OpButtonCapturedFrame();
        OpButtonCapturedFrame( const uint8_t * _data, size_t _dataSize, int _width, int _height, int _sourceWidth, int _sourceHeight );

        const uint8_t * data; // raw ARGB pixel data (4 bytes per pixel)
        size_t dataSize;
        int width;
        int height;
        int sourceWidth;  // original window/screen width before scaling
        int sourceHeight; // original window/screen height before scaling
    };
} // namespace OpButton
