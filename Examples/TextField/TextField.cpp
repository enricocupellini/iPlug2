#include "TextField.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

TextField::TextField(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
   /* pGraphics->AttachControl(new ITextControl(b, "Hello iPlug 2!", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));*/


    TF.Attach(pGraphics, b.GetCentredInside(300,50));
  };
#endif
}

#if IPLUG_DSP
void TextField::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}
#endif

bool TextField::SerializeState(IByteChunk& chunk) const
{
  TRACE;

  SavedString->Set((const char*)TF.D->EnteredText);
  chunk.PutStr(SavedString->Get());

  return  SerializeParams(chunk); // must remember to call SerializeParams at the end
}


int TextField::UnserializeState(const IByteChunk &chunk, int startPos)
{
  TRACE;

   startPos = chunk.GetStr(*SavedString, startPos);
   TF.D->EnteredText = (char*)SavedString->Get();
 
  return UnserializeParams(chunk, startPos);

}

inline void TextField::BuildStructure() {
  

}
