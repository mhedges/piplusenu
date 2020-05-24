///////////////////////////////////////////////////////////////////////////////
// use of tmp:
//
// Tmp(0) : nax seg
// Tmp(1) : nst seg
// 
// use of debug bits: bits 0-2 are reserved
// ----------------------------------------
//  0  : all      events
//  1  : passed   events
//  2  : rejected events
// 
//  3  : events with SPMC T<100
//  4  : events with pbars ID=300000+I, X> 25cm
//  5  : events with pbars ID=400000+I
//  6  : events with pbars reaching the final stage
//  7  : events with pbars P > 100 MeV/c reaching the final stage
//  8  : events with pi (-211) in VD9 (before the target)
//  9  : events with pi (-211) at stage 4 (case of pbar simulation)
// 10  : events with pbars P > 100 MeV/c reaching the final stage
// 11  : events with pbars reaching the stopping target at T > 700 ns
// 99  : print warnings
///////////////////////////////////////////////////////////////////////////////
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TEnv.h"
#include "TSystem.h"

#include "Stntuple/loop/TStnAna.hh"
#include "Stntuple/obj/TStnHeaderBlock.hh"
#include "Stntuple/val/stntuple_val_functions.hh"
//------------------------------------------------------------------------------
// Mu2e offline includes
//-----------------------------------------------------------------------------
// #include "CalorimeterGeom/inc/HexMap.hh"

#include "ana/TStepPointMCAnaModule.hh"

#include "ana/InitVirtualDetectors.hh"

ClassImp(TStepPointMCAnaModule)
//-----------------------------------------------------------------------------
TStepPointMCAnaModule::TStepPointMCAnaModule(const char* name, const char* title):
  TStnModule(name,title)
{
  // fPdgCode       = 11;
  // fGeneratorCode = 28;
  fSpmcBlockName = "SpmcBlock";
  fVDetBlockName = "VDetBlock";

  fPdgDb = TDatabasePDG::Instance();

  for (int i=0; i<5000; i++) fParticleCache[i] = 0;

  SetParticleCache(   11,fPdgDb->GetParticle(   11)); // e
  SetParticleCache(  -11,fPdgDb->GetParticle(  -11)); // e
  SetParticleCache(   13,fPdgDb->GetParticle(   13)); // mu
  SetParticleCache(  -13,fPdgDb->GetParticle(  -13)); // mu
  SetParticleCache(   22,fPdgDb->GetParticle(   22)); // photon
  SetParticleCache(  211,fPdgDb->GetParticle(  211)); // pi^-
  SetParticleCache( -211,fPdgDb->GetParticle( -211)); // pi^-
  SetParticleCache( 2112,fPdgDb->GetParticle( 2112)); // neutron
  SetParticleCache( 2212,fPdgDb->GetParticle( 2212)); // proton
  SetParticleCache(-2212,fPdgDb->GetParticle(-2212)); // pbar

  fStnt = TStntuple::Instance();
}

//-----------------------------------------------------------------------------
TStepPointMCAnaModule::~TStepPointMCAnaModule() {
}


