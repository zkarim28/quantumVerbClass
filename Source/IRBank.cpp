/*
  ==============================================================================

    IRBank.cpp

  ==============================================================================
*/

#include "IRBank.h"
#include <juce_audio_formats/juce_audio_formats.h>

namespace reverb
{

//==============================================================================
/**
 * Global IRBank object
 */
const IRBank& IRBank::getInstance()
{
    static IRBank irBank;
    return irBank;
}

//==============================================================================
/**
 * @brief Construct IR bank
 *
 * Sets public buffer & sample rate references and calls build() method to populate
 * IR bank.
 */
IRBank::IRBank()
: buffers(buffersModifiable), sampleRates(sampleRatesModifiable)
{
    build();
}

//==============================================================================
/**
 * @brief Build IR bank from binary data
 *
 * Finds all audio resources and builds sample buffers for each. Rejects any
 * resources with the wrong format (only keep WAVE or AIFF).
 */
//void IRBank::build()
//{
//    if (!buffersModifiable.empty() && !sampleRatesModifiable.empty())
//    {
//        return;
//    }
//    else if (buffersModifiable.size() != sampleRatesModifiable.size())
//    {
//        buffersModifiable.clear();
//        sampleRatesModifiable.clear();
//    }
//
//    // Register basic audio formats
//    juce::AudioFormatManager formatMgr;
//    formatMgr.registerBasicFormats();
//
//    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
//    {
//        // Prepare memory input stream
//        int dataSize;
//        const char * data;
//
//        data = BinaryData::getNamedResource(BinaryData::namedResourceList[i], dataSize);
//        auto dataStream = new juce::MemoryInputStream(data, dataSize, false);
//
//        // Attempt to create an audio format reader for stream
//        std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(dataStream));
//
//        // Attempt to create an audio format reader for the stream
//        //            std::unique_ptr<juce::MemoryInputStream> dataStream = std::make_unique<juce::MemoryInputStream>(/* parameters */);
//
//
//
//        // If reader was successfully created, this is a valid audio resource
//        if (reader)
//        {
//            int numChannels = reader->numChannels;
//            int numSamples = (int)reader->lengthInSamples;
//
//            buffersModifiable[BinaryData::namedResourceList[i]] = juce::AudioSampleBuffer(numChannels, numSamples);
//            reader->read(&buffersModifiable[BinaryData::namedResourceList[i]], 0, numSamples, 0, true, true);
//
//            sampleRatesModifiable[BinaryData::namedResourceList[i]] = reader->sampleRate;
//        }
//    }
//}
//}

#include <juce_audio_formats/juce_audio_formats.h>

void IRBank::build()
{
    // Check if buffers and sample rates are already filled
    if (!buffersModifiable.empty() && !sampleRatesModifiable.empty())
    {
        return; // Exit if they are not empty
    }
    // Ensure buffers and sample rates have the same size, otherwise clear them
    else if (buffersModifiable.size() != sampleRatesModifiable.size())
    {
        buffersModifiable.clear();
        sampleRatesModifiable.clear();
    }
    
    // Create an AudioFormatManager and register basic formats (WAV, AIFF, etc.)
    juce::AudioFormatManager formatMgr;
    formatMgr.registerBasicFormats();
    
    // Iterate over all named resources in BinaryData
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
    {
        int dataSize;
        // Get the data for the current resource
        const char* data = BinaryData::getNamedResource(BinaryData::namedResourceList[i], dataSize);
        
        // Create a MemoryInputStream for the resource data
        auto dataStream = std::make_unique<juce::MemoryInputStream>(data, dataSize, false);
        
        // Create an AudioFormatReader using the format manager and the memory input stream
        std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(std::move(dataStream)));
        
        // Check if the reader was successfully created
        if (reader != nullptr)
        {
            // Get number of channels and number of samples from the reader
            int numChannels = reader->numChannels;
            int numSamples = static_cast<int>(reader->lengthInSamples);
            
            // Allocate an AudioSampleBuffer with the appropriate number of channels and samples
            buffersModifiable[BinaryData::namedResourceList[i]] = juce::AudioSampleBuffer(numChannels, numSamples);
            
            // Read the audio data into the buffer
            reader->read(&buffersModifiable[BinaryData::namedResourceList[i]], 0, numSamples, 0, true, true);
            
            // Store the sample rate of the audio data
            sampleRatesModifiable[BinaryData::namedResourceList[i]] = reader->sampleRate;
        }
    }
}
}
