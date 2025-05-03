#pragma once

class OverlayFilter
{
public:
    virtual ~OverlayFilter() = default;

    // Process a single audio sample.
    virtual float processSample(float inputSample) = 0;

    // Optional parameter setters.
    virtual void setMix(float newMix) { mix = newMix; }
    virtual void setDrive(float newDrive) { drive = newDrive; }

protected:
    float mix{ 1.0f };   // 0.0 = effect off; 1.0 = fully effected.
    float drive{ 1.0f }; // Controls the intensity of the effect.
};