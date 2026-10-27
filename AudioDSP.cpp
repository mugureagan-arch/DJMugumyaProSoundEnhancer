/**
 * @file AudioDSP.cpp
 * @brief Implementation of Audio DSP Algorithms for DJ Mugumya Pro Sound Enhancer
 * @author DJ Mugumya Audio Labs
 */

#include "AudioDSP.h"

namespace MugumyaDSP {

BiquadFilter::BiquadFilter() { Reset(); }

void BiquadFilter::Reset() {
    b0 = 1.0f; b1 = 0.0f; b2 = 0.0f;
    a1 = 0.0f; a2 = 0.0f;
    s1 = 0.0f; s2 = 0.0f;
}

void BiquadFilter::SetupLowShelf(float frequencyHz, float gainDb, float Q, float sampleRate) {
    if (sampleRate <= 0.0f) sampleRate = 44100.0f;
    frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.49f);
    Q = std::clamp(Q, 0.1f, 10.0f);

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * static_cast<float>(M_PI) * frequencyHz / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);
    float sqrtA = std::sqrt(A);
    float twoSqrtAAlpha = 2.0f * sqrtA * alpha;

    float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAAlpha;
    float invA0 = 1.0f / a0;

    b0 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 + twoSqrtAAlpha)) * invA0;
    b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0)) * invA0;
    b2 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAAlpha)) * invA0;
    a1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0)) * invA0;
    a2 = ((A + 1.0f) + (A - 1.0f) * cosw0 - twoSqrtAAlpha) * invA0;
}

void BiquadFilter::SetupPeaking(float frequencyHz, float gainDb, float Q, float sampleRate) {
    if (sampleRate <= 0.0f) sampleRate = 44100.0f;
    frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.49f);
    Q = std::clamp(Q, 0.1f, 10.0f);

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * static_cast<float>(M_PI) * frequencyHz / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);

    float a0 = 1.0f + alpha / A;
    float invA0 = 1.0f / a0;

    b0 = (1.0f + alpha * A) * invA0;
    b1 = (-2.0f * cosw0) * invA0;
    b2 = (1.0f - alpha * A) * invA0;
    a1 = (-2.0f * cosw0) * invA0;
    a2 = (1.0f - alpha / A) * invA0;
}

void BiquadFilter::SetupHighShelf(float frequencyHz, float gainDb, float Q, float sampleRate) {
    if (sampleRate <= 0.0f) sampleRate = 44100.0f;
    frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.49f);
    Q = std::clamp(Q, 0.1f, 10.0f);

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * static_cast<float>(M_PI) * frequencyHz / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);
    float sqrtA = std::sqrt(A);
    float twoSqrtAAlpha = 2.0f * sqrtA * alpha;

    float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + twoSqrtAAlpha;
    float invA0 = 1.0f / a0;

    b0 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAAlpha)) * invA0;
    b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0)) * invA0;
    b2 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAAlpha)) * invA0;
    a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0)) * invA0;
    a2 = ((A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAAlpha) * invA0;
}

Stereo3BandEQ::Stereo3BandEQ() : m_SampleRate(44100.0f) {}

void Stereo3BandEQ::Init(float sampleRate) {
    m_SampleRate = (sampleRate > 0.0f) ? sampleRate : 44100.0f;
    Reset();
}

void Stereo3BandEQ::Reset() {
    m_LowShelfL.Reset(); m_LowShelfR.Reset();
    m_MidPeakL.Reset();  m_MidPeakR.Reset();
    m_HighShelfL.Reset(); m_HighShelfR.Reset();
}

void Stereo3BandEQ::UpdateCoefficients(float sampleRate,
                                      float lowFreq, float lowGainDb,
                                      float midFreq, float midGainDb, float midQ,
                                      float highFreq, float highGainDb) {
    m_SampleRate = (sampleRate > 0.0f) ? sampleRate : 44100.0f;
    m_LowShelfL.SetupLowShelf(lowFreq, lowGainDb, 0.707f, m_SampleRate);
    m_LowShelfR.SetupLowShelf(lowFreq, lowGainDb, 0.707f, m_SampleRate);

    m_MidPeakL.SetupPeaking(midFreq, midGainDb, midQ, m_SampleRate);
    m_MidPeakR.SetupPeaking(midFreq, midGainDb, midQ, m_SampleRate);

    m_HighShelfL.SetupHighShelf(highFreq, highGainDb, 0.707f, m_SampleRate);
    m_HighShelfR.SetupHighShelf(highFreq, highGainDb, 0.707f, m_SampleRate);
}

