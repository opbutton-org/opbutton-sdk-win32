#pragma once

#include "OpButton/OpButtonCapturedFrame.h"

#include <cstddef>
#include <cstdint>

namespace OpButton
{
    class OpButtonScreenCaptureProvider
    {
    public:
        OpButtonScreenCaptureProvider();
        virtual ~OpButtonScreenCaptureProvider();

    public:
        // Called when live settings change (resolution, fps).
        // Provider should adjust its capture parameters accordingly.
        virtual void onLiveSettingsChanged( int _maxDimension, int _fps ) = 0;

        // Capture a frame. Fills `frame` with raw ARGB pixel data.
        // Returns true if a frame was successfully captured.
        // The data pointer must remain valid until the next call to captureFrame().
        virtual bool captureFrame( OpButtonCapturedFrame & _frame ) = 0;
    };

} // namespace OpButton
