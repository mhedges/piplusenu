//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "piplusenu/ana/scripts/modules.hh"

def_name genp_001("spmc_ana");

void  spmc_ana(int DebugBit = -1) {
  m_spmc = (TStepPointMCAnaModule*) g.x->AddModule("TStepPointMCAnaModule",0);  
  if (DebugBit >= 0) m_spmc->SetDebugBit(DebugBit,1);
}

