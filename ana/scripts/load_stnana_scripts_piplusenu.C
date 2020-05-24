//
#include "TInterpreter.h"
#include "piplusenu/ana/scripts/modules.hh"
//-----------------------------------------------------------------------------
int load_stnana_scripts_piplusenu() {
  char        macro[200];
  const char* script[] = { 
    //    "global_vars.cc",
    //    "init_geometry.C",
    "genp.C",
    0 
  };

  const char* work_dir = gSystem->Getenv("MU2E_BASE_RELEASE");

  TInterpreter* cint = gROOT->GetInterpreter();
  
  for (int i=0; script[i] != 0; i++) {
    sprintf(macro,"%s/piplusenu/ana/scripts/%s",work_dir,script[i]);
    if (! cint->IsLoaded(macro)) {
      cint->LoadMacro(macro);
    }
  }
  
  return 0;
}
