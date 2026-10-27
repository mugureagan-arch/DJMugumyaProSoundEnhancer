/**
 * @file DJMugumyaProSoundEnhancer.cpp
 * @brief Complete C++ Implementation of DJ Mugumya Pro Sound Enhancer for VirtualDJ 8
 * @author DJ Mugumya Audio Labs
 * @copyright (c) 2026 DJ Mugumya Audio Labs. All rights reserved.
 */

#include "DJMugumyaProSoundEnhancer.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif

extern "C" {

VDJ_EXPORT HRESULT VDJ_API DllGetClassObject(const GUID &rclsid, const GUID &riid, void** ppObject) {
    if (ppObject == nullptr) return E_POINTER;
    *ppObject = nullptr;

    if (memcmp(&rclsid, &CLSID_VdjPlugin8, sizeof(GUID)) == 0 &&
        memcmp(&riid, &IID_IVdjPluginDsp8, sizeof(GUID)) == 0) {
        *ppObject = static_cast<IVdjPluginDsp8*>(new CDJMugumyaProSoundEnhancer8());
        return NO_ERROR;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

} // extern "C"

CDJMugumyaProSoundEnhancer8::CDJMugumyaProSoundEnhancer8()
    : m_InputGainParam(0.5f),
      m_BassGainParam(0.5f),
      m_MidGainParam(0.5f),
      m_TrebleGainParam(0.5f),
      m_CompThresholdParam(0.6f),
      m_CompRatioParam(0.214f),
      m_LimiterCeilParam(0.9f),
      m_WetDryParam(1.0f),
      m_OutputGainParam(0.5f),
      m_BypassParam(false),
      m_ResetButton(0),
      m_CurrentSampleRate(44100.0f),
      m_NeedEqUpdate(true) {
}

CDJMugumyaProSoundEnhancer8::~CDJMugumyaProSoundEnhancer8() {}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnLoad() {
    m_CurrentSampleRate = (SampleRate > 0) ? static_cast<float>(SampleRate) : 44100.0f;

    m_Equalizer.Init(m_CurrentSampleRate);
    m_Compressor.Init(m_CurrentSampleRate);
    m_Limiter.Init(m_CurrentSampleRate);
    m_StereoMeter.Init(m_CurrentSampleRate);

    m_InputGainSmoother.Init(1.0f, 25.0f, m_CurrentSampleRate);
    m_OutputGainSmoother.Init(1.0f, 25.0f, m_CurrentSampleRate);
    m_WetDrySmoother.Init(1.0f, 25.0f, m_CurrentSampleRate);

    DeclareParameterSlider(&m_InputGainParam,     ID_INPUT_GAIN,      "Input Gain",      "InGain", 0.5f);
    DeclareParameterSlider(&m_BassGainParam,      ID_BASS_GAIN,       "Bass (100Hz)",    "Bass",   0.5f);
    DeclareParameterSlider(&m_MidGainParam,       ID_MID_GAIN,        "Mid Presence",    "Mid",    0.5f);
    DeclareParameterSlider(&m_TrebleGainParam,    ID_TREBLE_GAIN,     "Treble (8kHz)",   "Treble", 0.5f);
    DeclareParameterSlider(&m_CompThresholdParam, ID_COMP_THRESHOLD,  "Comp Threshold",  "Thresh", 0.6f);
    DeclareParameterSlider(&m_CompRatioParam,      ID_COMP_RATIO,      "Comp Ratio",      "Ratio",  0.214f);
    DeclareParameterSlider(&m_LimiterCeilParam,   ID_LIMITER_CEILING, "Limiter Ceiling", "Ceil",   0.9f);
    DeclareParameterSlider(&m_WetDryParam,        ID_WET_DRY,         "Wet/Dry Mix",     "Mix",    1.0f);
    DeclareParameterSlider(&m_OutputGainParam,    ID_OUTPUT_GAIN,     "Master Output",   "OutGain",0.5f);
    DeclareParameterSwitch(&m_BypassParam,        ID_BYPASS,          "Bypass",          "Bypass", false);
    DeclareParameterButton(&m_ResetButton,        ID_RESET,           "Reset Defaults",  "Reset");

    UpdateDspParameters();
    return S_OK;
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnGetPluginInfo(TVdjPluginInfo8 *infos) {
    if (!infos) return E_POINTER;
    infos->PluginName  = "DJ Mugumya Pro Sound Enhancer";
    infos->Author      = "DJ Mugumya Audio Labs";
    infos->Description = "Professional 3-Band EQ, Dynamics & Limiter (VirtualDJ 2021-2025 Compatible)";
    infos->Version     = "1.1.0";
    infos->Flags       = VDJFLAG_PROCESSAFTERSTOP;
    infos->Bitmap      = NULL;
    return S_OK;
}

ULONG VDJ_API CDJMugumyaProSoundEnhancer8::Release() {
    delete this;
    return 0;
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnGetUserInterface(TVdjPluginInterface8 *pluginInterface) {
    if (!pluginInterface) return E_POINTER;
    pluginInterface->Type = VDJINTERFACE_DEFAULT;
    return S_OK;
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnStart() {
    if (SampleRate > 0 && SampleRate != static_cast<int>(m_CurrentSampleRate)) {
        m_CurrentSampleRate = static_cast<float>(SampleRate);
        m_Equalizer.Init(m_CurrentSampleRate);
        m_Compressor.Init(m_CurrentSampleRate);
        m_Limiter.Init(m_CurrentSampleRate);
        m_StereoMeter.Init(m_CurrentSampleRate);
        m_InputGainSmoother.SetTime(25.0f, m_CurrentSampleRate);
        m_OutputGainSmoother.SetTime(25.0f, m_CurrentSampleRate);
        m_WetDrySmoother.SetTime(25.0f, m_CurrentSampleRate);
        m_NeedEqUpdate = true;
    }
    m_Equalizer.Reset();
    m_Compressor.Reset();
    m_Limiter.Reset();
    m_StereoMeter.Reset();
    UpdateDspParameters();
    return S_OK;
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnStop() {
    m_Compressor.Reset();
    m_Limiter.Reset();
    m_StereoMeter.Reset();
    return S_OK;
}

void CDJMugumyaProSoundEnhancer8::ResetToConservativeDefaults() {
    m_InputGainParam     = 0.5f;
    m_BassGainParam      = 0.5f;
    m_MidGainParam       = 0.5f;
    m_TrebleGainParam    = 0.5f;
    m_CompThresholdParam = 0.6f;
    m_CompRatioParam     = 0.214f;
    m_LimiterCeilParam   = 0.9f;
    m_WetDryParam        = 1.0f;
    m_OutputGainParam    = 0.5f;
    m_BypassParam        = false;
    m_NeedEqUpdate       = true;
    UpdateDspParameters();
}

void CDJMugumyaProSoundEnhancer8::UpdateDspParameters() {
    float inGainDb = (m_InputGainParam - 0.5f) * 24.0f;
    m_InputGainSmoother.SetTarget(MugumyaDSP::DbToLinear(inGainDb));

    float outGainDb = (m_OutputGainParam - 0.5f) * 24.0f;
    m_OutputGainSmoother.SetTarget(MugumyaDSP::DbToLinear(outGainDb));

    m_WetDrySmoother.SetTarget(std::clamp(m_WetDryParam, 0.0f, 1.0f));

    if (m_NeedEqUpdate) {
        float bassGainDb   = (m_BassGainParam - 0.5f) * 24.0f;
        float midGainDb    = (m_MidGainParam - 0.5f) * 24.0f;
        float trebleGainDb = (m_TrebleGainParam - 0.5f) * 24.0f;
        m_Equalizer.UpdateCoefficients(m_CurrentSampleRate,
                                       100.0f,  bassGainDb,
                                       1500.0f, midGainDb, 1.0f,
                                       8000.0f, trebleGainDb);
        m_NeedEqUpdate = false;
    }

    float thresholdDb = -30.0f + m_CompThresholdParam * 30.0f;
    float ratio = 1.0f + m_CompRatioParam * 7.0f;
    m_Compressor.SetParameters(thresholdDb, ratio, 15.0f, 120.0f, 0.0f);

    float ceilDb = -3.0f + m_LimiterCeilParam * 3.0f;
    m_Limiter.SetParameters(ceilDb, 50.0f);
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnParameter(int id) {
    switch (id) {
        case ID_RESET:
            ResetToConservativeDefaults();
            break;
        case ID_BASS_GAIN:
        case ID_MID_GAIN:
        case ID_TREBLE_GAIN:
            m_NeedEqUpdate = true;
            UpdateDspParameters();
            break;
        default:
            UpdateDspParameters();
            break;
    }
    return S_OK;
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnGetParameterString(int id, char *outParam, int outParamSize) {
    if (!outParam || outParamSize <= 0) return E_POINTER;
    switch (id) {
        case ID_INPUT_GAIN: {
            float db = (m_InputGainParam - 0.5f) * 24.0f;
            snprintf(outParam, outParamSize, "%+.1f dB", db);
            return S_OK;
        }
        case ID_BASS_GAIN: {
            float db = (m_BassGainParam - 0.5f) * 24.0f;
            snprintf(outParam, outParamSize, "%+.1f dB", db);
            return S_OK;
        }
        case ID_MID_GAIN: {
            float db = (m_MidGainParam - 0.5f) * 24.0f;
            snprintf(outParam, outParamSize, "%+.1f dB", db);
            return S_OK;
        }
        case ID_TREBLE_GAIN: {
            float db = (m_TrebleGainParam - 0.5f) * 24.0f;
            snprintf(outParam, outParamSize, "%+.1f dB", db);
            return S_OK;
        }
        case ID_COMP_THRESHOLD: {
            float db = -30.0f + m_CompThresholdParam * 30.0f;
            snprintf(outParam, outParamSize, "%.1f dB", db);
            return S_OK;
        }
        case ID_COMP_RATIO: {
            float ratio = 1.0f + m_CompRatioParam * 7.0f;
            snprintf(outParam, outParamSize, "%.1f:1", ratio);
            return S_OK;
        }
        case ID_LIMITER_CEILING: {
            float db = -3.0f + m_LimiterCeilParam * 3.0f;
            snprintf(outParam, outParamSize, "%.2f dBFS", db);
            return S_OK;
        }
        case ID_WET_DRY: {
            snprintf(outParam, outParamSize, "%d%% Wet", static_cast<int>(m_WetDryParam * 100.0f + 0.5f));
            return S_OK;
        }
        case ID_OUTPUT_GAIN: {
            float db = (m_OutputGainParam - 0.5f) * 24.0f;
            snprintf(outParam, outParamSize, "%+.1f dB", db);
            return S_OK;
        }
        case ID_BYPASS: {
            snprintf(outParam, outParamSize, "%s", m_BypassParam ? "BYPASSED" : "ACTIVE");
            return S_OK;
        }
        case ID_RESET: {
            snprintf(outParam, outParamSize, "Reset");
            return S_OK;
        }
        default:
            return E_NOTIMPL;
    }
}

HRESULT VDJ_API CDJMugumyaProSoundEnhancer8::OnProcessSamples(float *buffer, int nb) {
    if (!buffer || nb <= 0) return S_OK;

    if (m_BypassParam) {
        for (int i = 0; i < nb; ++i) {
            m_StereoMeter.ProcessSample(buffer[i * 2], buffer[i * 2 + 1]);
        }
        return S_OK;
    }

    for (int i = 0; i < nb; ++i) {
        float dryL = buffer[i * 2];
        float dryR = buffer[i * 2 + 1];

        float curInGain  = m_InputGainSmoother.Process();
        float curOutGain = m_OutputGainSmoother.Process();
        float curWetMix  = m_WetDrySmoother.Process();
        float curDryMix  = 1.0f - curWetMix;

        float wetL = dryL * curInGain;
        float wetR = dryR * curInGain;

        m_Equalizer.ProcessStereo(wetL, wetR);
        m_Compressor.ProcessStereo(wetL, wetR);
        m_Limiter.ProcessStereo(wetL, wetR);

        float outL = (wetL * curWetMix + dryL * curDryMix) * curOutGain;
        float outR = (wetR * curWetMix + dryR * curDryMix) * curOutGain;

        buffer[i * 2]     = outL;
        buffer[i * 2 + 1] = outR;

        m_StereoMeter.ProcessSample(outL, outR);
    }
    return S_OK;
}