//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::BookEventHistograms(HistBase_t* Hist, const char* Folder) {
  //  char name [200];
  //  char title[200];
  EventHist_t* hist = (EventHist_t*) Hist;

  HBook1F(hist->fEventNumber,"evtnum",Form("%s: Event Number",Folder), 1000, 0,  1.e4,Folder);
  HBook1F(hist->fRunNumber  ,"runnum",Form("%s: Run   Number",Folder), 1000, 0,  1.e6,Folder);
  HBook1F(hist->fNSimp      ,"nsimp" ,Form("%s: N(sim particles)",Folder), 200, 0,  200,Folder);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::BookStepPointMCHistograms(HistBase_t* Hist, const char* Folder) {
  //     char name [200];
  //     char title[200];
  StepPointMCHist_t* hist = (StepPointMCHist_t*) Hist;

  HBook1F(hist->fVolumeID,"vol_id"   ,Form("%s: VolumeID"       ,Folder), 200,    0, 10000,Folder);
  HBook1F(hist->fGenIndex,"gen_index",Form("%s: GenIndex"       ,Folder), 200,    0, 10000,Folder);
  HBook1F(hist->fSimID   ,"sim_id"   ,Form("%s: SimID"          ,Folder), 200,    0,  1000,Folder);

  HBook1F(hist->fPDGCode[0] ,"pdg_0" ,Form("%s: PDG code"       ,Folder),  500,  -250,   250,Folder);
  HBook1F(hist->fPDGCode[1] ,"pdg_1" ,Form("%s: PDG code"       ,Folder), 2000, -10000, 10000,Folder);

  HBook1F(hist->fCreationCode   ,"cr_code",Form("%s: Creation code",Folder), 200,   0,   200,Folder);
  HBook1F(hist->fParentSimID    ,"psim_id",Form("%s: Parent SimID",Folder), 200,   0,  1000,Folder);
  HBook1F(hist->fParentPDGCode  ,"ppdg"   ,Form("%s: Parent PDG code",Folder), 200, -100,   100,Folder);
  HBook1F(hist->fEndProcessCode ,"end_code",Form("%s: End process code",Folder), 200,   0,   200,Folder);
  HBook1F(hist->fEDepTot        ,"edep_tot",Form("%s: EDEP tot"        ,Folder), 200,   0,   10 ,Folder);
  HBook1F(hist->fEDepNio        ,"edep_nio",Form("%s: EDEP NIO"        ,Folder), 200,   0,   10 ,Folder);
  HBook1F(hist->fTime           ,"time"    ,Form("%s: Time"            ,Folder), 200,   0,  2000,Folder);
  HBook1F(hist->fStepLength     ,"step"    ,Form("%s: Ltep Length"     ,Folder), 200,   0,   100,Folder);
  HBook1F(hist->fMom[0]         ,"mom"     ,Form("%s: Momentum[0]"     ,Folder), 500,   0,   500,Folder);
  HBook1F(hist->fMom[1]         ,"mom_1"   ,Form("%s: Momentum[1]"     ,Folder), 500,   0,  5000,Folder);

  HBook2F(hist->fCosThVsMom     ,"cth_vs_mom",Form("%s: Cos(Th) vs Mom",Folder), 250,   0,  5000,100,-1,1,Folder);

  HBook1F(hist->fEKin           ,"ekin"    ,Form("%s: kinetic energy"  ,Folder), 400,   0,   100,Folder);

  HBook2F(hist->fYVsX           ,"y_vs_x"     ,Form("%s: Y vs X"       ,Folder), 100, -250,  250, 100, -250, 250, Folder);
  HBook2F(hist->fYVsZ           ,"y_vs_z"     ,Form("%s: Y vs Z"       ,Folder), 500, -250,  250, 500, -250, 250, Folder);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::BookSimpHistograms(HistBase_t* Hist, const char* Folder) {
  SimpHist_t* hist = (SimpHist_t*) Hist;

  HBook1F(hist->fVolumeID   ,"vol_id"   ,Form("%s: Volume ID"   ,Folder), 200,  2400, 2600,Folder);
  HBook1F(hist->fStage      ,"stage"    ,Form("%s: Stage"       ,Folder),  10,     0,   10,Folder);
  HBook1F(hist->fGeneratorID,"gen_id"   ,Form("%s: Generator ID",Folder), 200,   -10,  190,Folder);
  HBook1F(hist->fTime       ,"time"     ,Form("%s: Stop Time"   ,Folder), 200,     0, 2000,Folder);
  HBook1F(hist->fParentMom  ,"pmom"     ,Form("%s: Parent Mom"  ,Folder), 200,     0, 2000,Folder);
  HBook1F(hist->fParentPDG  ,"ppdg"     ,Form("%s: Parent PDG"  ,Folder), 200, -1000, 1000,Folder);

  HBook1F(hist->fStartMom[0],"mom"        ,Form("%s: start Mom[0]"  ,Folder), 500,     0,  500,Folder);
  HBook1F(hist->fStartMom[1],"mom_1"      ,Form("%s: start Mom[1]"  ,Folder), 500,     0, 5000,Folder);
  HBook2F(hist->fYVsX       ,"y_vs_x"     ,Form("%s: yend vs Xend " ,Folder), 250,  -250, 250, 250, -250, 250,Folder);
  HBook2F(hist->fXEndVsZEnd ,"xe_vs_ze"   ,Form("%s: xend vs zend " ,Folder), 250,  -5000, 20000, 100, -5000, 5000,Folder);
  HBook2F(hist->fYVsX_2480  ,"y_vs_x_2480",Form("%s: Y vs X [2480]" ,Folder), 250,  -250, 250, 250, -250, 250,Folder);
  HBook2F(hist->fYVsX_2513  ,"y_vs_x_2513",Form("%s: Y vs X [2513]" ,Folder), 250,  -250, 250, 250, -250, 250,Folder);

  HBook2F(hist->fCosThVsMom ,"cth_vs_mom",Form("%s: Cos(Th) vs Mom",Folder), 250,   0,  5000,100,-1,1,Folder);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::BookVDetHistograms(HistBase_t* Hist, const char* Folder) {

  VDetHist_t* hist = (VDetHist_t*) Hist;

  HBook1F(hist->fIndex   ,"index"   ,Form("%s: VD index"      ,Folder),1000, 0, 1000,Folder);
  HBook1F(hist->fPDGCode ,"pdg_code",Form("%s: PDG code"      ,Folder),2000,-1000, 1000,Folder);
  HBook1F(hist->fGenCode ,"gen_code",Form("%s: generator code",Folder), 100, -10, 90,Folder);
  HBook1F(hist->fMom[0]  ,"mom"  ,Form("%s: Momentum[0]"   ,Folder), 500, 0, 500,Folder);
  HBook1F(hist->fMom[1]  ,"mom_1",Form("%s: Momentum[1]"   ,Folder), 500, 0,5000,Folder);
  HBook1F(hist->fTime    ,"time"    ,Form("%s: Hit Time  "    ,Folder), 200, 0,2000,Folder);
  HBook2F(hist->fYVsX    ,"y_vs_x"  ,Form("%s: Y vs X (all)"  ,Folder), 250, -250, 250, 250, -250, 250,Folder);
  HBook2F(hist->fYVsZ    ,"y_vs_z"  ,Form("%s: Y vs Z (all)"  ,Folder), 250, -250, 250, 250, -250, 250,Folder);
  HBook1F(hist->fPt      ,"pt"      ,Form("%s: Pt"            ,Folder), 200, 0, 200,Folder);
  HBook1F(hist->fPp      ,"pp"      ,Form("%s: P(parallel)"   ,Folder), 200, 0, 200,Folder);
  HBook1F(hist->fTanTh   ,"tan_th"  ,Form("%s: tan(pitch ang)",Folder), 500, -1, 4,Folder);
  HBook1F(hist->fEKin    ,"ekin"    ,Form("%s: kinetic energy",Folder), 400,  0, 100,Folder);

  HBook2F(hist->fCosThVsMom,"cth_vs_mom",Form("%s: Cos(Th) vs Mom",Folder), 250,   0,  5000,100,-1,1,Folder);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::BookHistograms() {

  //  char name [200];
  //  char title[200];

  TFolder* fol;
  TFolder* hist_folder;
  char     folder_name[200];

  DeleteHistograms();
  hist_folder = (TFolder*) GetFolder()->FindObject("Hist");
//-----------------------------------------------------------------------------
// book event histograms
//-----------------------------------------------------------------------------
  int book_event_histset[kNEventHistSets];
  for (int i=0; i<kNEventHistSets; i++) book_event_histset[i] = 0;

  book_event_histset[ 0] = 1;		// all events
  book_event_histset[ 1] = 0;		// just an example - this should be the default

  for (int i=0; i<kNEventHistSets; i++) {
    if (book_event_histset[i] != 0) {
      sprintf(folder_name,"evt_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fEvent[i] = new EventHist_t;
      BookEventHistograms(fHist.fEvent[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// book StepPointMC histograms
//-----------------------------------------------------------------------------
  int book_spmc_histset[kNStepPointMCHistSets];
  for (int i=0; i<kNStepPointMCHistSets; i++) book_spmc_histset[i] = 0;

  book_spmc_histset[0] = 1;		// all steps
  book_spmc_histset[1] = 1;		// electrons
  book_spmc_histset[2] = 1;		// positrons
  book_spmc_histset[3] = 1;		// mu-
  book_spmc_histset[4] = 1;		// mu+
  book_spmc_histset[5] = 1;		// photons
  book_spmc_histset[6] = 1;		// negative pions
  book_spmc_histset[7] = 1;		// positive pions
  book_spmc_histset[8] = 1;		// protons+antiprotons
  book_spmc_histset[9] = 1;		// everything else

  book_spmc_histset[10]  = 1;		// SPMC with T < 100 ns

  book_spmc_histset[20]  = 1;		// protons

  book_spmc_histset[21]  = 1;		// antiprotons
  book_spmc_histset[22]  = 1;		// antiprotons p > 100 MeV/c

  book_spmc_histset[101] = 1;		// electrons with P > 60 MeV/c
  book_spmc_histset[102] = 1;		// electrons with T > 400 ns
  book_spmc_histset[103] = 1;		// electrons with p < 2 MeV/c
  book_spmc_histset[104] = 1;		// electrons with 2 < p < 3 MeV/c
  book_spmc_histset[105] = 1;		// electrons with p > 3 MeV/c

  book_spmc_histset[203] = 1;		// positrons with p < 2 MeV/c
  book_spmc_histset[204] = 1;		// positrons with 2 < p < 3 MeV/c
  book_spmc_histset[205] = 1;		// positrons with p > 3 MeV/c

  book_spmc_histset[301] = 1;           // mu- p < 50 MeV/c
  book_spmc_histset[302] = 1;           // mu- p > 50 MeV/c

  book_spmc_histset[ 506] = 1;		// negative pions with weight
  book_spmc_histset[ 521] = 1;		// antiprotons, with Striganov weights
  book_spmc_histset[ 522] = 1;		// antiprotons p > 100 MeV/c, with Striganov weights

  book_spmc_histset[ 900] = 1;		// all particles , VD=9
  book_spmc_histset[ 913] = 1;		// mu-   in VD=9
  book_spmc_histset[ 921] = 1;		// pbars in VD=9

  book_spmc_histset[9100] = 1;		// all particles , VD=91
  book_spmc_histset[9113] = 1;		// mu- in VD=91
  book_spmc_histset[9121] = 1;		// pbars in VD=91

  for (int i=0; i<kNStepPointMCHistSets; i++) {
    if (book_spmc_histset[i] != 0) {
      sprintf(folder_name,"spmc_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fStepPointMC[i] = new StepPointMCHist_t;
      BookStepPointMCHistograms(fHist.fStepPointMC[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// book SimParticle histograms ??  none so far
//-----------------------------------------------------------------------------
  int book_simp_histset[kNSimpHistSets];
  for (int i=0; i<kNSimpHistSets; i++) book_simp_histset[i] = 0;

  book_simp_histset[  0] = 1;		// all particles 0+ID
  book_simp_histset[  3] = 1;		// mu-
  book_simp_histset[  4] = 1;		// mu+
  book_simp_histset[  5] = 1;		// pi-
  book_simp_histset[ 21] = 1;		// pbars

  book_simp_histset[100] = 1;		// stage 0
  book_simp_histset[103] = 1;		// mu-
  book_simp_histset[104] = 1;		// mu+
  book_simp_histset[105] = 1;		// pi-
  book_simp_histset[121] = 1;		// pbars

  book_simp_histset[200] = 1;		// stage 1
  book_simp_histset[203] = 1;		// mu-
  book_simp_histset[204] = 1;		// mu+
  book_simp_histset[205] = 1;		// pi-
  book_simp_histset[221] = 1;		// pbars

  book_simp_histset[300] = 1;		// stage 2
  book_simp_histset[303] = 1;		// mu-
  book_simp_histset[304] = 1;		// mu+
  book_simp_histset[305] = 1;		// pi-
  book_simp_histset[321] = 1;		// pbars

  book_simp_histset[400] = 1;		// stage 3
  book_simp_histset[403] = 1;		// mu-
  book_simp_histset[404] = 1;		// mu+
  book_simp_histset[405] = 1;		// pi-
  book_simp_histset[421] = 1;		// pbars

  book_simp_histset[500] = 1;		// stage 4
  book_simp_histset[503] = 1;		// mu-
  book_simp_histset[504] = 1;		// mu+
  book_simp_histset[505] = 1;		// pi-
  book_simp_histset[521] = 1;		// pbars

  book_simp_histset[600] = 1;		// particles stopped in the stoppping target
  book_simp_histset[603] = 1;		// mu-
  book_simp_histset[604] = 1;		// mu+
  book_simp_histset[605] = 1;		// pi-
  book_simp_histset[621] = 1;		// pbars

  book_simp_histset[700] = 1;		// particles stopped in the stoppping target, with weight
  book_simp_histset[703] = 1;		// mu-
  book_simp_histset[704] = 1;		// mu+
  book_simp_histset[705] = 1;		// pi-
  book_simp_histset[721] = 1;		// pbars

  book_simp_histset[1021] = 1;		// pbar in the production vertex for events reaching the end
  book_simp_histset[1022] = 1;		// pbar P>100 in the production vertex

  book_simp_histset[1023] = 1;		// pbar in the production vertex with Striganov's weights
  book_simp_histset[1024] = 1;		// pbar P > 100 in the production vertex with Striganov's weights

  for (int i=0; i<kNSimpHistSets; i++) {
    if (book_simp_histset[i] != 0) {
      sprintf(folder_name,"simp_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fSimp[i] = new SimpHist_t;
      BookSimpHistograms(fHist.fSimp[i],Form("Hist/%s",folder_name));
    }
  }
//-----------------------------------------------------------------------------
// book VDet histograms - so far need only VDET=9 (in front of the stopping target)
// virtual detector ID's are defined in DataProducts/inc/VirtualDetectorId.hh
//-----------------------------------------------------------------------------
  int book_vdet_histset[kNVDetHistSets];
  for (int i=0; i<kNVDetHistSets; i++) book_vdet_histset[i] = 0;

  book_vdet_histset[  9] = 1;		// all particles, VDET= 9 , ST_In
  book_vdet_histset[ 13] = 1;		// all particles, VDET=13 , TT_FrontHollow
  book_vdet_histset[ 14] = 1;		// all particles, VDET=13 , TT_FrontHollow, r > 40

  book_vdet_histset[101] = 1;		// e-  , VDET=1: Coll1_In
  book_vdet_histset[102] = 1;		// e-  , VDET=2: Coll1_Out
  book_vdet_histset[103] = 1;		// e-  , VDET=3: Coll31_In
  book_vdet_histset[104] = 1;		// e-  , VDET=4: Coll31_Out
  book_vdet_histset[105] = 1;		// e-  , VDET=5: Coll32_In 
  book_vdet_histset[106] = 1;		// e-  , VDET=6: Coll32_Out
  book_vdet_histset[107] = 1;		// e-  , VDET=7: Coll5_In
  book_vdet_histset[108] = 1;		// e-  , VDET=8: Coll5_Out
  book_vdet_histset[109] = 1;		// e-  , VDET=9
  book_vdet_histset[198] = 1;		// e-  , VDET=98
  book_vdet_histset[199] = 1;		// e-  , VDET=99

  book_vdet_histset[201] = 1;		// e-  , VDET=1: Coll1_In
  book_vdet_histset[202] = 1;		// e-  , VDET=2: Coll1_Out
  book_vdet_histset[203] = 1;		// e-  , VDET=3: Coll31_In
  book_vdet_histset[204] = 1;		// e-  , VDET=4: Coll31_Out
  book_vdet_histset[205] = 1;		// e-  , VDET=5: Coll32_In 
  book_vdet_histset[206] = 1;		// e-  , VDET=6: Coll32_Out
  book_vdet_histset[207] = 1;		// e-  , VDET=7: Coll5_In
  book_vdet_histset[208] = 1;		// e-  , VDET=8: Coll5_Out
  book_vdet_histset[209] = 1;		// e+  , VDET=9
  book_vdet_histset[298] = 1;		// e+  , VDET=98
  book_vdet_histset[299] = 1;		// e+  , VDET=99

  book_vdet_histset[301] = 1;		// all mu- , VDET=1: Coll1_In
  book_vdet_histset[302] = 1;		// all mu- , VDET=2: Coll1_Out
  book_vdet_histset[303] = 1;		// all mu- , VDET=3: Coll31_In
  book_vdet_histset[304] = 1;		// all mu- , VDET=4: Coll31_Out
  book_vdet_histset[305] = 1;		// all mu- , VDET=5: Coll32_In 
  book_vdet_histset[306] = 1;		// all mu- , VDET=6: Coll32_Out
  book_vdet_histset[307] = 1;		// all mu- , VDET=7: Coll5_In
  book_vdet_histset[308] = 1;		// all mu- , VDET=8: Coll5_Out
  book_vdet_histset[309] = 1;		// all mu- , VDET=9: ST_In
  book_vdet_histset[398] = 1;		// all mu- , VDET=98
  book_vdet_histset[399] = 1;		// all mu- , VDET=99

  book_vdet_histset[401] = 1;		// all mu+ , VDET=1: Coll1_In
  book_vdet_histset[402] = 1;		// all mu+ , VDET=2: Coll1_Out
  book_vdet_histset[403] = 1;		// all mu+ , VDET=3: Coll31_In
  book_vdet_histset[404] = 1;		// all mu+ , VDET=4: Coll31_Out
  book_vdet_histset[405] = 1;		// all mu+ , VDET=5: Coll32_In 
  book_vdet_histset[406] = 1;		// all mu+ , VDET=6: Coll32_Out
  book_vdet_histset[407] = 1;		// all mu+ , VDET=7: Coll5_In
  book_vdet_histset[408] = 1;		// all mu+ , VDET=8: Coll5_Out
  book_vdet_histset[409] = 1;		// all mu+ , VDET=9: ST_In
  book_vdet_histset[498] = 1;		// all mu+ , VDET=98
  book_vdet_histset[499] = 1;		// all mu+ , VDET=99

  book_vdet_histset[501] = 1;		// p<50 MeV/c mu- , VDET=1: Coll1_In
  book_vdet_histset[502] = 1;		// p<50 MeV/c mu- , VDET=2: Coll1_Out
  book_vdet_histset[503] = 1;		// p<50 MeV/c mu- , VDET=3: Coll31_In
  book_vdet_histset[504] = 1;		// p<50 MeV/c mu- , VDET=4: Coll31_Out
  book_vdet_histset[505] = 1;		// p<50 MeV/c mu- , VDET=5: Coll32_In 
  book_vdet_histset[506] = 1;		// p<50 MeV/c mu- , VDET=6: Coll32_Out
  book_vdet_histset[507] = 1;		// p<50 MeV/c mu- , VDET=7: Coll5_In
  book_vdet_histset[508] = 1;		// p<50 MeV/c mu- , VDET=8: Coll5_Out
  book_vdet_histset[509] = 1;		// p<50 MeV/c mu- , VDET=9: ST_In
  book_vdet_histset[598] = 1;		// p<50 MeV/c mu- , VDET=98
  book_vdet_histset[599] = 1;		// p<50 MeV/c mu- , VDET=99


  book_vdet_histset[601] = 1;		// p>50 MeV/c mu- , VDET=1: Coll1_In
  book_vdet_histset[602] = 1;		// p>50 MeV/c mu- , VDET=2: Coll1_Out
  book_vdet_histset[603] = 1;		// p>50 MeV/c mu- , VDET=3: Coll31_In
  book_vdet_histset[604] = 1;		// p>50 MeV/c mu- , VDET=4: Coll31_Out
  book_vdet_histset[605] = 1;		// p>50 MeV/c mu- , VDET=5: Coll32_In 
  book_vdet_histset[606] = 1;		// p>50 MeV/c mu- , VDET=6: Coll32_Out
  book_vdet_histset[607] = 1;		// p>50 MeV/c mu- , VDET=7: Coll5_In
  book_vdet_histset[608] = 1;		// p>50 MeV/c mu- , VDET=8: Coll5_Out
  book_vdet_histset[609] = 1;		// p>50 MeV/c mu- , VDET=9: ST_In
  book_vdet_histset[698] = 1;		// p>50 MeV/c mu- , VDET=98
  book_vdet_histset[699] = 1;		// p>50 MeV/c mu- , VDET=99

  book_vdet_histset[1001] = 1;		// pi- , VDET=1: Coll1_In
  book_vdet_histset[1002] = 1;		// pi- , VDET=2: Coll1_Out
  book_vdet_histset[1003] = 1;		// pi- , VDET=3: Coll31_In
  book_vdet_histset[1004] = 1;		// pi- , VDET=4: Coll31_Out
  book_vdet_histset[1005] = 1;		// pi- , VDET=5: Coll32_In 
  book_vdet_histset[1006] = 1;		// pi- , VDET=6: Coll32_Out
  book_vdet_histset[1007] = 1;		// pi- , VDET=7: Coll5_In
  book_vdet_histset[1008] = 1;		// pi- , VDET=8: Coll5_Out
  book_vdet_histset[1009] = 1;		// pi- , VDET=9: ST_In
  book_vdet_histset[1098] = 1;		// pi- , VDET=98
  book_vdet_histset[1099] = 1;		// pi- , VDET=99

  book_vdet_histset[1101] = 1;		// pi+ , VDET=1: Coll1_In
  book_vdet_histset[1102] = 1;		// pi+ , VDET=2: Coll1_Out
  book_vdet_histset[1103] = 1;		// pi+ , VDET=3: Coll31_In
  book_vdet_histset[1104] = 1;		// pi+ , VDET=4: Coll31_Out
  book_vdet_histset[1105] = 1;		// pi+ , VDET=5: Coll32_In 
  book_vdet_histset[1106] = 1;		// pi+ , VDET=6: Coll32_Out
  book_vdet_histset[1107] = 1;		// pi+ , VDET=7: Coll5_In
  book_vdet_histset[1108] = 1;		// pi+ , VDET=8: Coll5_Out
  book_vdet_histset[1109] = 1;		// pi+ , VDET=9: ST_In
  book_vdet_histset[1198] = 1;		// pi+ , VDET=98
  book_vdet_histset[1199] = 1;		// pi+ , VDET=99

  book_vdet_histset[2001] = 1;		// pbars , VDET=1: Coll1_In
  book_vdet_histset[2002] = 1;		// pbars , VDET=2: Coll1_Out
  book_vdet_histset[2003] = 1;		// pbars , VDET=3: Coll31_In
  book_vdet_histset[2004] = 1;		// pbars , VDET=4: Coll31_Out
  book_vdet_histset[2005] = 1;		// pbars , VDET=5: Coll32_In 
  book_vdet_histset[2006] = 1;		// pbars , VDET=6: Coll32_Out
  book_vdet_histset[2007] = 1;		// pbars , VDET=7: Coll5_In
  book_vdet_histset[2008] = 1;		// pbars , VDET=8: Coll5_Out
  book_vdet_histset[2009] = 1;		// pbars , VDET=9: ST_In
  book_vdet_histset[2091] = 1;		// pbars , VDET=91: before pbar window
  book_vdet_histset[2092] = 1;		// pbars , VDET=92: after pbar window
  book_vdet_histset[2098] = 1;		// pbars , VDET=98
  book_vdet_histset[2099] = 1;		// pbars , VDET=99

  book_vdet_histset[3001] = 1;		// pbars reaching the end, VDET=1: Coll1_In
  book_vdet_histset[3002] = 1;		// pbars reaching the end, VDET=2: Coll1_Out
  book_vdet_histset[3003] = 1;		// pbars reaching the end, VDET=3: Coll31_In
  book_vdet_histset[3004] = 1;		// pbars reaching the end, VDET=4: Coll31_Out
  book_vdet_histset[3005] = 1;		// pbars reaching the end, VDET=5: Coll32_In 
  book_vdet_histset[3006] = 1;		// pbars reaching the end, VDET=6: Coll32_Out
  book_vdet_histset[3007] = 1;		// pbars reaching the end, VDET=7: Coll5_In
  book_vdet_histset[3008] = 1;		// pbars reaching the end, VDET=8: Coll5_Out
  book_vdet_histset[3009] = 1;		// pbars reaching the end, VDET=9: ST_In
  book_vdet_histset[3091] = 1;		// pbars reaching the end, VDET=91: before pbar window
  book_vdet_histset[3092] = 1;		// pbars reaching the end, VDET=92: after pbar window
  book_vdet_histset[3098] = 1;		// pbars reaching the end, VDET=98
  book_vdet_histset[3099] = 1;		// pbars reaching the end, VDET=99

  book_vdet_histset[4001] = 1;		// pbars P>100 MeV/c reaching the end, VDET=1: Coll1_In
  book_vdet_histset[4002] = 1;		// pbars P>100 MeV/c reaching the end, VDET=2: Coll1_Out
  book_vdet_histset[4003] = 1;		// pbars P>100 MeV/c reaching the end, VDET=3: Coll31_In
  book_vdet_histset[4004] = 1;		// pbars P>100 MeV/c reaching the end, VDET=4: Coll31_Out
  book_vdet_histset[4005] = 1;		// pbars P>100 MeV/c reaching the end, VDET=5: Coll32_In 
  book_vdet_histset[4006] = 1;		// pbars P>100 MeV/c reaching the end, VDET=6: Coll32_Out
  book_vdet_histset[4007] = 1;		// pbars P>100 MeV/c reaching the end, VDET=7: Coll5_In
  book_vdet_histset[4008] = 1;		// pbars P>100 MeV/c reaching the end, VDET=8: Coll5_Out
  book_vdet_histset[4009] = 1;		// pbars P>100 MeV/c reaching the end, VDET=9: ST_In
  book_vdet_histset[4091] = 1;		// pbars P>100 MeV/c reaching the end, VDET=91: before pbar window
  book_vdet_histset[4092] = 1;		// pbars P>100 MeV/c reaching the end, VDET=92: after pbar window
  book_vdet_histset[4098] = 1;		// pbars P>100 MeV/c reaching the end, VDET=98
  book_vdet_histset[4099] = 1;		// pbars P>100 MeV/c reaching the end, VDET=99

  for (int i=0; i<kNVDetHistSets; i++) {
    if (book_vdet_histset[i] != 0) {
      sprintf(folder_name,"vdet_%i",i);
      fol = (TFolder*) hist_folder->FindObject(folder_name);
      if (! fol) fol = hist_folder->AddFolder(folder_name,folder_name);
      fHist.fVDet[i] = new VDetHist_t;
      BookVDetHistograms(fHist.fVDet[i],Form("Hist/%s",folder_name));
    }
  }
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::FillEventHistograms(HistBase_t* Hist) {
//   double            cos_th, xv, yv, rv, zv, p;
//   TLorentzVector    mom;

  EventHist_t* hist = (EventHist_t*) Hist;

  int event_number = GetHeaderBlock()->EventNumber();
  int run_number   = GetHeaderBlock()->RunNumber();

  hist->fEventNumber->Fill(event_number);
  hist->fRunNumber->Fill(run_number);
  hist->fNSimp->Fill(fNSimp);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::FillStepPointMCHistograms(HistBase_t* Hist, TStepPointMC* Step, SpmcData_t* SpmcData, double Weight) {

  StepPointMCHist_t* hist = (StepPointMCHist_t*) Hist;
  
  int pdg_code = Step->PDGCode();

  int id = (Step->VolumeID());

  VDetData_t* vdd = fVDet+id;

  hist->fVolumeID->Fill(id,Weight);
  hist->fGenIndex->Fill(Step->GenIndex(),Weight);
  hist->fSimID   ->Fill(Step->SimID(),Weight);
  hist->fPDGCode[0] ->Fill(pdg_code,Weight);
  hist->fPDGCode[1] ->Fill(pdg_code,Weight);
  hist->fCreationCode->Fill(Step->CreationCode(),Weight);
  hist->fParentSimID ->Fill(Step->ParentSimID(),Weight);
  hist->fParentPDGCode->Fill(Step->ParentPDGCode(),Weight);
  hist->fEndProcessCode->Fill(Step->EndProcessCode(),Weight);

  hist->fEDepTot->Fill(Step->EDepTot(),Weight);
  hist->fEDepNio->Fill(Step->EDepNio(),Weight);
  hist->fTime   ->Fill(Step->Time(),Weight);
  hist->fStepLength->Fill(Step->StepLength(),Weight);

  double p = Step->Mom()->Mag();
  hist->fMom[0]->Fill(p,Weight);
  hist->fMom[1]->Fill(p,Weight);
  
  double m(0);
  if (SpmcData->fParticle) {
    m = SpmcData->fParticle->Mass()*1e3;   // convert GeV -> MeV
  }

  double ekin = sqrt(p*p+m*m)-m;
  hist->fEKin->Fill(ekin,Weight);

  float x = Step->Pos()->X();
  float y = Step->Pos()->Y();
  float z = Step->Pos()->Z();

  // hack for stage 2:

  if (fabs(z-2929.) < 0.1) x = x+3904;

  hist->fYVsX->Fill(x,y,Weight);		// useful for stage 2
  hist->fYVsZ->Fill(z,y,Weight);		// useful for stage 1

  float pp;
  if (vdd->fIZLocal == 3) pp = Step->Mom()->Pz();
  else                    pp = -Step->Mom()->Px();

  float cos_th = pp/p;
  hist->fCosThVsMom->Fill(p,cos_th,Weight);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::FillSimpHistograms(HistBase_t* Hist, TSimParticle* Simp, SimpData_t* Sd, double Weight) {

  SimpHist_t* hist = (SimpHist_t*) Hist;

  int stage  = Simp->GetUniqueID()/100000;
  
  hist->fVolumeID->Fill(Simp->fEndVolumeIndex,Weight);
  hist->fStage->Fill(stage,Weight);
  hist->fGeneratorID->Fill(Simp->fGeneratorID,Weight);
  
  float xe = Simp->EndPos()->X()+3904.;
  float ye = Simp->EndPos()->Y();
  float ze = Simp->EndPos()->Z();
  float te = Simp->EndPos()->T();

  hist->fTime->Fill(te,Weight);

  // hist->fParentMom->Fill(fParent->StartMom()->P());
  // hist->fParentPDG->Fill(fParent->PDGCode());

  float p = Simp->StartMom()->P();
  hist->fStartMom[0]->Fill(p,Weight);
  hist->fStartMom[1]->Fill(p,Weight);

  hist->fYVsX->Fill(xe,ye,Weight);
  hist->fXEndVsZEnd->Fill(ze,xe,Weight);
//-----------------------------------------------------------------------------
// looks like something to do with the stopping target - but this is still 34 foils..
//-----------------------------------------------------------------------------
  if (Simp->fEndVolumeIndex == 2480) hist->fYVsX_2480->Fill(xe,ye,Weight);
  if (Simp->fEndVolumeIndex == 2513) hist->fYVsX_2513->Fill(xe,ye,Weight);

  float cos_th = Simp->StartMom()->Pz()/p;
  hist->fCosThVsMom->Fill(p,cos_th,Weight);
}

//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::FillVDetHistograms(HistBase_t* Hist, TStepPointMC* Step) {

  VDetHist_t* hist = (VDetHist_t*) Hist;

  int id = (Step->VolumeID());

  VDetData_t* vdd = fVDet+id;
  
  hist->fIndex   ->Fill(id);
  hist->fPDGCode ->Fill(Step->PDGCode());
  hist->fGenCode ->Fill(Step->GenIndex());

  float p = Step->Mom()->Mag();
  hist->fMom[0]->Fill(p);
  hist->fMom[1]->Fill(p);

  hist->fTime    ->Fill(Step->Time());

  // calculate local X and local Z

  double phi  = vdd->fPhiXZ*M_PI/180;

  double dx   = Step->Pos()->X()-vdd->fX;
  double dz   = Step->Pos()->Z()-vdd->fZ;

  double xloc =  dx*cos(phi) - dz*sin(phi);
  double zloc =  dx*sin(phi) + dz*cos(phi);

  hist->fYVsX    ->Fill(xloc,Step->Pos()->Y());
  hist->fYVsZ    ->Fill(zloc,Step->Pos()->Y());

  float py = Step->Mom()->Py();

  float px, pp, pt;
  if (vdd->fIZLocal == 3) {
    px = Step->Mom()->Px();
    pp = Step->Mom()->Pz();
  }
  else {
    px = Step->Mom()->Pz();
    pp = -Step->Mom()->Px();
  }

  pt = sqrt(px*px+py*py);

  hist->fPt->Fill(pt);
  hist->fPp->Fill(pp);
  hist->fTanTh->Fill(pt/pp);

  float cos_th = pp/p;
  hist->fCosThVsMom->Fill(p,cos_th);
}

//-----------------------------------------------------------------------------
// register data blocks and book histograms
//-----------------------------------------------------------------------------
int TStepPointMCAnaModule::BeginJob() {
//-----------------------------------------------------------------------------
// register data blocks 'SpmcBlock' or 'StepPointMCBlock' (old)
//-----------------------------------------------------------------------------
  RegisterDataBlock(fSpmcBlockName.Data(),"TStepPointMCBlock",&fStepPointMCBlock);
  RegisterDataBlock("SimpBlock"          ,"TSimpBlock"       ,&fSimpBlock       );
  RegisterDataBlock(fVDetBlockName.Data(),"TStepPointMCBlock",&fVDetBlock       );
//-----------------------------------------------------------------------------
// book histograms
//-----------------------------------------------------------------------------
  BookHistograms();

//-----------------------------------------------------------------------------
// initialize virtual detector offsets - a convenience for histogram filling
//-----------------------------------------------------------------------------
  InitVirtualDetectors(fVDet,&fNVDet);

  return 0;
}


//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::FillHistograms() {

//-----------------------------------------------------------------------------
// event histograms
//
// EVT_0: all events
//-----------------------------------------------------------------------------
  FillEventHistograms(fHist.fEvent[0]);
//-----------------------------------------------------------------------------
// SIM_PARTICLE histograms
//-----------------------------------------------------------------------------
  SimpData_t* sd = nullptr;  // temp

  for (int i=0; i<fNSimp; i++) {
    TSimParticle* simp = fSimpBlock->Particle(i);
    int pdg_code = simp->PDGCode();
    int simp_id  = simp->GetUniqueID();
    int vid1     = simp->EndVolumeIndex();
    double pend  = simp->EndMom()->P();

    FillSimpHistograms(fHist.fSimp[  0],simp,sd);
    if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[  3],simp,sd);
    if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[  4],simp,sd);
    if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[  5],simp,sd);
    if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[ 21],simp,sd);

    if      (simp_id < 100000) {
      FillSimpHistograms(fHist.fSimp[100],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[103],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[104],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[105],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[121],simp,sd);
    }
    else if (simp_id < 200000) {
      FillSimpHistograms(fHist.fSimp[200],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[203],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[204],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[205],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[221],simp,sd);
    }
    else if (simp_id < 300000) {
      FillSimpHistograms(fHist.fSimp[300],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[303],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[304],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[305],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[321],simp,sd);
    }
    else if (simp_id < 400000) {
      FillSimpHistograms(fHist.fSimp[400],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[403],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[404],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[405],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[421],simp,sd);
    }
    else if (simp_id < 500000) {
      FillSimpHistograms(fHist.fSimp[500],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[503],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[504],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[505],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[521],simp,sd);
    }

    if ((pend == 0) && (vid1 > 2500) && (vid1 < 2600)) {
//-----------------------------------------------------------------------------
// particle stopped in the stopping target
//-----------------------------------------------------------------------------
      FillSimpHistograms(fHist.fSimp[600],simp,sd);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[603],simp,sd);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[604],simp,sd);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[605],simp,sd);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[621],simp,sd);

      FillSimpHistograms(fHist.fSimp[700],simp,sd,fWeight);
      if (pdg_code ==    13) FillSimpHistograms(fHist.fSimp[703],simp,sd,fWeight);
      if (pdg_code ==   -13) FillSimpHistograms(fHist.fSimp[704],simp,sd,fWeight);
      if (pdg_code ==  -211) FillSimpHistograms(fHist.fSimp[705],simp,sd,fWeight);
      if (pdg_code == -2212) FillSimpHistograms(fHist.fSimp[721],simp,sd,fWeight);
    }
  }

//-----------------------------------------------------------------------------
// StepPointMC histograms
// for beamline studies, fStepPointMCBlock contains hits of particles for which 
// one of the stopping conditions has been satisfied, thus, this block contains 
// one StepPointMC per particle
//-----------------------------------------------------------------------------
  TStepPointMC* spmc;
  SpmcData_t    spmc_data;

  //  SimpData_t    sd1

  int nsteps = fStepPointMCBlock->NStepPoints();
  for (int i=0; i<nsteps; i++) {
    spmc             = fStepPointMCBlock->StepPointMC(i);
    float p          = spmc->Mom()->Mag();
    float t          = spmc->Time();
    int pdg_code     = spmc->PDGCode();
    int abs_pdg_code = abs(pdg_code);
//-----------------------------------------------------------------------------
// particles of interest are electrons, pions, muons and photons,
// speed up the mass extraction
//-----------------------------------------------------------------------------
    if (abs_pdg_code < 2500) spmc_data.fParticle = GetParticleCache(pdg_code);
    else                     spmc_data.fParticle = NULL;

    if (spmc_data.fParticle == NULL) {
      if (GetDebugBit(99) == 1) printf(">>> WARNING:bit99: no particle with PDG code=%i cached from ROOT particle DB\n",pdg_code);
    }

    FillStepPointMCHistograms(fHist.fStepPointMC[0],spmc,&spmc_data);

    if      (pdg_code ==   11) {
      FillStepPointMCHistograms(fHist.fStepPointMC[1],spmc,&spmc_data);
      if (p >  60) FillStepPointMCHistograms(fHist.fStepPointMC[101],spmc,&spmc_data);
      if (t > 400) FillStepPointMCHistograms(fHist.fStepPointMC[102],spmc,&spmc_data);
      if (p  <   2)             FillStepPointMCHistograms(fHist.fStepPointMC[103],spmc,&spmc_data);
      if ((p >=  2) && (p < 3)) FillStepPointMCHistograms(fHist.fStepPointMC[104],spmc,&spmc_data);
      if (p  >   3)             FillStepPointMCHistograms(fHist.fStepPointMC[105],spmc,&spmc_data);
    }
    else if (pdg_code ==  -11) {
      FillStepPointMCHistograms(fHist.fStepPointMC[2],spmc,&spmc_data);
      if (p  <   2)             FillStepPointMCHistograms(fHist.fStepPointMC[203],spmc,&spmc_data);
      if ((p >=  2) && (p < 3)) FillStepPointMCHistograms(fHist.fStepPointMC[204],spmc,&spmc_data);
      if (p  >   3)             FillStepPointMCHistograms(fHist.fStepPointMC[205],spmc,&spmc_data);
    }
    else if (pdg_code ==   13) {
//-----------------------------------------------------------------------------
// mu-
//-----------------------------------------------------------------------------
      FillStepPointMCHistograms(fHist.fStepPointMC[3],spmc,&spmc_data);
      if   (p <  50)                FillStepPointMCHistograms(fHist.fStepPointMC[301],spmc,&spmc_data);
      else                          FillStepPointMCHistograms(fHist.fStepPointMC[302],spmc,&spmc_data);
    }
    else if (pdg_code     ==   -13) FillStepPointMCHistograms(fHist.fStepPointMC[4],spmc,&spmc_data);
    else if (pdg_code     ==    22) FillStepPointMCHistograms(fHist.fStepPointMC[5],spmc,&spmc_data);
    else if (pdg_code     ==  -211) {
      FillStepPointMCHistograms(fHist.fStepPointMC[  6],spmc,&spmc_data);
      FillStepPointMCHistograms(fHist.fStepPointMC[506],spmc,&spmc_data,fWeight);
    }
    else if (pdg_code     ==   211) FillStepPointMCHistograms(fHist.fStepPointMC[7],spmc,&spmc_data);
    else if (abs_pdg_code ==  2212) FillStepPointMCHistograms(fHist.fStepPointMC[8],spmc,&spmc_data);  // protons+pbars
    else                            FillStepPointMCHistograms(fHist.fStepPointMC[9],spmc,&spmc_data);  // everything else

    if (pdg_code          ==  2212) FillStepPointMCHistograms(fHist.fStepPointMC[20],spmc,&spmc_data); // protons
    if (pdg_code          == -2212) {
//-----------------------------------------------------------------------------
// Antiproton
// SPMC_21: antiprotonparameters in the last recorded trajectory point
//-----------------------------------------------------------------------------
//      if (nh == 0) GetHeaderBlock()->Print("<<trouble>> nh = 0");

      FillStepPointMCHistograms(fHist.fStepPointMC[ 21],spmc,&spmc_data);                    // pbars
      FillStepPointMCHistograms(fHist.fStepPointMC[521],spmc,&spmc_data,fWeight);
//-----------------------------------------------------------------------------
// SIMP_1021: antiproton parameters in the production vertex
//-----------------------------------------------------------------------------
      int simp_id = spmc->SimID();
      TSimParticle* simp (nullptr);
      while (simp_id > 0) {
	 simp = fSimpBlock->FindParticle(simp_id);
	 simp_id = simp->ParentID();
      }

      FillSimpHistograms(fHist.fSimp[1021],simp,sd);
      FillSimpHistograms(fHist.fSimp[1023],simp,sd,fWeight);
//-----------------------------------------------------------------------------
// for events with antiproton reached the last plane, antiproton hits 
// in different detectors
//-----------------------------------------------------------------------------
      int nh = 0;
      for (int i=0; i<fNVDetHits; i++) {
	TStepPointMC* step = fVDetBlock->StepPointMC(i);
	if (step->PDGCode() == -2212) {
	  if      ((step->SimID() < 100000) && (step->VolumeID() ==  91)) {
	    FillVDetHistograms(fHist.fVDet[3091],step);
	    nh++;
	  }
	  else if (step->VolumeID() ==  92) FillVDetHistograms(fHist.fVDet[3092],step);
	  else if (step->VolumeID() ==   1) FillVDetHistograms(fHist.fVDet[3001],step);
	  else if (step->VolumeID() ==   2) FillVDetHistograms(fHist.fVDet[3002],step);
	  else if (step->VolumeID() ==   3) FillVDetHistograms(fHist.fVDet[3003],step);
	  else if (step->VolumeID() ==   4) FillVDetHistograms(fHist.fVDet[3004],step);
	  else if (step->VolumeID() ==   5) FillVDetHistograms(fHist.fVDet[3005],step);
	  else if (step->VolumeID() ==   6) FillVDetHistograms(fHist.fVDet[3006],step);
	  else if (step->VolumeID() ==   7) FillVDetHistograms(fHist.fVDet[3007],step);
	  else if (step->VolumeID() ==   8) FillVDetHistograms(fHist.fVDet[3008],step);
	  else if (step->VolumeID() ==   9) FillVDetHistograms(fHist.fVDet[3009],step);
	  else if (step->VolumeID() ==  98) FillVDetHistograms(fHist.fVDet[3098],step);
	  else if (step->VolumeID() ==  99) FillVDetHistograms(fHist.fVDet[3099],step);
	}
      }

      if (p > 100) { 
	FillStepPointMCHistograms(fHist.fStepPointMC[ 22],spmc,&spmc_data);                  // pbars p > 100 MeV/c
	FillStepPointMCHistograms(fHist.fStepPointMC[522],spmc,&spmc_data,fWeight);                  // pbars p > 100 MeV/c

	FillSimpHistograms(fHist.fSimp[1022],simp,sd);
	FillSimpHistograms(fHist.fSimp[1024],simp,sd,fWeight);
//-----------------------------------------------------------------------------
// for antiprotons P>100 MeV/c reached the last plane, antiproton hits 
// in different detectors
//-----------------------------------------------------------------------------
	int nh = 0;
	for (int i=0; i<fNVDetHits; i++) {
	  TStepPointMC* step = fVDetBlock->StepPointMC(i);
	  if (step->PDGCode() == -2212) {
	    if      ((step->SimID() < 100000) && (step->VolumeID() ==  91)) {
	      FillVDetHistograms(fHist.fVDet[4091],step);
	      nh++;
	    }
	    else if (step->VolumeID() ==  92) FillVDetHistograms(fHist.fVDet[4092],step);
	    else if (step->VolumeID() ==   1) FillVDetHistograms(fHist.fVDet[4001],step);
	    else if (step->VolumeID() ==   2) FillVDetHistograms(fHist.fVDet[4002],step);
	    else if (step->VolumeID() ==   3) FillVDetHistograms(fHist.fVDet[4003],step);
	    else if (step->VolumeID() ==   4) FillVDetHistograms(fHist.fVDet[4004],step);
	    else if (step->VolumeID() ==   5) FillVDetHistograms(fHist.fVDet[4005],step);
	    else if (step->VolumeID() ==   6) FillVDetHistograms(fHist.fVDet[4006],step);
	    else if (step->VolumeID() ==   7) FillVDetHistograms(fHist.fVDet[4007],step);
	    else if (step->VolumeID() ==   8) FillVDetHistograms(fHist.fVDet[4008],step);
	    else if (step->VolumeID() ==   9) FillVDetHistograms(fHist.fVDet[4009],step);
	    else if (step->VolumeID() ==  98) FillVDetHistograms(fHist.fVDet[4098],step);
	    else if (step->VolumeID() ==  99) FillVDetHistograms(fHist.fVDet[4099],step);
	  }
	}
      }

    }
//-----------------------------------------------------------------------------
// all hits with T < 100 ns
//-----------------------------------------------------------------------------
    if (spmc->Time()      <   100) {
      FillStepPointMCHistograms(fHist.fStepPointMC[10],spmc,&spmc_data);
      if (GetDebugBit(3)) {
	GetHeaderBlock()->Print("");
	spmc->Print();
      }
    }

    if (spmc->VolumeID() == 9) {
      FillStepPointMCHistograms(fHist.fStepPointMC[900],spmc,&spmc_data);
      if (pdg_code ==    13) FillStepPointMCHistograms(fHist.fStepPointMC[913],spmc,&spmc_data);
      if (pdg_code == -2212) FillStepPointMCHistograms(fHist.fStepPointMC[921],spmc,&spmc_data);
    }

    if (spmc->VolumeID() == 91) {
      FillStepPointMCHistograms(fHist.fStepPointMC[9100],spmc,&spmc_data);
      if (pdg_code ==    13) FillStepPointMCHistograms(fHist.fStepPointMC[9113],spmc,&spmc_data);
      if (pdg_code == -2212) FillStepPointMCHistograms(fHist.fStepPointMC[9121],spmc,&spmc_data);
    }
  }
  
//-----------------------------------------------------------------------------
// VDET histograms
//-----------------------------------------------------------------------------
  for (int i=0; i<fNVDetHits; i++) {
    TStepPointMC* step = fVDetBlock->StepPointMC(i);

    if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[ 9],step);
    if (step->VolumeID() == 13) {
      FillVDetHistograms(fHist.fVDet[13],step);
      float x = step->Pos()->X()+3904.;
      float y = step->Pos()->Y();
      float r = sqrt(x*x+y*y);
      if ((r >= 400) && (r < 800)) { 
	FillVDetHistograms(fHist.fVDet[14],step);
      }
    }

    if (step->PDGCode() == 11) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[101],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[102],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[103],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[104],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[105],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[106],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[107],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[108],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[109],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[198],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[199],step);
    }
    if (step->PDGCode() == -11) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[201],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[202],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[203],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[204],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[205],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[206],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[207],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[208],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[209],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[298],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[299],step);
    }
    if (step->PDGCode() == 13) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[301],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[302],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[303],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[304],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[305],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[306],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[307],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[308],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[309],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[398],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[399],step);

      float pmu = step->Mom()->Mag();
      if (pmu < 50) {
	if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[501],step);
	if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[502],step);
	if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[503],step);
	if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[504],step);
	if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[505],step);
	if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[506],step);
	if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[507],step);
	if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[508],step);
	if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[509],step);
	if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[598],step);
	if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[599],step);
      }
      else {
	if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[601],step);
	if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[602],step);
	if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[603],step);
	if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[604],step);
	if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[605],step);
	if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[606],step);
	if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[607],step);
	if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[608],step);
	if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[609],step);
	if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[698],step);
	if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[699],step);
      }
    }
    if (step->PDGCode() == -13) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[401],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[402],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[403],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[404],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[405],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[406],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[407],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[408],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[409],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[498],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[499],step);
    }
