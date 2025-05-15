#pragma once

class OverlayFilter
{
public:
    virtual ~OverlayFilter() = default;

    // process a single audio sample.
    virtual float processSample(float inputSample) = 0;

    // parameter setters.
    virtual void setMix(float newMix) { mix = newMix; }
    virtual void setDrive(float newDrive) { drive = newDrive; }

protected:
    float mix{ 1.0f };   // 0.0 =  off; 1.0 ison.
    float drive{ 1.0f }; //  intensity of the effect.
};