TransparentCompressor::TransparentCompressor()
    : m_SampleRate(44100.0f), m_ThresholdDb(-12.0f), m_Ratio(2.5f),
      m_KneeWidthDb(4.0f), m_MakeupLinear(1.0f),
      m_AttackCoeff(0.001f), m_ReleaseCoeff(0.0001f),
      m_EnvelopeLevel(0.0f), m_CurrentGrDb(0.0f) {}

void TransparentCompressor::Init(float sampleRate) {
    m_SampleRate = (sampleRate > 0.0f) ? sampleRate : 44100.0f;
    Reset();
    SetParameters(m_ThresholdDb, m_Ratio, 15.0f, 120.0f, 0.0f);
}

void TransparentCompressor::Reset() {
    m_EnvelopeLevel = 0.0f;
    m_CurrentGrDb.store(0.0f, std::memory_order_relaxed);
}

void TransparentCompressor::SetParameters(float thresholdDb, float ratio, float attackMs, float releaseMs, float makeupGainDb) {
    m_ThresholdDb = std::clamp(thresholdDb, -40.0f, 0.0f);
    m_Ratio = std::clamp(ratio, 1.0f, 20.0f);
    m_MakeupLinear = DbToLinear(std::clamp(makeupGainDb, 0.0f, 18.0f));

    attackMs = std::max(0.5f, attackMs);
    releaseMs = std::max(5.0f, releaseMs);

    m_AttackCoeff = std::exp(-1.0f / (attackMs * 0.001f * m_SampleRate));
    m_ReleaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * m_SampleRate));
}

float TransparentCompressor::CalculateGainDb(float detectorDb) {
    float halfKnee = m_KneeWidthDb * 0.5f;
    float delta = detectorDb - m_ThresholdDb;

    if (delta <= -halfKnee) {
        return 0.0f;
    } else if (delta >= halfKnee) {
        return (detectorDb - m_ThresholdDb) * (1.0f / m_Ratio - 1.0f);
    } else {
        float x = delta + halfKnee;
        float factor = (1.0f / m_Ratio - 1.0f);
        return factor * (x * x) / (2.0f * m_KneeWidthDb);
    }
}

float TransparentCompressor::ProcessStereo(float &left, float &right) {
    float maxIn = std::max(std::fabs(left), std::fabs(right));
    maxIn = FlushDenormal(maxIn);

    if (maxIn > m_EnvelopeLevel) {
        m_EnvelopeLevel = m_AttackCoeff * m_EnvelopeLevel + (1.0f - m_AttackCoeff) * maxIn;
    } else {
        m_EnvelopeLevel = m_ReleaseCoeff * m_EnvelopeLevel + (1.0f - m_ReleaseCoeff) * maxIn;
    }
    m_EnvelopeLevel = FlushDenormal(m_EnvelopeLevel);

    float envDb = LinearToDb(m_EnvelopeLevel);
    float gainDb = CalculateGainDb(envDb);
    m_CurrentGrDb.store(gainDb, std::memory_order_relaxed);

    float gainLinear = DbToLinear(gainDb) * m_MakeupLinear;
    left *= gainLinear;
    right *= gainLinear;
    return gainDb;
}

TruePeakLimiter::TruePeakLimiter()
    : m_SampleRate(44100.0f), m_CeilingLinear(0.9659f),
      m_ReleaseCoeff(0.999f), m_Envelope(0.0f), m_Clipped(false) {}

void TruePeakLimiter::Init(float sampleRate) {
    m_SampleRate = (sampleRate > 0.0f) ? sampleRate : 44100.0f;
    Reset();
    SetParameters(-0.3f, 50.0f);
}