//-----------------------------------------------------------------------------
// negative pions
//-----------------------------------------------------------------------------
    if (step->PDGCode() == -211) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[1001],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[1002],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[1003],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[1004],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[1005],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[1006],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[1007],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[1008],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[1009],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[1098],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[1099],step);
    }
//-----------------------------------------------------------------------------
// positive pions
//-----------------------------------------------------------------------------
    if (step->PDGCode() ==  211) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[1101],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[1102],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[1103],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[1104],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[1105],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[1106],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[1107],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[1108],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[1109],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[1198],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[1199],step);
    }
//-----------------------------------------------------------------------------
// pbars
//-----------------------------------------------------------------------------
    if (step->PDGCode() == -2212) {
      if (step->VolumeID() ==  1) FillVDetHistograms(fHist.fVDet[2001],step);
      if (step->VolumeID() ==  2) FillVDetHistograms(fHist.fVDet[2002],step);
      if (step->VolumeID() ==  3) FillVDetHistograms(fHist.fVDet[2003],step);
      if (step->VolumeID() ==  4) FillVDetHistograms(fHist.fVDet[2004],step);
      if (step->VolumeID() ==  5) FillVDetHistograms(fHist.fVDet[2005],step);
      if (step->VolumeID() ==  6) FillVDetHistograms(fHist.fVDet[2006],step);
      if (step->VolumeID() ==  7) FillVDetHistograms(fHist.fVDet[2007],step);
      if (step->VolumeID() ==  8) FillVDetHistograms(fHist.fVDet[2008],step);
      if (step->VolumeID() ==  9) FillVDetHistograms(fHist.fVDet[2009],step);
      if (step->VolumeID() == 91) FillVDetHistograms(fHist.fVDet[2091],step);
      if (step->VolumeID() == 92) FillVDetHistograms(fHist.fVDet[2092],step);
      if (step->VolumeID() == 98) FillVDetHistograms(fHist.fVDet[2098],step);
      if (step->VolumeID() == 99) FillVDetHistograms(fHist.fVDet[2099],step);
    }
  }
}



