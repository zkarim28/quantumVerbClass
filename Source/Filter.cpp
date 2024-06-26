/*
  ==============================================================================

    Filter.cpp

  ==============================================================================
*/

#include "Filter.h"
#include "PluginProcessor.h"

#include "Logger.h"
#include <complex>
#include <cmath> // For M_PI and pow

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a Filter object with optional frequency/gain/Q parameters
     *
     * The Filter object is constructed from the processor pointer and from the optional frequency/gain/Q parameters.
     * The frequency is the cut-off frequency of the derived LowShelfFilter/HighShelfFilter and the center frequency of the PeakFilter class.
     *
     * @param [in] processor    Pointer to main processor
     * @param [in] freq    Band frequency
     * @param [in] gain    Band gain
     * @param [in] q    Q factor
     */
    Filter::Filter(juce::AudioProcessor * processor, float freq, float q, float gain)
        : Task(processor), frequency(freq), Q(q), gainFactor(gain)
    {

    }

    Filter::~Filter(){
        
    }

    /**
     * @brief Updates parameters from processor parameter tree
     *
     * @param [in] params   Processor parameter tree
     * @param [in] blockId  ID of block whose paramters should be checked
     *
     */
    void Filter::updateParams(const juce::AudioProcessorValueTreeState& params,
        const juce::String& blockId)
    {
        // Frequency
        float _frequency = getParam(params,
                                    blockId + AudioProcessor::PID_FILTER_FREQ_SUFFIX);

        if (frequency != _frequency)
        {
            frequency = _frequency;
            buildFilter();
            mustExec = true;
        }

        // Q factor
        float _Q = getParam(params,
                            blockId + AudioProcessor::PID_FILTER_Q_SUFFIX);

        if (Q != _Q)
        {
            Q = _Q;
            buildFilter();
            mustExec = true;
        }
    }

    //==============================================================================
    /**
    * @brief Filters the audio in AudioSampleBuffer
    *
    * This function filters an AudioBuffer using the IIR filter's coefficients
    *
    * @param [in,out] ir   Contains the audio to be filtered, the output is placed in that same buffer
    *
    */
    AudioBlock Filter::exec(AudioBlock ir)
    {
        buildFilter();

        if (ir.getNumChannels() != 1)
        {
            logger.dualPrint(Logger::Level::Warning, "Filter: AudioBuffer channel count is not 1");
            return ir;
        }

        juce::dsp::ProcessContextReplacing<float> context(ir);
        juce::dsp::IIR::Filter<float>::process(context);

        // Reset mustExec flag
        mustExec = false;
        

        return ir;
    }

    //==============================================================================
    /**
    * @brief Returns the filter absolute amplitude response at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    */


#include <complex>
#include <cmath> // For M_PI and pow

float Filter::getAmplitude(float freq) {
    // All filters are 2nd order and thus have five coefficients
    float * coeffs = coefficients->getRawCoefficients();

    float b0 = coeffs[0];
    float b1 = coeffs[1];
    float b2 = coeffs[2];
    float a1 = coeffs[3];
    float a2 = coeffs[4];

    //Compute transfer function argument e^(jw)
    std::complex<float> input;
    std::complex<float> output;

    float minusOmega = -(2 * M_PI * freq) / processor->getSampleRate();

    input.real(std::cos(minusOmega));
    input.imag(std::sin(minusOmega));

    // Convert scalar coefficients to complex
    std::complex<float> b0_complex = static_cast<std::complex<float>>(b0);
    std::complex<float> b1_complex = static_cast<std::complex<float>>(b1);
    std::complex<float> b2_complex = static_cast<std::complex<float>>(b2);
    std::complex<float> a1_complex = static_cast<std::complex<float>>(a1);
    std::complex<float> a2_complex = static_cast<std::complex<float>>(a2);
    std::complex<float> one_complex = static_cast<std::complex<float>>(1.0f);

    // Compute and return transfer function amplitude
    return std::abs(b0_complex + b1_complex * input + b2_complex * std::pow(input, 2.0f)) /
           std::abs(one_complex + a1_complex * input + a2_complex * std::pow(input, 2.0f));
    }

    //==============================================================================
    /**
    * @brief Returns the filter amplitude response in dB at a given frequency
    *
    * @param [in] freq   Frequency at which the filter magnitude is evaluated
    *
    * @return Filter amplitude response in dB
    */

    float Filter::getdBAmplitude(float freq)
    {
        return 20 * std::log10(getAmplitude(freq));
    }

    //==============================================================================
    /**
    * @brief Sets the filter frequency and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Frequency to be set
    */


    void Filter::setFrequency(float freq)
    {

        frequency = freq;

        buildFilter();
    }

    //==============================================================================
    /**
    * @brief Sets the filter Q factor and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Q factor to be set
    */

    void Filter::setQ(float q)
    {

        Q = q;

        buildFilter();
    }
    //==============================================================================
    /**
    * @brief Sets the filter band gain and updates the IIR filter coefficients (Meant to be used by Equalizer class)
    *
    * @param [in] freq   Band gain to be set
    */

    void Filter::setGain(float gain)
    {

        gainFactor = gain;

        buildFilter();
    }

    //==============================================================================
    /**
    * @brief Generates and sets the low-shelf IIR filter coefficients according to the current filter parameters
    *
    */
    void LowShelfFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Generates and sets the high-shelf IIR filter coefficients according to the current filter parameters
    *
    */
    void HighShelfFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

    //==============================================================================
    /**
    * @brief Generates and sets the peak IIR filter coefficients according to the current filter parameters
    *
    */
    void PeakFilter::buildFilter()
    {

        coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(processor->getSampleRate(), 
            frequency, Q, gainFactor);
    }

}
