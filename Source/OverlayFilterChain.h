#pragma once
#include "OverlayFilter.h"
#include <memory>

class OverlayFilterChain
{
public:
    // Set the active overlay filter (for example, a VileFilter).
    void setActiveFilter(std::unique_ptr<OverlayFilter> newFilter)
    {
        activeFilter = std::move(newFilter);
    }

    // Process a sample through the active overlay filter.
    float processSample(float inputSample)
    {
        if (!activeFilter)
            return inputSample;
        return activeFilter->processSample(inputSample);
    }

    // Forward parameter settings to the active filter.
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