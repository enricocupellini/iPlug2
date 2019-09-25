#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
//#include "IGraphicsConstants.h"
enum EParams
{
    kParamMultislider,
    kParamMultislider2,
    kParamMultislider3,
    kParamMultislider4,
    kParamMultislider5,
    kParamMultislider6,
    kParamMultislider7,
    kParamMultislider8,
    kParamMultislider9,
    kParamMultislider10,
    kParamMultislider11,
    kParamMultislider12,
    kParamMultislider13,
    kParamMultislider14,
    kParamMultislider15,
    kParamMultislider16,
    kParamMode,
    kNumParams
};

enum ECtrlTags
{
    kCtrlTagCaption,
    kCtrlTagBrowser,
    kCtrlTagMeter,
    kCtrlTags
};

using namespace iplug;
using namespace igraphics;

const int kNumPrograms = 4;

class FileBrowser : public IDirBrowseControlBase
{
private:
    WDL_String mLabel;
    WDL_String mPreviousPath;
    WDL_String path;

public:
    FileBrowser(const IRECT& bounds)
    : IDirBrowseControlBase(bounds, ".fxb")
    {
        //    DesktopPath(path);
        path.Set(__FILE__);
        path.remove_filepart();
#ifdef OS_WIN
        path.Append("\\resources\\presets\\");
#else
        path.Append("/resources/presets/");
#endif
        AddPath(path.Get(), "");
        
        mLabel.Set("Click here to browse preset banks...");
        mPreviousPath = path;
    }
    
    void Draw(IGraphics& g) override
    {
        g.FillRect(COLOR_TRANSLUCENT, mRECT);
        
        IRECT labelRect = mRECT.GetFromBottom(mText.mSize);
        IRECT bmpRect = mRECT.GetReducedFromBottom(mText.mSize);
        WDL_String fileName;
        IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate()); fileName.Set(mPlug->GetPresetName(mPlug->GetCurrentPresetIdx()));
        
        g.DrawText(IText(24.f, COLOR_BLACK), fileName.Get(), bmpRect);
        
        g.FillRect(COLOR_WHITE, labelRect);
        g.DrawText(mText, mLabel.Get(), labelRect);
    }
    
    void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
        SetUpMenu();

        mMainMenu.AddItem("Save Preset Bank", 0);
        mMainMenu.AddItem("Select Preset Bank Folder", 1);
        mMainMenu.AddSeparator(2);
        GetUI()->CreatePopupMenu(*this, mMainMenu, x, y);
    }
    
    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx) override
    {
        if(pSelectedMenu)
        {
            IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate());
            if(pSelectedMenu == &mMainMenu && pSelectedMenu->GetChosenItemIdx() < 2)
            {
                switch (pSelectedMenu->GetChosenItemIdx()) {
                    case 0:     // SAVE preset bank
                    {
                        WDL_String fileName;
                        if (strcmp(mLabel.Get(), "Click here to browse preset banks...") == 0)  // match
                        {
                            fileName.Set("Bank ...");
                        }
                        else{
                            fileName.Set(mLabel.Get());
                        }
//                         fileName.Set(mPlug->GetPresetName(mPlug->GetCurrentPresetIdx()));
                        GetUI()->PromptForFile(fileName, mPreviousPath, EFileAction::Save, "fxb");
//                        mPlug->SaveProgramAsFXP(fileName.Get());
                        if (strcmp(fileName.Get(), "") != 0)  // not match
                        {
                            mPlug->ModifyCurrentPreset();
                            mPlug->SaveBankAsFXB(fileName.Get());
                            mLabel.Set(fileName.get_filepart());
                            SetDirty(false);
                        }
                        break;
                    }
                    case 1:
                    {
                        GetUI()->PromptForDirectory(mPreviousPath);
                        if (mPreviousPath.GetLength()>0) {
                            mPaths.Empty(true);
                            AddPath(mPreviousPath.Get(), "");
                        }
                        SetDirty(false);
                        break;
                    }
                    default:
                        break;
                }
            }
            else{
                IPopupMenu::Item* pItem = pSelectedMenu->GetChosenItem();
                WDL_String* pStr = mFiles.Get(pItem->GetTag());
                mPlug->LoadBankFromFXB(pStr->Get());
                pStr->remove_fileext();
                mLabel.Set(pStr->get_filepart());
//                mPlug->LoadProgramFromFXP(pStr->Get());
                SetDirty(false);
            }
        }
    }
};

class CaptionPresets : public ICaptionControl {
public:
    CaptionPresets(const IRECT& bounds, int paramIdx, const IText& text, const IColor& BGColor, bool showParamLabel = true)
    :ICaptionControl(bounds, paramIdx, text, BGColor) {}
    
    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx) override {
        if (pSelectedMenu != nullptr) {
            IPluginBase* mPlug = dynamic_cast<IPluginBase*> (GetDelegate());
            mPlug->RestorePreset(pSelectedMenu->GetChosenItemIdx());
            mPlug->GetUI()->GetControlWithTag(kCtrlTagBrowser)->SetDirty();
        }
    }
};


class IPlugPresets : public Plugin
{
public:
    IPlugPresets(const InstanceInfo& info);
    
#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
    void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
    void OnIdle() override;
    void OnParamChange(int paramIdx) override;
    void OnPopupMenuSelection(IPopupMenu* pSelectedMenu, int valIdx);
private:
    bool loaded = false;
    void createPresets();
    IVMeterControl<1>::Sender mMeterSender { kCtrlTagMeter };
#endif
};
