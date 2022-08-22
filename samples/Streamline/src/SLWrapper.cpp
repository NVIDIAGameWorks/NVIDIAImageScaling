// The MIT License(MIT)
//
// Copyright(c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <SLWrapper.h>
#include <assert.h>
#include <stdexcept>
#include <iostream>
#include <d3d12.h>

bool SLWrapper::m_sl_initialized = false;
bool SLWrapper::m_nis_available = false;

SLWrapper::SLWrapper()
{
    if (!m_sl_initialized)
    {
        std::cout << "Must initialise Streamline before creating the wrapper" << std::endl;
        return;
    }
    m_nis_available = CheckSupportNIS();
    m_nis_consts = {};
}

void SLWrapper::LogFunctionCallback(sl::LogType type, const char* msg) {
    if (type == sl::LogType::eLogTypeError) {
        // Add a breakpoint here to break on errors
        printf("Error: %s", msg);
    }
    if (type == sl::LogType::eLogTypeWarn) {
        // Add a breakpoint here to break on warnings
        printf("Warning: %s", msg);
    }
    else {
        printf("other: %s", msg);
    }
}

void SLWrapper::Initialize()
{
    sl::Preferences pref;
    pref.showConsole = true;
#if _DEBUG
    pref.logMessageCallback = &LogFunctionCallback;
    pref.logLevel = sl::LogLevel::eLogLevelDefault;
#else
    pref.logLevel = sl::LogLevel::eLogLevelOff;
#endif
    sl::Feature features[] = { sl::Feature::eFeatureNIS };
    pref.featuresToLoad = features;
    pref.numFeaturesToLoad = _countof(features);
    m_sl_initialized = slInit(pref);
    if (!m_sl_initialized)
    {
        std::cout << "Failed to initialze Streamline" << std::endl;
    }
    std::cout << "Streamline Initialized" << std::endl;
}

void SLWrapper::Shutdown()
{
    if (m_sl_initialized)
    {
        slShutdown();
        m_sl_initialized = false;
    }
}

void SLWrapper::SetNISConsts(const sl::NISConstants& consts, int frameNumber)
{
    m_nis_consts = consts;
    if (!slSetFeatureConstants(sl::eFeatureNIS, &m_nis_consts, frameNumber))
    {
        std::cout << "Failed to set NIS constants" << std::endl;
    }
}

bool SLWrapper::CheckSupportNIS() {
    if (!m_sl_initialized)
    {
        std::cout << "Streamline not initialized" << std::endl;
        return false;
    }
    bool support = slIsFeatureSupported(sl::eFeatureNIS);
    if (!support)
    {
        std::cout << "NIS is not supported on this system" << std::endl;
    }
    std::cout << "NIS is supported on this system" << std::endl;
    return support;
}

void SLWrapper::EvaluateNIS(ID3D12GraphicsCommandList* context, ID3D12Resource* unresolvedColor, ID3D12Resource* resolvedColor)
{
    bool success = true;
    // change extents to use subrects
    sl::Extent extentIn = { 0, 0, 0, 0 };
    sl::Extent extentOut = { 0, 0, 0, 0 };
    {
        sl::Resource unresolvedColorResource;
        unresolvedColorResource.native = unresolvedColor;
        sl::Resource resolvedColorResource;
        resolvedColorResource.native = resolvedColor;
        success = success && slSetTag(&unresolvedColorResource, sl::eBufferTypeScalingInputColor, 0, &extentIn);
        success = success && slSetTag(&resolvedColorResource, sl::eBufferTypeScalingOutputColor, 0, &extentOut);
    }
    if (!success)
    {
        std::cout << "Failed to tag NIS resources" << std::endl;
        return;
    }
    if (!slSetFeatureConstants(sl::Feature::eFeatureNIS, &m_nis_consts, 0))
    {
        std::cout << "Failed to set NIS constants" << std::endl;
        return;
    }
    if (!slEvaluateFeature(context, sl::Feature::eFeatureNIS, 0))
    {
        std::cout << "Failed NIS evaluation" << std::endl;
    }
}