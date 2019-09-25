#include "IPlugPresets.h"
#include "IPlug_include_in_plug_src.h"
#include "IPlugPaths.h"
#include "IconsForkAwesome.h"

IPlugPresets::IPlugPresets(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
    GetParam(kParamMode)->InitEnum("", 0, 4, "", IParam::kFlagsNone, "", "preset one", "preset two", "preset three", "preset four");
    GetParam(kParamMultislider)->InitDouble("", 0.5, 0., 1., 0.01, "");
    createPresets();
    
#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
    mMakeGraphicsFunc = [&]() {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
    };
    
    mLayoutFunc = [&](IGraphics* pGraphics) {
        if(pGraphics->NControls())
        {
            //Could handle new layout here
            return;
        }
        
        //    pGraphics->EnableLiveEdit(true);
        pGraphics->HandleMouseOver(true);
        pGraphics->AttachCornerResizer(EUIResizerMode::Scale, true);
        pGraphics->AttachPanelBackground(COLOR_GRAY);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        pGraphics->EnableTooltips(true);
        pGraphics->AttachTextEntryControl();
        pGraphics->AttachPopupMenuControl(DEFAULT_LABEL_TEXT);
        
        IRECT b = pGraphics->GetBounds().GetPadded(-5);
        
        const int nRows = 1;
        const int nCols = 5;
        
        int cellIdx = -1;
        
        auto nextCell = [&](){
            return b.GetGridCell(++cellIdx, nRows, nCols).GetPadded(-5.).GetMidVPadded(80.f);
        };
        
        auto sameCell = [&](){
            return b.GetGridCell(cellIdx, nRows, nCols).GetPadded(-5.);
        };
        
        pGraphics->AttachControl(new IVMultiSliderControl<16>(nextCell(), "MultiSlider", DEFAULT_STYLE, kParamMultislider, EDirection::Vertical, 0., 1.6), kNoTag, "vcontrols");
        pGraphics->AttachControl(new IVMeterControl<1>(nextCell().GetMidHPadded(20), "Meter"), kNoTag, "vcontrols");
        
        auto button1action = [pGraphics](IControl* pCaller){
            SplashClickActionFunc(pCaller);
//            int presetNumber = pGraphics->GetDelegate()->GetParam(kParamMode)->Int();
            dynamic_cast<IPluginBase*> (pGraphics->GetDelegate())->ModifyCurrentPreset();
        };
        
        pGraphics->AttachControl(new IVButtonControl(nextCell().FracRectVertical(0.5, true).GetCentredInside(80.), button1action, "Save Preset", DEFAULT_STYLE, false), kNoTag, "vcontrols");
        pGraphics->AttachControl(new CaptionPresets(sameCell().FracRectVertical(0.5, false).GetMidVPadded(10.f), kParamMode, IText(24.f, COLOR_BLACK), COLOR_LIGHT_GRAY, false), kCtrlTagCaption);
        
        IRECT wideCell;
#ifndef OS_WEB
        wideCell = nextCell().Union(nextCell());
        pGraphics->AttachControl(new ITextControl(wideCell.GetFromTop(20.f), "Preset Browser"));
        pGraphics->AttachControl(new FileBrowser(wideCell.GetReducedFromTop(20.f)), kCtrlTagBrowser);
#else
        nextCell();
        nextCell();
#endif
        loaded = true;
        RestorePreset(1);
    };
#endif
}

void IPlugPresets::createPresets() {

    MakePreset("preset one", 1/16., 2/16., 3/16., 4/16., 5/16., 6/16., 7/16., 8/16., 9/16., 10/16., 11/16., 12/16., 13/16., 14/16., 15/16., 1., 0);
    MakePreset("preset two", 15/16., 14/16., 13/16., 12/16., 11/16., 10/16., 9/16., 8/16., 7/16., 6/16., 5/16., 4/16., 3/16., 2/16., 1/16., 0., 1);
    MakePreset("preset three", 0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 2);
    MakePreset("preset four", 0., 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0., 3);

}

#if IPLUG_DSP

void IPlugPresets::OnIdle()
{
    mMeterSender.TransmitData(*this);
}

void IPlugPresets::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    for (int s = 0; s < nFrames; s++) {
        static double phase = 0.;
        phase += 0.0001;
        outputs[0][s] = sin(phase);
        outputs[1][s] = 0.;
    }
    mMeterSender.ProcessBlock(outputs, nFrames);
    
    for (int s = 0; s < nFrames; s++) {
        outputs[0][s] = 0.;
        outputs[1][s] = 0.;
    }
}

void IPlugPresets::OnParamChange(int paramIdx) {
    if (loaded) {
        switch (paramIdx) {
            case kParamMultislider:
                printf("MultiSlider\n");
                break;
                
            default:
                printf("paramChange\n");
                break;
        }
    }
}

#endif
