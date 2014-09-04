#ifndef __GAIN__
#define __GAIN__

#include "IPlug_include_in_plug_hdr.h"

class Gain : public IPlug
{
public:
  Gain(IPlugInstanceInfo instanceInfo);
  ~Gain();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  const int NUM_PROGRAMS = 1;

  const double GAIN_EXP_CONST = log(10./.1);
  const double DEFAULT_GAIN = .5;
  
  const double THREE_DB = 1.41254;
  const double DEFAULT_PAN_CENTER = .5;
  
  // Parameters & Internal Variables
  double gain, gainDb, pan, invertLeftMultiplier, invertRightMultiplier;
  bool bypass, invertLeft, invertRight;
  
  // Paramter Indices
  enum Parameters {
    
    gainParam,
    panParam,
    bypassParam,
    invertLeftParam,
    invertRightParam,
    
    totalNumParams
  };

};

#endif