void TruePeakLimiter::Reset() {
    m_Envelope = 0.0f;
    m_Clipped.store(false, std::memory_order_relaxed);
}

void TruePeakLimiter::SetParameters(float ceilingDb, float releaseMs) {
    ceilingDb = std::clamp(ceilingDb, -6.0f, 0.0f);
    m_CeilingLinear = DbToLinear(ceilingDb);
    releaseMs = std::max(5.0f, releaseMs);
    m_ReleaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * m_SampleRate));
}

void TruePeakLimiter::ProcessStereo(float &left, float &right) {
    float peak = std::max(std::fabs(left), std::fabs(right));
    if (peak > m_CeilingLinear) {
        m_Clipped.store(true, std::memory_order_relaxed);
    }

    if (peak > m_Envelope) {
        m_Envelope = peak;
    } else {
        m_Envelope = m_ReleaseCoeff * m_Envelope + (1.0f - m_ReleaseCoeff) * peak;
    }
    m_Envelope = FlushDenormal(m_Envelope);

    if (m_Envelope > m_CeilingLinear) {
        float limiterGain = m_CeilingLinear / m_Envelope;
        left *= limiterGain;
        right *= limiterGain;
    }

    left  = SoftSaturate(left, m_CeilingLinear);
    right = SoftSaturate(right, m_CeilingLinear);
}

StereoMeter::StereoMeter()
    : m_SampleRate(44100.0f), m_PeakDecay(0.9995f), m_RmsFilter(0.001f),
      m_CurPeakL(0.0f), m_CurPeakR(0.0f), m_CurRmsSqL(0.0f), m_CurRmsSqR(0.0f),
      m_LeftPeakDb(-100.0f), m_RightPeakDb(-100.0f), m_LeftRmsDb(-100.0f), m_RightRmsDb(-100.0f) {}

void StereoMeter::Init(float sampleRate) {
    m_SampleRate = (sampleRate > 0.0f) ? sampleRate : 44100.0f;
    m_PeakDecay = std::exp(-1.0f / (0.3f * m_SampleRate));
    m_RmsFilter = 1.0f - std::exp(-1.0f / (0.15f * m_SampleRate));
    Reset();
}

void StereoMeter::Reset() {
    m_CurPeakL = 0.0f; m_CurPeakR = 0.0f;
    m_CurRmsSqL = 0.0f; m_CurRmsSqR = 0.0f;
    m_LeftPeakDb.store(-100.0f, std::memory_order_relaxed);
    m_RightPeakDb.store(-100.0f, std::memory_order_relaxed);
    m_LeftRmsDb.store(-100.0f, std::memory_order_relaxed);
    m_RightRmsDb.store(-100.0f, std::memory_order_relaxed);
}

void StereoMeter::ProcessSample(float left, float right) {
    float absL = std::fabs(left);
    float absR = std::fabs(right);

    m_CurPeakL = std::max(absL, m_CurPeakL * m_PeakDecay);
    m_CurPeakR = std::max(absR, m_CurPeakR * m_PeakDecay);

    m_CurRmsSqL += m_RmsFilter * (left * left - m_CurRmsSqL);
    m_CurRmsSqR += m_RmsFilter * (right * right - m_CurRmsSqR);

    m_CurPeakL = FlushDenormal(m_CurPeakL);
    m_CurPeakR = FlushDenormal(m_CurPeakR);
    m_CurRmsSqL = FlushDenormal(m_CurRmsSqL);
    m_CurRmsSqR = FlushDenormal(m_CurRmsSqR);

    m_LeftPeakDb.store(LinearToDb(m_CurPeakL), std::memory_order_relaxed);
    m_RightPeakDb.store(LinearToDb(m_CurPeakR), std::memory_order_relaxed);
    m_LeftRmsDb.store(LinearToDb(std::sqrt(std::max(0.0f, m_CurRmsSqL))), std::memory_order_relaxed);
    m_RightRmsDb.store(LinearToDb(std::sqrt(std::max(0.0f, m_CurRmsSqR))), std::memory_order_relaxed);
}

} // namespace MugumyaDSP