/**
 * @file DJMugumyaProSoundEnhancer.h
 * @brief DJ Mugumya Pro Sound Enhancer - VirtualDJ 8 Audio DSP Plugin Class
 * @author DJ Mugumya Audio Labs
 * @copyright (c) 2026 DJ Mugumya Audio Labs. All rights reserved.
 */

#ifndef DJMUGUMYAPROSOUNDENHANCER_H
#define DJMUGUMYAPROSOUNDENHANCER_H

#include "vdjDsp8.h"
#include "AudioDSP.h"

enum ParameterIDs {
    ID_INPUT_GAIN = 1,
    ID_BASS_GAIN,
    ID_MID_GAIN,
    ID_TREBLE_GAIN,
    ID_COMP_THRESHOLD,
    ID_COMP_RATIO,
    ID_LIMITER_CEILING,
    ID_WET_DRY,
    ID_OUTPUT_GAIN,
    ID_BYPASS,
    ID_RESET,
    PARAM_COUNT
};

class CDJMugumyaProSoundEnhancer8 : public IVdjPluginDsp8 {
public:
    CDJMugumyaProSoundEnhancer8();
    virtual ~CDJMugumyaProSoundEnhancer8();

    HRESULT VDJ_API OnLoad() override;
    HRESULT VDJ_API OnGetPluginInfo(TVdjPluginInfo8 *infos) override;
    ULONG   VDJ_API Release() override;
    HRESULT VDJ_API OnGetUserInterface(TVdjPluginInterface8 *pluginInterface) override;
    HRESULT VDJ_API OnParameter(int id) override;
    HRESULT VDJ_API OnGetParameterString(int id, char *outParam, int outParamSize) override;

    HRESULT VDJ_API OnStart() override;
    HRESULT VDJ_API OnStop() override;
    HRESULT VDJ_API OnProcessSamples(float *buffer, int nb) override;

    float GetLeftMeterDb() const  { return m_StereoMeter.GetLeftPeakDb(); }
    float GetRightMeterDb() const { return m_StereoMeter.GetRightPeakDb(); }
    float GetGainReductionDb() const { return m_Compressor.GetCurrentGainReductionDb(); }
    bool  HasClipped() const      { return m_Limiter.HasClipped(); }

private:
    float m_InputGainParam;
    float m_BassGainParam;
    float m_MidGainParam;
    float m_TrebleGainParam;
    float m_CompThresholdParam;
    float m_CompRatioParam;
    float m_LimiterCeilParam;
    float m_WetDryParam;
    float m_OutputGainParam;
    bool  m_BypassParam;
    int   m_ResetButton;

    MugumyaDSP::Stereo3BandEQ         m_Equalizer;
    MugumyaDSP::TransparentCompressor m_Compressor;
    MugumyaDSP::TruePeakLimiter       m_Limiter;
    MugumyaDSP::StereoMeter           m_StereoMeter;

    MugumyaDSP::ParameterSmoother m_InputGainSmoother;
    MugumyaDSP::ParameterSmoother m_OutputGainSmoother;
    MugumyaDSP::ParameterSmoother m_WetDrySmoother;

    float m_CurrentSampleRate;
    bool  m_NeedEqUpdate;

    void UpdateDspParameters();
    void ResetToConservativeDefaults();
};

#endif // DJMUGUMYAPROSOUNDENHANCER_H
