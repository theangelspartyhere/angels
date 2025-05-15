#pragma once
#include "OverlayFilter.h"
#include <memory>

class OverlayFilterChain
{
public:
    //set the active overlay filter 
    void setActiveFilter(std::unique_ptr<OverlayFilter> newFilter)
    {
        activeFilter = std::move(newFilter);
    }

    // rocess a sample through the active overlay filter.
    float processSample(float inputSample)
    {
        if (!activeFilter)
            return inputSample;
        return activeFilter->processSample(inputSample);
    }

    // forward parameter settings to the active filter.
    void setMix(float newMix)
    {
        if (activeFilter)
            activeFilter->setMix(newMix);
    }

    void setDrive(float newDrive)
    {
        if (activeFilter)
            activeFilter->setDrive(newDrive);
    }

private:
    std::unique_ptr<OverlayFilter> activeFilter;
};