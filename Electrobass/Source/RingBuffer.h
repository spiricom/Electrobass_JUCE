
/*
  ==============================================================================
    adapted by Jeff Snyder from WaveformComponent.h by Alex Johnson
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <vector>

using std::vector;
using std::array;

template <typename Type>
class AudioBufferQueue {
public:
    static constexpr size_t order = 9; // number of points.
    static constexpr size_t bufferSize = 1U << order; // 512 size
    static constexpr size_t size = 5;

    // Add data to the buffer
    void push(const Type* dataToPush, size_t numSamples) {
        jassert(numSamples <= bufferSize);

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite(1, start1, size1, start2, size2); // Creates a FIFO to manage a buffer with the specified capacity.

        if (size1 > 0) // If size1 is positive, copy datatopush into the buffer.
            juce::FloatVectorOperations::copy(buffers[(size_t)start1].data(), dataToPush, (int)juce::jmin(bufferSize, numSamples));

        abstractFifo.finishedWrite(size1); // Sets the end of the queue.
    }//push()

    // Delete data from the buffer
    void pop(Type* outputBuffer) {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(1, start1, size1, start2, size2);

        if (size1 > 0) // If size1 is positive, copy data from the buffer into the output buffer.
            juce::FloatVectorOperations::copy(outputBuffer, buffers[(size_t)start1].data(), (int)bufferSize);

        abstractFifo.finishedRead(size1); // Sets the start of the queue.
    }//pop()

private:
    juce::AbstractFifo abstractFifo { size };
    array<array<Type, bufferSize>, size> buffers;
};//AudioBufferQueue


template <typename Type>
class ScopeDataCollector {
public:
    ScopeDataCollector(AudioBufferQueue<Type>& queueToUse) : audioBufferQueue(queueToUse) {}

    void process(const Type* data, size_t numSamples) {
        size_t index = 0;

        // If the sample hits a trigger threshold:
        if (state == State::waitingForTrigger) {
            while (index++ < numSamples) {
                auto curSample = (*data++) + ((0.0001f * (juce::Random::getSystemRandom().nextFloat() - 0.5f))); // Get the current sample, then move onto the next.  Add a tiny bit of noise to avoid stuck scope lines when amplitude goes to zero.
                // If a new trigger level:
                if ((curSample >= triggerLevel) && (prevSample < triggerLevel)) {
                    collectedIndex = 0;
                    state = State::collecting;
                    break;
                }//if
                prevSample = curSample;
            }//while
        }//if

        // Audio is ready for collecting:
        if (state == State::collecting) {
            while (index++ < numSamples) {
                buffer[collectedIndex++] = *data++; // Copy current sample into the buffer, then move onto the next.
                // If we hit the end of the buffer, push and reset.
                if (collectedIndex == buffer.size()) {
                    audioBufferQueue.push(buffer.data(), buffer.size());
                    state = State::waitingForTrigger;
                    prevSample = Type(100);
                    break;
                }//if
            }//while
        }//if
    }//process()

private:
    AudioBufferQueue<Type> &audioBufferQueue;
    array<Type, AudioBufferQueue<Type>::bufferSize> buffer;
    size_t collectedIndex; // Buffer index.
    Type prevSample = Type(100);

    static constexpr auto triggerLevel = Type(0.0f); // Visualizer threshold

    enum class State { waitingForTrigger, collecting };
    State state = State::waitingForTrigger;
};//ScopeDataCollector


template <typename Type>
class WaveformComponent : public juce::Component, private juce::Timer {
public:
    WaveformComponent(AudioBufferQueue<Type>& queueToUse) : audioBufferQueue(queueToUse) {
        sampleData.fill(Type(0));
        startTimerHz(90); // 90 fps
    }//ScopeComponent()

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().reduced(5);

        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds.toFloat(), 5.0f, 2.0f);

        // Position of the plot
        auto area = getLocalBounds();
        auto h = (Type)area.getHeight() * 2;
        auto w = (Type)area.getWidth();

        // Oscilloscope
        g.setColour(Colours::white);
        auto scopeRect = juce::Rectangle<Type>{ Type(0), Type(0), w - 4.0f, h / 2.0f  };
        plot(sampleData.data(), sampleData.size(), g, scopeRect, 0.8f, h/4.0f);
    }//paint()

    void resized() override {}

private:
    AudioBufferQueue<Type>& audioBufferQueue; // Buffers
    array<Type, AudioBufferQueue<Type>::bufferSize> sampleData;

    // Clear the buffer and paint the waveform.
    void timerCallback() override {
        audioBufferQueue.pop(sampleData.data());
        repaint();
    }//timerCallback()

    // Draw the oscilloscope with jmap
    static void plot(const Type* data,
        size_t numSamples,
        juce::Graphics& g,
        juce::Rectangle<Type> rect,
        Type scaler = Type(1),
        Type offset = Type(0))
    {
        auto w = rect.getWidth() - 4.0f;
        auto h = rect.getHeight()/2.0f;
        auto right = rect.getRight();

        auto center = rect.getBottom() - offset;
        auto gain = h * scaler;

        for (size_t i = 1; i < numSamples; ++i)
            g.drawLine({ juce::jmap(Type(i-1), Type(0), Type(numSamples-1), Type(right-w), Type(right)),
                          center-gain * data[i-1],
                          juce::jmap(Type(i), Type(0), Type(numSamples-1), Type(right-w), Type(right)),
                          center-gain * data[i] }, 2.0f);
    }//plot()
};//WaveformComponent