//_____________________________________________________________________________
int TStepPointMCAnaModule::BeginRun() {
  int rn = GetHeaderBlock()->RunNumber();
  TStntuple::Init(rn);
  return 0;
}


//_____________________________________________________________________________
int TStepPointMCAnaModule::Event(int ientry) {

  //  double                p;
  //  TLorentzVector        mom;

  fStepPointMCBlock->GetEntry(ientry);
  fSimpBlock->GetEntry(ientry);
  fVDetBlock->GetEntry(ientry);
//-----------------------------------------------------------------------------
// assume electron in the first particle, otherwise the logic will need to 
// be changed
// if there are several hits, use the first one
//-----------------------------------------------------------------------------
  fNVDetHits = fVDetBlock->NStepPoints();

  fNSimp = fSimpBlock->NParticles();
//-----------------------------------------------------------------------------
// determine the cross section weight looking at the first particle
//-----------------------------------------------------------------------------
  fWeight = 1.;
  if (fNSimp > 0) {
    TSimParticle* sp0 = fSimpBlock->Particle(0);
    if (sp0->PDGCode() == -2212) {
//-----------------------------------------------------------------------------
// pbar production, assume Bob
// assuming parent particle exists, determine the production cross-section weight
// pbeam, nx, ny: the beam momentum and direction
//-----------------------------------------------------------------------------
      double pbeam(8.9), nx(-0.24192190), nz(-0.97029573);
      const TLorentzVector* sm = sp0->StartMom();
      double px    = sm->Px();
      double pz    = sm->Pz();
      double costh = (px*nx+pz*nz)/sqrt(px*px+pz*pz);
      double th    = TMath::ACos(costh);
//-----------------------------------------------------------------------------
// convert momentum to GeV/c
//-----------------------------------------------------------------------------
      double plab  = sm->P()/1000.;  

      fWeight      = fStnt->PBar_Striganov_d2N(pbeam,plab,th);
    }
  }
//-----------------------------------------------------------------------------
// determine simulation stage by looking at the last particle
//-----------------------------------------------------------------------------
  fStage = -1;
  if (fNSimp > 0) {
    TSimParticle* simp = fSimpBlock->Particle(fNSimp-1);
    int simp_id  = simp->GetUniqueID();
    fStage = simp_id / 100000;
  }

  for (int i=0; i<fNSimp; i++) {
    TSimParticle* simp = fSimpBlock->Particle(i);
    int simp_id  = simp->GetUniqueID();

    if (i < kMaxNSimp) {
      fSimData[i].fStage = simp_id/100000;

      int parent_id = simp->ParentID();
      while (parent_id > 0) {
	TSimParticle* parent = fSimpBlock->FindParticle(parent_id);
	int stage  = parent_id/100000;                        // stage : 0, 1, 2 3, 4 (so far)

	fSimData[i].fPart[stage] = parent;
	parent_id = parent->ParentID();
      }
    }
    else {
      printf(" TStepPointMCAnaModule::Event ERROR: too many SimParticles\n");
    }
  }
//-----------------------------------------------------------------------------
// everything is precalculated, fill histograms
//-----------------------------------------------------------------------------
  FillHistograms();

  Debug();

  return 0;		       
}

