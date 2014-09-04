#include "Gain.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"



enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 30,
  kGainY = 40,
  kKnobFrames = 60
};

Gain::Gain(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(totalNumParams, NUM_PROGRAMS, instanceInfo)
{
  TRACE;

  gain = DEFAULT_GAIN;
  gainDb = .1 * exp( GAIN_EXP_CONST * DEFAULT_GAIN);
  pan = DEFAULT_PAN_CENTER;
  bypass = false;
  invertLeft = false;
  invertLeftMultiplier = 1.;
  invertRight = false;
  invertRightMultiplier = 1.;
  
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(gainParam)->InitDouble("Gain", .5, 0., 1., 0.01, "");
  GetParam(panParam)->InitDouble("Pan", .5, 0., 1., 0.01, "");
  GetParam(bypassParam)->InitDouble("Bypass", 0., 0., 1., 1., "");
  GetParam(invertLeftParam)->InitDouble("Invert Left", 0., 0., 1., 1., "");
  GetParam(invertRightParam)->InitDouble("Invert Right", 0., 0., 1., 1., "");


  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  IColor background = *new IColor(255, 57, 57, 57);
  pGraphics->AttachPanelBackground(&background);

  // Bitmap doesn't have knob in neutral position
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, gainParam, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", NUM_PROGRAMS);
}

Gain::~Gain() {}

void Gain::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  if (!bypass) {
    
      for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2) {
        
        /* Linear Pan Law (-6db center, not constant):
         leftChannel[i] = leftChannel[i] * (1.f - pan);
         rightChannel[i] = rightChannel[i] * pan;
         */
        
        /* Constant Power Pan Law (-3db center, thus THREE_DB used to raise center):
         leftChannel[i] s= leftChannel[i] * cosf(pan * M_PI_2) * THREE_DB;
         rightChannel[i] = rightChannel[i] * sinf(pan * M_PI_2) * THREE_DB;
         */
        
        
        /* Phase inverting is simple, just multiply the incoming signal for a channel by -1.f */
 
        *out1 = invertLeftMultiplier * *in1 * gainDb * cosf(pan * M_PI_2) * THREE_DB;
        *out2 = invertRightMultiplier * *in2 * gainDb * sinf(pan * M_PI_2) * THREE_DB;
        
      }
    
  } else {
    *out1 = *in1;
    *out2 = *in2;
  }

}

void Gain::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void Gain::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case gainParam:
      gain = GetParam(gainParam)->Value();
      // Map 0.-1. to .1-10.  (10. = 20db, .1 = -20db)
      gainDb = .1 * exp( GAIN_EXP_CONST * GetParam(gainParam)->Value());
      break;

    case panParam:
      // 0 = L, 0.5 = C, 1 = R
      pan = GetParam(panParam)->Value();
      break;
      
    case bypassParam:
      if (GetParam(bypassParam)->Value() > 0.) {
        bypass = true;
      } else {
        bypass = false;
      }
      break;
      
    case invertLeftParam:
      if (GetParam(invertLeftParam)->Value() > 0.) {
        invertLeft = true;
        invertLeftMultiplier = -1.;
      } else {
        invertLeft = false;
        invertLeftMultiplier = 1.;
      }
      break;
      
    case invertRightParam:
      if (GetParam(invertRightParam)->Value() > 0.) {
        invertRight = true;
        invertRightMultiplier = -1.;
      } else {
        invertRight = false;
        invertRightMultiplier = 1.;
      }
      break;
      
    default:
      break;
  }
}
