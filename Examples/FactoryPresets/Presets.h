#pragma once

#include <stdarg.h>

//#include "IPlugParameter.h"

using namespace iplug;
using namespace igraphics;

const int kNumPrograms = 4;

void initParams(IEditorDelegate& plugin) {
    plugin.GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
    plugin.GetParam(kParamMode)->InitEnum("Mode", 0, 4, "Ciao", IParam::kFlagsNone, "Gr", "one", "two", "three", "four");
    plugin.GetParam(kParamFreq1)->InitDouble("Freq 1 - X", 0.5, 0., 2, 0.01, "Hz");
    plugin.GetParam(kParamFreq2)->InitDouble("Freq 2 - Y", 0.5, 0., 2, 0.01, "Hz");
}

void createPresets(Plugin& plugin) {
#ifdef APP_API

    

#else
        plugin.MakePreset("preset one", 100.0, 0, 0.5, 0.5);
        plugin.MakePreset("preset two", 80.0, 1, 1., 1.);
        plugin.MakePreset("preset three", 40.0, 2, 2., 2.);
        plugin.MakePreset("preset four", 20.0, 3, 3., 3.);
#endif
}