//-----------------------------------------------------------------------------
// bit 4: pbars with X> 25cm at TS5 in
// bit 5: pbars ID=400000+I
// bit 6: pbar in the final state
// bit 7: pbar P > 100 MeV/c in the final state
//-----------------------------------------------------------------------------
void TStepPointMCAnaModule::Debug() {

  for (int i=0; i<fNSimp; i++) {
    TSimParticle* simp = fSimpBlock->Particle(i);
    int pdg_code = simp->PDGCode();
    int simp_id  = simp->GetUniqueID();

    if (GetDebugBit(4) == 1) {
      if ((pdg_code == -2212) && (simp_id > 300000) && (simp_id < 400000)) {
	float xe = simp->EndPos()->X()+3904.;
	float ye = simp->EndPos()->Y();
	float ze = simp->EndPos()->Z();
	float te = simp->EndPos()->T();
	if (xe > 250) GetHeaderBlock()->Print(Form("bit:4: xe, ye, ze, te = %10.3f %10.3f%10.3f %10.3f",xe,ye,ze,te));
      }
    }

    if (GetDebugBit(5) == 1) {
      if ((pdg_code == -2212) && (simp_id > 400000) && (simp_id < 500000)) {
	GetHeaderBlock()->Print(Form("bit:5: pbar ID=%10i",simp_id));
      }
    }
  }
//-----------------------------------------------------------------------------
// for all stages except S3, StepPointMC collection represents particles reaching 
// the "STOP" volume, one hit per particle
//-----------------------------------------------------------------------------
  if (GetDebugBit(6) == 1) {
    int nsteps = fStepPointMCBlock->NStepPoints();
    for (int i=0; i<nsteps; i++) {
      TStepPointMC* spmc = fStepPointMCBlock->StepPointMC(i);
      float p            = spmc->Mom()->Mag();
      float t            = spmc->Time();
      int pdg_code       = spmc->PDGCode();
      
      if (pdg_code          == -2212) {
	GetHeaderBlock()->Print(Form("bit:6: pbar in the end, p = %10.3f t= %10.3f",p,t));
      }
    }
  }    
  if (GetDebugBit(7) == 1) {
    int nsteps = fStepPointMCBlock->NStepPoints();
    for (int i=0; i<nsteps; i++) {
      TStepPointMC* spmc = fStepPointMCBlock->StepPointMC(i);
      float p            = spmc->Mom()->Mag();
      float t            = spmc->Time();
      int pdg_code       = spmc->PDGCode();
      
      if ((pdg_code          == -2212) && (p > 100)) {
	GetHeaderBlock()->Print(Form("bit:7: pbar P > 100 in the end, p = %10.3f t= %10.3f",p,t));
      }
    }
  }
//-----------------------------------------------------------------------------
// bit:8  events with pi- at VD9
//-----------------------------------------------------------------------------
  if (GetDebugBit(8) == 1) {
    for (int i=0; i<fNVDetHits; i++) {
      TStepPointMC* step = fVDetBlock->StepPointMC(i);
      if      ((step->PDGCode() == -211) && (step->VolumeID() ==  9)) {
	float p            = step->Mom()->Mag();
	float t            = step->Time();
	GetHeaderBlock()->Print(Form("bit:8: pi- at VD 09, p = %10.3f t= %10.3f",p,t));
      }
    }
  }
//-----------------------------------------------------------------------------
// bit:9  events with pi- at stage4
//-----------------------------------------------------------------------------
  if (GetDebugBit(9) == 1) {
    for (int i=0; i<fNSimp; i++) {
      TSimParticle* simp = fSimpBlock->Particle(i);
      int pdg_code = simp->PDGCode();
      int simp_id  = simp->GetUniqueID();

      if (simp_id > 400000) {
	if (pdg_code ==  -211) {
	  GetHeaderBlock()->Print(Form("bit:9: pi- at stage 4"));
	}
      }
    }
  }
//-----------------------------------------------------------------------------
// bit:10  events with pbars- after TS2 with times 280-320 ns
//-----------------------------------------------------------------------------
  if (GetDebugBit(10) == 1) {
    int nsteps = fStepPointMCBlock->NStepPoints();
    for (int i=0; i<nsteps; i++) {
      TStepPointMC* spmc = fStepPointMCBlock->StepPointMC(i);
      float p            = spmc->Mom()->Mag();
      float t            = spmc->Time();
      int pdg_code       = spmc->PDGCode();
      
      if ((pdg_code  == -2212) && (t > 280) && (t < 320)) {
	GetHeaderBlock()->Print(Form("bit:10: pbar 280 < time < 320 ns in the end, p = %10.3f t= %10.3f",p,t));
      }
    }
  }
//-----------------------------------------------------------------------------
// bit:11  events with pbars reaching teh ST at T > 700 ns
//-----------------------------------------------------------------------------
  if (GetDebugBit(11) == 1) {
    for (int i=0; i<fNSimp; i++) {
      TSimParticle* simp = fSimpBlock->Particle(i);
      int pdg_code = simp->PDGCode();
      int simp_id  = simp->GetUniqueID();
      float te     = simp->EndPos()->T();
      float pe     = simp->EndMom()->P();

      if (simp_id > 400000) {
	if ((pdg_code ==  -2212) && (te > 700)) {
	  GetHeaderBlock()->Print(Form("bit:11: pbar at ST pe = %10.3f te = %10.3f",pe,te));
	}
      }
    }
  }
}

//_____________________________________________________________________________
int TStepPointMCAnaModule::EndJob() {
  printf("----- end job: ---- %s\n",GetName());
  return 0;
}

//_____________________________________________________________________________
void TStepPointMCAnaModule::Test001() {
}

