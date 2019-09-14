#include "IPlugChunks.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

const int kDummyParamForMultislider = -2;

class ITempPresetSaveButtonControl : public IPanelControl
{
    Plugin* mPlug = nullptr;
public:
    ITempPresetSaveButtonControl(Plugin *pPlug, IRECT pR)
    : IPanelControl(pR, COLOR_RED) {mPlug = pPlug;}
    
    void OnMouseDown(int x, int y, IMouseMod* pMod)
    {
        WDL_String presetFilePath;
        DesktopPath(presetFilePath);
#ifdef OS_WIN
        presetFilePath.Append("\\IPlugChunksPreset.txt");
#elif defined OS_MAC
        presetFilePath.Append("IPlugChunksPreset.txt");
#endif
        mPlug->DumpPresetBlob(presetFilePath.Get());
    }
    
};

class PresetFunctionsMenu : public IPanelControl
{
private:
    Plugin* mPlug = nullptr;
    WDL_String mPreviousPath;
    
public:
    PresetFunctionsMenu(Plugin *pPlug, IRECT pR)
    : IPanelControl(pR, COLOR_BLUE)
    {mPlug = pPlug;}
    
    bool Draw(IGraphics* pGraphics)
    {
        pGraphics->FillRect(COLOR_WHITE, mRECT);
        
        int ax = mRECT.R - 8;
        int ay = mRECT.T + 4;
        int bx = ax + 4;
        int by = ay;
        int cx = ax + 2;
        int cy = ay + 2;
        IBlend blend{};
        pGraphics->FillTriangle(COLOR_GRAY, ax , ay, bx, by, cx, cy, &blend);
        
        return true;
    }
    
    void OnMouseDown(int x, int y, IMouseMod* pMod)
    {
        if (pMod->L)
        {
            doPopupMenu();
        }
        
//        Redraw(); // seems to need this
        SetDirty();
    }
    
    void doPopupMenu()
    {
        IPopupMenu menu;
        
        IGraphics* gui = mPlug->GetUI();
        
        menu.AddItem("Save Program...");
        menu.AddItem("Save Bank...");
        menu.AddSeparator();
        menu.AddItem("Load Program...");
        menu.AddItem("Load Bank...");
        gui->CreatePopupMenu(*this, menu, mRECT, 0);
        int itemChosen = menu.GetChosenItemIdx();
        WDL_String fileName;
        
        //printf("chosen %i /n", itemChosen);
        switch (itemChosen)
        {
            case 0: //Save Program
                fileName.Set(mPlug->GetPresetName(mPlug->GetCurrentPresetIdx()));
                GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Save, "fxp");
                mPlug->SaveProgramAsFXP(fileName.Get());
                break;
            case 1: //Save Bank
                fileName.Set("IPlugChunksBank");
                GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Save, "fxb");
                mPlug->SaveBankAsFXB(fileName.Get());
                break;
            case 3: //Load Preset
                GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Open, "fxp");
                mPlug->LoadProgramFromFXP(fileName.Get());
                break;
            case 4: // Load Bank
                GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Open, "fxb");
                mPlug->LoadBankFromFXB(fileName.Get());
                break;
            default:
                break;
        }
    }
};

