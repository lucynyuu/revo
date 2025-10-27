//
//  RevoDSPKernel.hpp
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-18.
//

#pragma once

#import <AudioToolbox/AudioToolbox.h>
#import <algorithm>
#import <vector>
#import <span>

#import "RevoParameterAddresses.h"

/*
 RevoDSPKernel
 As a non-ObjC class, this is safe to use from render thread.
 */
class RevoDSPKernel {
public:
    void initialize(int inputChannelCount, int outputChannelCount, double inSampleRate) {
        mSampleRate = inSampleRate;
        
        mMaxDelaySamples = static_cast<size_t>(mSampleRate * 3.0);
        mDelayBuffer.resize(inputChannelCount);
        mWriteIndex.resize(inputChannelCount, 0);
        for (auto& buffer : mDelayBuffer) {
            buffer.assign(mMaxDelaySamples, 0.0f);
        }
    }
    
    void deInitialize() {
    }
    
    // MARK: - Bypass
    bool isBypassed() {
        return mBypassed;
    }
    
    void setBypass(bool shouldBypass) {
        mBypassed = shouldBypass;
    }
    
    // MARK: - Parameter Getter / Setter
    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case RevoParameterAddress::timeinterval:
                mTimeInterval = value;
                break;
            case RevoParameterAddress::dry:
                mDry = value/100;
                break;
            case RevoParameterAddress::wet:
                mWet = value/100;
                break;
            case RevoParameterAddress::feedback:
                mFeedback = value/100;
                break;
            case RevoParameterAddress::zeno:
                mZeno = (bool)value;
                break;
            case RevoParameterAddress::pingpong:
                mPingPong = (bool)value;
                break;
            case RevoParameterAddress::frequency:
                mFrequency = value;
                break;
                // Add a case for each parameter in RevoParameterAddresses.h
        }
    }
    
    AUValue getParameter(AUParameterAddress address) {
        // Return the goal. It is not thread safe to return the ramping value.
        
        switch (address) {
            case RevoParameterAddress::timeinterval:
                return (AUValue)mTimeInterval;
            case RevoParameterAddress::dry:
                return (AUValue)(mDry*100);
            case RevoParameterAddress::wet:
                return (AUValue)(mWet*100);
            case RevoParameterAddress::feedback:
                return (AUValue)(mFeedback*100);
            case RevoParameterAddress::zeno:
                return (AUValue)((float)mZeno);
            case RevoParameterAddress::pingpong:
                return (AUValue)((float)mPingPong);
            case RevoParameterAddress::frequency:
                return (AUValue)mFrequency;
            default: return 0.f;
        }
    }
    
    // MARK: - Max Frames
    AUAudioFrameCount maximumFramesToRender() const {
        return mMaxFramesToRender;
    }
    
    void setMaximumFramesToRender(const AUAudioFrameCount &maxFrames) {
        mMaxFramesToRender = maxFrames;
    }
    
    // MARK: - Musical Context
    void setMusicalContextBlock(AUHostMusicalContextBlock contextBlock) {
        mMusicalContextBlock = contextBlock;
    }
    
    /**
     MARK: - Internal Process
     
     This function does the core siginal processing.
     Do your custom DSP here.
     */
    void process(std::span<float const*> inputBuffers, std::span<float *> outputBuffers, AUEventSampleTime bufferStartTime, AUAudioFrameCount frameCount) {
        assert(inputBuffers.size() == outputBuffers.size());
        const UInt32 numChannels = static_cast<UInt32>(inputBuffers.size());

        if (mBypassed) {
            for (UInt32 ch = 0; ch < numChannels; ++ch)
                std::copy_n(inputBuffers[ch], frameCount, outputBuffers[ch]);
            return;
        }

        size_t baseDelaySamples = static_cast<size_t>(mTimeInterval * mSampleRate);
        baseDelaySamples = std::min(baseDelaySamples, mMaxDelaySamples - 1);
        if (baseDelaySamples < 1)
            baseDelaySamples = 1;

        const float twoPi = 2.0f * static_cast<float>(M_PI);
        const float phaseInc = twoPi * mFrequency / static_cast<float>(mSampleRate);

        for (UInt32 i = 0; i < frameCount; ++i) {
            float pan = 0.0f;
            if (mPingPong && numChannels >= 2)
                pan = std::sin(mPhase);

            if (mPingPong) {
                mPhase += phaseInc;
                if (mPhase >= twoPi)
                    mPhase -= twoPi;
                else if (mPhase < 0.0f)
                    mPhase += twoPi;
            }

            float leftGain = 0.5f, rightGain = 0.5f;
            if (mPingPong && numChannels >= 2) {
                leftGain  = (1.0f - pan) * 0.5f;
                rightGain = (1.0f + pan) * 0.5f;
            }

            for (UInt32 ch = 0; ch < numChannels; ++ch) {
                auto& buffer = mDelayBuffer[ch];
                size_t& wIndex = mWriteIndex[ch];

                float in = inputBuffers[ch][i];
                float delayedOutSample = 0.0f;

                if (mZeno) {
                    const float zenoFeedback = 0.5f;
                    float gainFactor = 0.9f;
                    size_t delaySamples = baseDelaySamples;
                    
                    float echoSum = 0.0f;
                    while (delaySamples >= 1) {
                        size_t readIdx = (wIndex + mMaxDelaySamples - delaySamples) % mMaxDelaySamples;
                        echoSum += buffer[readIdx] * gainFactor;
                        gainFactor *= zenoFeedback;
                        delaySamples /= 2;
                    }

                    delayedOutSample = echoSum;

                    float feedbackIn = echoSum * zenoFeedback;
                    float writeVal = in + feedbackIn;

                    if (writeVal > 1.0f) writeVal = 1.0f;
                    else if (writeVal < -1.0f) writeVal = -1.0f;

                    buffer[wIndex] = writeVal;
                } else {
                    size_t readIdx = (wIndex + mMaxDelaySamples - baseDelaySamples) % mMaxDelaySamples;
                    delayedOutSample = buffer[readIdx];

                    buffer[wIndex] = in + (delayedOutSample * mFeedback);
                }

                float wetComponent = delayedOutSample * mWet;
                float outSample = in * mDry;

                if (mPingPong && numChannels >= 2)
                    outSample += wetComponent * ((ch == 0) ? leftGain : rightGain);
                else
                    outSample += wetComponent;

                outputBuffers[ch][i] = outSample;
                wIndex = (wIndex + 1) % mMaxDelaySamples;
            }
        }
    }
    
    void handleOneEvent(AUEventSampleTime now, AURenderEvent const *event) {
        switch (event->head.eventType) {
            case AURenderEventParameter: {
                handleParameterEvent(now, event->parameter);
                break;
            }
                
            default:
                break;
        }
    }
    
    void handleParameterEvent(AUEventSampleTime now, AUParameterEvent const& parameterEvent) {
        // Implement handling incoming Parameter events as needed
    }
    
    // MARK: Member Variables
    AUHostMusicalContextBlock mMusicalContextBlock;
    
    double mSampleRate = 44100.0;
    
    double mTimeInterval = 0.5;
    double mDry = 1;
    double mWet = 0.5;
    double mFeedback = 0.25;
    bool mZeno = false;
    bool mPingPong = false;
    float mFrequency = 0.25f;
    
    float mPhase = 0.0f;
    
    bool mBypassed = false;
    AUAudioFrameCount mMaxFramesToRender = 1024;
    
    size_t mMaxDelaySamples = 0;
    std::vector<std::vector<float>> mDelayBuffer;
    std::vector<size_t> mWriteIndex;
};
