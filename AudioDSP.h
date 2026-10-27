/**
 * @file AudioDSP.h
 * @brief High-Performance Real-Time Audio DSP Algorithms
 * @author DJ Mugumya Audio Labs
 */

#ifndef AUDIODSP_H
#define AUDIODSP_H

#include <cmath>
#include <atomic>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace MugumyaDSP {

inline float FlushDenormal(float val) {
    if (std::fabs(val) < 1.0e-15f) return 0.0f;
    return val;
}

inline float DbToLinear(float db) {
    return std::pow(10.0f, db * 0.05f);
}

inline float LinearToDb(float lin) {
    if (lin <= 1.0e-5f) return -100.0f;
    return 20.0f * std::log10(lin);
}

class ParameterSmoother {
public:
    ParameterSmoother() : m_Current(0.0f), m_Target(0.0f), m_Coeff(0.005f) {}

    void Init(float initialValue, float timeMs, float sampleRate) {
        m_Current = initialValue;
        m_Target = initialValue;
        SetTime(timeMs, sampleRate);
    }

    void SetTime(float timeMs, float sampleRate) {
        if (sampleRate <= 0.0f) sampleRate = 44100.0f;
        if (timeMs <= 0.0f) {
            m_Coeff = 1.0f;
        } else {
            float tau = (timeMs * 0.001f);
            m_Coeff = 1.0f - std::exp(-1.0f / (tau * sampleRate));
        }
    }

    void SetTarget(float target) { m_Target = target; }
    float GetTarget() const { return m_Target; }
    float GetCurrent() const { return m_Current; }

    inline float Process() {
        m_Current += m_Coeff * (m_Target - m_Current);
        m_Current = FlushDenormal(m_Current);
        return m_Current;
    }

    void SnapToTarget() { m_Current = m_Target; }

private:
    float m_Current;
    float m_Target;
    float m_Coeff;
};

class BiquadFilter {
public:
    BiquadFilter();
    void Reset();
    void SetupLowShelf(float frequencyHz, float gainDb, float Q, float sampleRate);
    void SetupPeaking(float frequencyHz, float gainDb, float Q, float sampleRate);
    void SetupHighShelf(float frequencyHz, float gainDb, float Q, float sampleRate);

    inline float Process(float input) {
        float out = b0 * input + s1;
        s1 = b1 * input - a1 * out + s2;
        s2 = b2 * input - a2 * out;
        s1 = FlushDenormal(s1);
        s2 = FlushDenormal(s2);
        return out;
    }

private:
    float b0, b1, b2, a1, a2;
    float s1, s2;
};

class Stereo3BandEQ {
public:
    Stereo3BandEQ();
    void Init(float sampleRate);
    void Reset();
    void UpdateCoefficients(float sampleRate,
                            float lowFreq, float lowGainDb,
                            float midFreq, float midGainDb, float midQ,
                            float highFreq, float highGainDb);

    inline void ProcessStereo(float &left, float &right) {
        left = m_HighShelfL.Process(m_MidPeakL.Process(m_LowShelfL.Process(left)));
        right = m_HighShelfR.Process(m_MidPeakR.Process(m_LowShelfR.Process(right)));
    }

private:
    BiquadFilter m_LowShelfL, m_LowShelfR;
    BiquadFilter m_MidPeakL,  m_MidPeakR;
    BiquadFilter m_HighShelfL, m_HighShelfR;
    float m_SampleRate;
};

class TransparentCompressor {
public:
    TransparentCompressor();
    void Init(float sampleRate);
    void Reset();
    void SetParameters(float thresholdDb, float ratio, float attackMs, float releaseMs, float makeupGainDb);
    float ProcessStereo(float &left, float &right);

    float GetCurrentGainReductionDb() const {
        return m_CurrentGrDb.load(std::memory_order_relaxed);
    }

private:
    float m_SampleRate;
    float m_ThresholdDb;
    float m_Ratio;
    float m_KneeWidthDb;
    float m_MakeupLinear;
    float m_AttackCoeff;
    float m_ReleaseCoeff;
    float m_EnvelopeLevel;
    std::atomic<float> m_CurrentGrDb;

    float CalculateGainDb(float detectorDb);
};

class TruePeakLimiter {
public:
    TruePeakLimiter();
    void Init(float sampleRate);
    void Reset();
    void SetParameters(float ceilingDb, float releaseMs);
    void ProcessStereo(float &left, float &right);

    bool HasClipped() const { return m_Clipped.load(std::memory_order_relaxed); }
    void ClearClipFlag() { m_Clipped.store(false, std::memory_order_relaxed); }

private:
    float m_SampleRate;
    float m_CeilingLinear;
    float m_ReleaseCoeff;
    float m_Envelope;
    std::atomic<bool> m_Clipped;

    inline float SoftSaturate(float x, float ceiling) {
        if (x > ceiling) {
            float excess = x - ceiling;
            return ceiling + (excess / (1.0f + excess));
        } else if (x < -ceiling) {
            float excess = -x - ceiling;
            return -(ceiling + (excess / (1.0f + excess)));
        }
        return x;
    }
};

class StereoMeter {
public:
    StereoMeter();
    void Init(float sampleRate);
    void Reset();
    void ProcessSample(float left, float right);

    float GetLeftPeakDb() const { return m_LeftPeakDb.load(std::memory_order_relaxed); }
    float GetRightPeakDb() const { return m_RightPeakDb.load(std::memory_order_relaxed); }
    float GetLeftRmsDb() const { return m_LeftRmsDb.load(std::memory_order_relaxed); }
    float GetRightRmsDb() const { return m_RightRmsDb.load(std::memory_order_relaxed); }

private:
    float m_SampleRate;
    float m_PeakDecay;
    float m_RmsFilter;
    float m_CurPeakL, m_CurPeakR;
    float m_CurRmsSqL, m_CurRmsSqR;
    std::atomic<float> m_LeftPeakDb;
    std::atomic<float> m_RightPeakDb;
    std::atomic<float> m_LeftRmsDb;
    std::atomic<float> m_RightRmsDb;
};

} // namespace MugumyaDSP

#endif // AUDIODSP_H