IPlugChunks::IPlugChunks(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms)), mGain(1.)
{
    TRACE;
    
    memset(mSteps, 0, NUM_SLIDERS*sizeof(double));
    
    // Define parameter ranges, display units, labels.
    //arguments are: name, defaultVal, minVal, maxVal, step, label
    
  GetParam(kGain)->InitDouble("Gain", 0.0, -70.0, 12.0, 0.1, "dB");

    MakePresetFromBlob("Ramp Up", "AAAAAJqZqT8AAAAAmpm5PwAAAIA9Csc/AAAAAAAA0D8AAABA4XrUPwAAAIDC9dg/AAAAwMzM3D8AAAAQ16PgPwAAALBH4eI/AAAA0MzM5D8AAADwUbjmPwAAAAjXo+g/AAAAKFyP6j8AAADMzMzsPwAAAOxRuO4/AAAAAAAA8D8AAAAAAAC8Pg==", 136);
    MakePresetFromBlob("Ramp Down", "AAAA7FG47j8AAABI4XrsPwAAALBH4eo/AAAAGK5H6T8AAABwPQrnPwAAANDMzOQ/AAAAwB6F4z8AAAAghevhPwAAAAB7FN4/AAAAgOtR2D8AAABAuB7VPwAAAACuR9E/AAAAgEfhyj8AAAAAhevBPwAAAABSuK4/AAAAAOB6hD8AAAAAAAC8Pg==", 136);
    MakePresetFromBlob("Triangle", "AAAAAIXrwT8AAACAR+HKPwAAAEBcj9I/AAAAgBSu1z8AAADA9SjcPwAAABDXo+A/AAAAsEfh4j8AAABQuB7lPwAAAGBmZuY/AAAAMDMz4z8AAAAAAADgPwAAAMD1KNw/AAAAQI/C1T8AAAAArkfRPwAAAICPwsU/AAAAAJqZuT8AAAAAAAAAAA==", 136);
    MakePresetFromBlob("Inv Triangle", "AAAAAAAA8D8AAABQuB7tPwAAAKBwPeo/AAAAcD0K5z8AAABA4XrkPwAAAJDC9eA/AAAAwEfh2j8AAABAj8LVPwAAAECPwtU/AAAAwMzM3D8AAAAghevhPwAAANDMzOQ/AAAAgBSu5z8AAACYmZnpPwAAAFyPwu0/AAAAAAAA8D8AAAAAAAAAAA==", 136);
    MakePresetFromBlob("Da Endz", "AAAAAAAA8D8AAAAA4HqEPwAAAADgeoQ/AAAAAOB6hD8AAAAA4HqEPwAAAADgeoQ/AAAAAOB6hD8AAAAA4HqEPwAAAADgeoQ/AAAAAOB6hD8AAAAA4HqEPwAAAADgeoQ/AAAAAOB6hD8AAAAA4HqEPwAAAADgeoQ/AAAAAAAA8D8AAAAAAAAAAA==", 136);
    MakePresetFromBlob("Alternate", "AAAAAAAA8D8AAAAA4HqEPwAAAAAAAPA/AAAAAOB6hD8AAAAAAADwPwAAAADgeoQ/AAAAAAAA8D8AAAAA4HqEPwAAAAAAAPA/AAAAAOB6hD8AAAAAAADwPwAAAADgeoQ/AAAAAAAA8D8AAAAA4HqEPwAAAAAAAPA/AAAAAOB6hD8AAAAAAAAAAA==", 136);
    MakePresetFromBlob("Alt Ramp Down", "AAAAAAAA8D8AAAAA4HqEPwAAALgehes/AAAAAOB6hD8AAACI61HoPwAAAADgeoQ/AAAAQArX4z8AAAAA4HqEPwAAAAAAAOA/AAAAAOB6hD8AAABAuB7VPwAAAADgeoQ/AAAAAKRwzT8AAAAA4HqEPwAAAAAzM8M/AAAAAOB6hD8AAAAAAAAAAA==", 136);
    MakePresetFromBlob("Alt Ramp Up", "AAAAgJmZyT8AAAAA4HqEPwAAAIBmZtY/AAAAAOB6hD8AAAAAKVzfPwAAAADgeoQ/AAAAMFyP4j8AAAAA4HqEPwAAAEDheuQ/AAAAAOB6hD8AAADwKFznPwAAAADgeoQ/AAAAIIXr6T8AAAAA4HqEPwAAANijcO0/AAAAAOB6hD8AAAAAAAAAAA==", 136);
    
#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
      
//      mMSlider = new MultiSliderControlV(this, IRECT(10, 10, 170, 110), kDummyParamForMultislider, NUM_SLIDERS, 10, &COLOR_WHITE, &COLOR_BLACK, &COLOR_RED);
      
//      pGraphics->AttachControl(mMSlider);
//      pGraphics->AttachControl(new IVSliderControl(IRECT(200, 10, 220, 110), kGain));
      
      //pGraphics->AttachControl(new ITempPresetSaveButtonControl(this, IRECT(350, 250, 390, 290)));
      pGraphics->AttachControl(new PresetFunctionsMenu(this, IRECT(350, 250, 390, 290)));
      
      // call RestorePreset(0) here which will initialize the multislider in the gui and the mSteps array
      RestorePreset(0);
      
  };
