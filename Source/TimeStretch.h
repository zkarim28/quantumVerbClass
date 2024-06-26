/*
  ==============================================================================

    TimeStretch.h

  ==============================================================================
*/

#pragma once

#include "signalsmith-stretch-main/signalsmith-stretch.h"

#include "Task.h"

namespace reverb
{

    //==============================================================================
    /**
     * Implements a time stretching algorithm to manage IR buffer length and sample rate.
     */
    class TimeStretch : public Task
    {
    public:
        //==============================================================================
        TimeStretch(juce::AudioProcessor * processor);
        virtual ~TimeStretch() = default;
        

        //==============================================================================
        using Ptr = std::shared_ptr<TimeStretch>;

        //==============================================================================
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockID) override;

        virtual AudioBlock exec(AudioBlock ir) override;

        //==============================================================================
        void prepareIR(juce::AudioSampleBuffer& ir);
        int getOutputNumSamples();

    protected:
        //==============================================================================
        static constexpr double MAX_IR_LENGTH_S = 5.0f;

        float irLengthS = 3.0f;

        //==============================================================================
        juce::AudioSampleBuffer irOrig;

        //==============================================================================
//        std::unique_ptr<soundtouch::SoundTouch> soundtouch;
        std::unique_ptr<signalsmith::stretch::SignalsmithStretch<float>> stretchProcessor;
    };

}
