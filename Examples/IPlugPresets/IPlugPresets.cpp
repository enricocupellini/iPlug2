#include "IPlugPresets.h"
#include "IPlug_include_in_plug_src.h"
#include "IPlugPaths.h"
#include "IconsForkAwesome.h"

IPlugPresets::IPlugPresets(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
    initParams(*this);
    createPresets(*this);
    
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
        pGraphics->AttachControl(new IVSliderControl(nextCell(), kParamSlider, "Slider"), kNoTag, "vcontrols");
        
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

#if IPLUG_DSP
void IPlugPresets::OnIdle()
{
    
}

void IPlugPresets::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
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