#endif
}

#if IPLUG_DSP
void IPlugChunks::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    double* in1 = inputs[0];
    double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];
    
    int samplesPerBeat = (int) GetSamplesPerBeat();
    int samplePos = (int) GetSamplePos();
    
    int count = mCount;
    int prevcount = mPrevCount;
    
    for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
    {
        int mod = (samplePos + s) % (samplesPerBeat * BEAT_DIV);
        
        count = mod / (samplesPerBeat / BEAT_DIV);
        
        if (count >= NUM_SLIDERS)
        {
            count = 0;
        }
        
        if (count != prevcount)
        {
            if (GetUI())
            {
                mMSlider->SetHighlight(count);
            }
        }
        
        prevcount = count;
        
        *out1 = *in1 * mSteps[count] * mGain;
        *out2 = *in2 * mSteps[count] * mGain;
    }
    
    mCount = count;
    mPrevCount = prevcount;
}

void IPlugChunks::Reset()
{
    TRACE;
    
    mCount = 0;
    mPrevCount = ULONG_MAX;
}

void IPlugChunks::OnParamChange(int paramIdx)
{
    TRACE;
    
    switch (paramIdx)
    {
        case kDummyParamForMultislider:
            mMSlider->GetLatestChange(mSteps);
            break;
        case kGain:
            mGain = GetParam(kGain)->DBToAmp();
            break;
        default:
            break;
    }
}

// this over-ridden method is called when the host is trying to store the plug-in state and needs to get the current data from your algorithm
bool IPlugChunks::SerializeState(IByteChunk* pChunk)
{
    TRACE;

    double v;
    
    // serialize the multi-slider state state before serializing the regular params
    for (int i = 0; i< NUM_SLIDERS; i++)
    {
        v = mSteps[i];
        pChunk->Put(&v);
    }
    
    return IPluginBase::SerializeParams(*pChunk); // must remember to call SerializeParams at the end
}

// this over-ridden method is called when the host is trying to load the plug-in state and you need to unpack the data into your algorithm
int IPlugChunks::UnserializeState(IByteChunk* pChunk, int startPos)
{
    TRACE;

    double v = 0.0;
    
    // unserialize the multi-slider state before unserializing the regular params
    for (int i = 0; i< NUM_SLIDERS; i++)
    {
        v = 0.0;
        startPos = pChunk->Get(&v, startPos);
        mSteps[i] = v;
    }
    
    // update values in control, will set dirty
    if(mMSlider)
        mMSlider->SetState(mSteps);
    
    return IPluginBase::UnserializeParams(*pChunk, startPos); // must remember to call UnserializeParams at the end
}

bool IPlugChunks::CompareState(const unsigned char* incomingState, int startPos)
{
    bool isEqual = true;
    const double* data = (const double*) incomingState;
    startPos = NUM_SLIDERS * sizeof(double);
    isEqual = (memcmp(data, mSteps, startPos) == 0);
    isEqual &= CompareState(incomingState, startPos); // fuzzy compare regular params
    
    return isEqual;
}

void IPlugChunks::PresetsChangedByHost()
{
    TRACE;
    
    if(mMSlider)
        mMSlider->SetState(mSteps);
    
    if(GetUI())
        GetUI()->SetAllControlsDirty();
}
#endif
