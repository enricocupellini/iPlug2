#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
const int kNumPrograms = 1;

using namespace iplug;
using namespace igraphics;

class TextField {

public:
  class Data;
  class Graphics;

  Data* D;
  Graphics* Gra;

  TextField() {
    D = new Data;
    D->EnteredText = "Hello World";
    D->Attached = false;
  }

  inline void Attach(IGraphics* G, IRECT R) {
    Gra = new Graphics(R, D);
    G->AttachControl(Gra);
    D->Attached = true;
  }

  class Graphics : public IControl {
  public:
    Data* D;
    IRECT R;

    Graphics(IRECT& r, Data* d) : IControl(r) {
      D = d;
      R = r;
    }

    virtual void Draw(IGraphics& G) override {
      G.DrawText(IText(40), D->EnteredText, R);
      G.DrawRect(IColor::GetRandomColor(), R, 0, 1);

    }

    virtual void OnTextEntryCompletion(const char* str, int valIdx) {
      D->EnteredText = strdup(str);
      SetDirty(false);
    }

    inline void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
      GetUI()->CreateTextEntry(*this, IText(40), R, D->EnteredText);
    }
  };

  class Data {

  public:
    bool Attached;
    char* EnteredText;

  };


};


enum EParams
{
  kGain = 0,
  kNumParams
};

class TextField : public Plugin
{
public:
  TextField(const InstanceInfo& info);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;

  virtual bool SerializeState(IByteChunk& chunk) const override; //{ TRACE; return SerializeParams(chunk); }
  virtual int UnserializeState(const IByteChunk& chunk, int startPos) override; // { TRACE; return UnserializeParams(chunk, startPos); }
  inline void BuildStructure();

  TextField TF;

  WDL_String *SavedString = new WDL_String;
  char *ONOFF = "0";


#endif
};
