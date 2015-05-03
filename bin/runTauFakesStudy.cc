//
// Pietro Vischia, <pietro.vischia@gmail.com>
//
// Charged Higgs analysis
//

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

//Load here all the dataformat that we will need
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/GenericParticle.h"

#include "CondFormats/JetMETObjects/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
//#include "TauAnalysis/SVfitStandalone/interface/SVfitStandaloneAlgorithm.h" //for svfit

#include "TauAnalysis/JetToTauFakeRate/interface/MacroUtils.h"
#include "TauAnalysis/JetToTauFakeRate/interface/HiggsUtils.h"
#include "TauAnalysis/JetToTauFakeRate/interface/SmartSelectionMonitor.h"
#include "TauAnalysis/JetToTauFakeRate/interface/TMVAUtils.h"
#include "TauAnalysis/JetToTauFakeRate/interface/LeptonEfficiencySF.h"
#include "TauAnalysis/JetToTauFakeRate/interface/PDFInfo.h"
#include "TauAnalysis/JetToTauFakeRate/interface/MuScleFitCorrector.h"
#include "TauAnalysis/JetToTauFakeRate/interface/BtagUncertaintyComputer.h"
#include "TauAnalysis/JetToTauFakeRate/interface/GammaWeightsHandler.h"

#include "TauAnalysis/JetToTauFakeRate/interface/PatUtils.h"


#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TEventList.h"
#include "TROOT.h"
#include "TNtuple.h"
#include <Math/VectorUtil.h>

using namespace std;


int main (int argc, char *argv[])
{
  //##############################################
  //########    GLOBAL INITIALIZATION     ########
  //##############################################

  // check arguments
  if (argc < 2)
    {
      std::cout << "Usage : " << argv[0] << " parameters_cfg.py" << std::endl;
      exit (0);
    }
  
  // load framework libraries
  gSystem->Load ("libFWCoreFWLite");
  AutoLibraryLoader::enable ();
  
  // configure the process
  const edm::ParameterSet & runProcess = edm::readPSetsFrom (argv[1])->getParameter < edm::ParameterSet > ("runProcess");

  bool debug = runProcess.getParameter<bool>("debug");
  bool isMC = runProcess.getParameter < bool > ("isMC");
  double xsec = runProcess.getParameter < double >("xsec");
  int mctruthmode = runProcess.getParameter < int >("mctruthmode");
  
  TString suffix = runProcess.getParameter < std::string > ("suffix");
  std::vector < std::string > urls = runProcess.getParameter < std::vector < std::string > >("input");
  TString baseDir = runProcess.getParameter < std::string > ("dirName");
  TString url = TString (argv[1]);
  TString outFileUrl (gSystem->BaseName (url));
  outFileUrl.ReplaceAll ("_cfg.py", "");
  if (mctruthmode != 0)
    {
      outFileUrl += "_filt";
      outFileUrl += mctruthmode;
    }
  TString outdir = runProcess.getParameter < std::string > ("outdir");
  TString outUrl (outdir);
  gSystem->Exec ("mkdir -p " + outUrl);
  
  bool
    filterOnlyJETHT    (false),
    filterOnlySINGLEMU (false);
  if (!isMC)
    {
      if (url.Contains ("JetHT"))     filterOnlyJETHT    = true;
      if (url.Contains ("SingleMu"))  filterOnlySINGLEMU = true;
    }
  
  bool isSingleMuPD (!isMC && url.Contains ("SingleMu")); // Do I really need this?
  bool isV0JetsMC (isMC && (url.Contains ("DYJetsToLL_50toInf") || url.Contains ("WJets")));
  bool isWGmc (isMC && url.Contains ("WG"));
  bool isZGmc (isMC && url.Contains ("ZG"));
  bool isMC_ZZ = isMC && (string (url.Data ()).find ("TeV_ZZ") != string::npos);
  bool isMC_WZ = isMC && (string (url.Data ()).find ("TeV_WZ") != string::npos);
  
  TString outTxtUrl = outUrl + "/" + outFileUrl + ".txt";
  FILE *outTxtFile = NULL;
  if (!isMC) outTxtFile = fopen (outTxtUrl.Data (), "w");
  printf ("TextFile URL = %s\n", outTxtUrl.Data ());
  
  //tree info
  TString dirname = runProcess.getParameter < std::string > ("dirName");
  
  //systematics
  bool runSystematics = runProcess.getParameter < bool > ("runSystematics");
  std::vector < TString > varNames (1, "");
  if (runSystematics)
    {
      varNames.push_back ("_jerup" ); varNames.push_back ("_jerdown" );
      varNames.push_back ("_jesup" ); varNames.push_back ("_jesdown" );
      varNames.push_back ("_umetup"); varNames.push_back ("_umetdown");
      varNames.push_back ("_lesup" ); varNames.push_back ("_lesdown" );
      varNames.push_back ("_puup"  ); varNames.push_back ("_pudown"  );
      varNames.push_back ("_btagup"); varNames.push_back ("_btagdown");
      if (isMC_ZZ)
        {
          varNames.push_back ("_zzptup"); varNames.push_back ("_zzptdown");
        }
      if (isMC_WZ)
        {
          varNames.push_back ("_wzptup"); varNames.push_back ("_wzptdown");
        }
    }
  
  size_t nvarsToInclude = varNames.size ();

  std::vector < std::string > allWeightsURL = runProcess.getParameter < std::vector < std::string > >("weightsFile");
  std::string weightsDir (allWeightsURL.size ()? allWeightsURL[0] : "");
  
  //##############################################
  //########    INITIATING HISTOGRAMS     ########
  //##############################################
  SmartSelectionMonitor mon;

  //generator level control : add an underflow entry to make sure the histo is kept
  //((TH1F*)mon.addHistogram( new TH1D( "higgsMass_raw",     ";Higgs Mass [GeV];Events", 500,0,1500) ))->Fill(-1.0,0.0001);

  // ensure proper normalization
  TH1D* normhist = (TH1D*) mon.addHistogram(new TH1D("initNorm", ";;Nev", 4,0.,4.));
  normhist->GetXaxis()->SetBinLabel (1, "Gen. Events");
  normhist->GetXaxis()->SetBinLabel (2, "Trigger");
  normhist->GetXaxis()->SetBinLabel (3, "Truthmode");
  normhist->GetXaxis()->SetBinLabel (4, "Base");

  //event selection
  TH1D* h = (TH1D*) mon.addHistogram (new TH1D ("eventflow", ";;Events", 6, 0, 6));
  h->GetXaxis()->SetBinLabel (1, "#geq 2 iso leptons");
  h->GetXaxis()->SetBinLabel (2, "M_{ll} veto");
  h->GetXaxis()->SetBinLabel (3, "#geq 2 jets");
  h->GetXaxis()->SetBinLabel (4, "E_{T}^{miss}");
  h->GetXaxis()->SetBinLabel (5, "op. sign");
  h->GetXaxis()->SetBinLabel (6, "#geq 2 b-tags");
  h = (TH1D*) mon.addHistogram (new TH1D ("eventflowslep", ";;Events", 6, 0, 6));
  h->GetXaxis()->SetBinLabel (1, "1 iso lepton");
  h->GetXaxis()->SetBinLabel (2, "#geq 2 jets");
  h->GetXaxis()->SetBinLabel (3, "E_{T}^{miss}");
  h->GetXaxis()->SetBinLabel (4, "#geq 1 b-tag");
  h->GetXaxis()->SetBinLabel (5, "1 #tau");
  h->GetXaxis()->SetBinLabel (6, "op. sign");


  // Setting up control categories
  std::vector < TString > controlCats;
  controlCats.clear ();
  controlCats.push_back("step1");
  controlCats.push_back("step2");
  controlCats.push_back("step3");
  controlCats.push_back("step4");
  controlCats.push_back("step5");
  controlCats.push_back("step6");
  
  for (size_t k = 0; k < controlCats.size (); ++k)
    {
      TString icat (controlCats[k]);


      // Function of (-> histogram of numerator and denominator separately. Efficiencies computed after harvesting): jet pt, jet eta, jet radius, number of vertexes
      mon.addHistogram(new TH1D(icat+"pt_numerator",   ";p_{T}^{jet};Events", 50, 0., 500.)); // Variable number of bins to be implemented
      mon.addHistogram(new TH1D(icat+"pt_denominator", ";p_{T}^{jet};Events", 50, 0., 500.)); // Variable number of bins to be implemented

      mon.addHistogram(new TH1D(icat+"eta_numerator",   ";#eta_{jet};Events", 25, -2.5, 2.5));
      mon.addHistogram(new TH1D(icat+"eta_denominator", ";#eta_{jet};Events", 25, -2.5, 2.5));
      
      mon.addHistogram(new TH1D(icat+"radius_numerator",   ";R_{jet};Events", 20, 0., 1.));
      mon.addHistogram(new TH1D(icat+"radius_denominator", ";R_{jet};Events", 20, 0., 1.));

      mon.addHistogram(new TH1D(icat+"nvtx_numerator",   ";N_{vtx};Events", 30, 0., 60.));
      mon.addHistogram(new TH1D(icat+"nvtx_denominator", ";N_{vtx};Events", 30, 0., 60.));

      // Some control plots, mostly on event selection
      mon.addHistogram(new TH1D(icat+"nvtx",    ";Vertices;Events",                        50, 0.,   50.));
      mon.addHistogram(new TH1D(icat+"ptmu",    ";Muon transverse momentum [GeV];Events",  50, 0.,  500.));
      mon.addHistogram(new TH1D(icat+"jetpt",   ";Transverse momentum [GeV];Events",       50, 0., 1000.));
      mon.addHistogram(new TH1D(icat+"met",     ";Missing transverse energy [GeV];Events", 50, 0., 1000.));
      mon.addHistogram(new TH1D(icat+"recoMet", ";Missing transverse energy [GeV];Events", 50, 0., 1000.));
      mon.addHistogram(new TH1D(icat+"mt",      ";Transverse mass;Events",                 50, 0.,  500.));



    }                           // End of loop on controlCats

  //
  // STATISTICAL ANALYSIS
  //
  TH1D *Hoptim_systs = (TH1D *) mon.addHistogram (new TH1D ("optim_systs", ";syst;", nvarsToInclude, 0, nvarsToInclude));
  for (size_t ivar = 0; ivar < nvarsToInclude; ivar++) Hoptim_systs->GetXaxis ()->SetBinLabel (ivar + 1, varNames[ivar]);



  //##############################################
  //######## GET READY FOR THE EVENT LOOP ########
  //##############################################

  fwlite::ChainEvent ev (urls);
  const size_t totalEntries = ev.size ();

  //MC normalization (to 1/pb)
  double xsecWeight = xsec / totalEntries;
  if(!isMC) xsecWeight = 1.0;
  if(debug){
    cout << "DEBUG: xsec: " << xsec << endl;
    cout << "DEBUG: xsecWeight: " << xsecWeight << endl;
    cout << "DEBUG: totalEntries: " << totalEntries << endl;
  }
  
  
  //jet energy scale and uncertainties 
  TString jecDir = runProcess.getParameter < std::string > ("jecDir");
  gSystem->ExpandPathName (jecDir);
  FactorizedJetCorrector *jesCor = utils::cmssw::getJetCorrector (jecDir, isMC);
  JetCorrectionUncertainty *totalJESUnc = new JetCorrectionUncertainty ((jecDir + "/MC_Uncertainty_AK5PFchs.txt").Data ());

  //muon energy scale and uncertainties
  // MuScleFitCorrector *muCor = getMuonCorrector (jecDir, url);

  //lepton efficiencies
  LeptonEfficiencySF lepEff;

  //b-tagging: beff and leff must be derived from the MC sample using the discriminator vs flavor
  //the scale factors are taken as average numbers from the pT dependent curves see:
  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagPOG#2012_Data_and_MC_EPS13_prescript
  BTagSFUtil btsfutil;
  double beff (0.68), sfb (0.99), sfbunc (0.015);
  double leff (0.13), sfl (1.05), sflunc (0.12);

  //pileup weighting
  edm::LumiReWeighting * LumiWeights = NULL;
  utils::cmssw::PuShifter_t PuShifters;
  double PUNorm[] = { 1, 1, 1 };
  if (isMC)
    {
      std::vector < double >dataPileupDistributionDouble = runProcess.getParameter < std::vector < double >>("datapileup");
      std::vector < float >dataPileupDistribution;
      for (unsigned int i = 0; i < dataPileupDistributionDouble.size (); i++)
        {
          dataPileupDistribution.push_back (dataPileupDistributionDouble[i]);
        }
      std::vector < float >mcPileupDistribution;
      utils::getMCPileupDistributionFromMiniAOD (ev, dataPileupDistribution.size (), mcPileupDistribution);
      while (mcPileupDistribution.size () < dataPileupDistribution.size ()) mcPileupDistribution.push_back (0.0);
      while (mcPileupDistribution.size () > dataPileupDistribution.size ()) dataPileupDistribution.push_back (0.0);
      gROOT->cd ();             //THIS LINE IS NEEDED TO MAKE SURE THAT HISTOGRAM INTERNALLY PRODUCED IN LumiReWeighting ARE NOT DESTROYED WHEN CLOSING THE FILE
      LumiWeights = new edm::LumiReWeighting (mcPileupDistribution, dataPileupDistribution);
      PuShifters = utils::cmssw::getPUshifters (dataPileupDistribution, 0.05);
      utils::getPileupNormalization (mcPileupDistribution, PUNorm, LumiWeights, PuShifters);
    }

  gROOT->cd ();                 //THIS LINE IS NEEDED TO MAKE SURE THAT HISTOGRAM INTERNALLY PRODUCED IN LumiReWeighting ARE NOT DESTROYED WHEN CLOSING THE FILE
  
  //higgs::utils::EventCategory eventCategoryInst(higgs::utils::EventCategory::EXCLUSIVE2JETSVBF); //jet(0,>=1)+vbf binning
  
  
  //##############################################
  //########           EVENT LOOP         ########
  //##############################################
  //loop on all the events
  printf ("Progressing Bar     :0%%       20%%       40%%       60%%       80%%       100%%\n");
  printf ("Scanning the ntuple :");
  int treeStep (totalEntries / 50);
  //DuplicatesChecker duplicatesChecker;
  //int nDuplicates(0);
  int nSkipped(0);
  int nMultiChannel(0);
  int nNoChannel(0);
  for (size_t iev = 0; iev < totalEntries; iev++)
    {
      if (iev % treeStep == 0)
        {
          printf (".");
          fflush (stdout);
        }

      std::vector < TString > tags (1, "all");
      mon.fillHisto("initNorm", tags, 0., 1.);

      //##############################################   EVENT LOOP STARTS   ##############################################
      ev.to (iev);              //load the event content from the EDM file
      //if(!isMC && duplicatesChecker.isDuplicate( ev.run, ev.lumi, ev.event) ) { nDuplicates++; continue; }

      //apply trigger and require compatibilitiy of the event with the PD
      edm::TriggerResultsByName tr = ev.triggerResultsByName ("HLT");
      if (!tr.isValid ())
        return false;

      bool jetTrigger (utils::passTriggerPatterns(tr, "HLT_PFJet450_v*")); // It is unprescaled
      bool muTrigger   (utils::passTriggerPatterns (tr, "HLT_IsoMu24_IterTrk02_v*", "HLT_IsoTkMu24_IterTrk02_v*"));
      
      if (filterOnlyJETHT)    {                     muTrigger = false; }
      if (filterOnlySINGLEMU) { jetTrigger = false;                    }
      
      if (!(jetTrigger || muTrigger)){ nSkipped++; continue;}         //ONLY RUN ON THE EVENTS THAT PASS OUR TRIGGERS
      mon.fillHisto("initNorm", tags, 1., 1.);
      //##############################################   EVENT PASSED THE TRIGGER   #######################################
      
      //load all the objects we will need to access
      reco::VertexCollection vtx;
      fwlite::Handle < reco::VertexCollection > vtxHandle;
      vtxHandle.getByLabel (ev, "offlineSlimmedPrimaryVertices");
      if (vtxHandle.isValid() ) vtx = *vtxHandle;

      double rho = 0;
      fwlite::Handle < double >rhoHandle;
      rhoHandle.getByLabel (ev, "fixedGridRhoFastjetAll");
      if (rhoHandle.isValid() ) rho = *rhoHandle;

      /////// No gen particles are needed. Consider removing the commented part at all
      // no genparticles needed // reco::GenParticleCollection gen;
      // no genparticles needed // fwlite::Handle < reco::GenParticleCollection > genHandle;
      // no genparticles needed // genHandle.getByLabel (ev, "prunedGenParticles");
      // no genparticles needed // if (genHandle.isValid() ) gen = *genHandle;
      // no genparticles needed // 
      // no genparticles needed // // Save time and don't load the rest of the objects when selecting by mctruthmode :)
      // no genparticles needed // bool hasTop(false);
      // no genparticles needed // int
      // no genparticles needed //   ngenLeptonsStatus3(0),
      // no genparticles needed //   ngenTausStatus3(0),
      // no genparticles needed //   ngenQuarksStatus3(0);
      // no genparticles needed // //double tPt(0.), tbarPt(0.); // top pt reweighting - dummy value results in weight equal to 1 if not set in loop
      // no genparticles needed // //float wgtTopPt(1.0), wgtTopPtUp(1.0), wgtTopPtDown(1.0);
      // no genparticles needed // if(isMC)
      // no genparticles needed //   {
      // no genparticles needed //     //if(iev != 500) continue;
      // no genparticles needed //     for(size_t igen=0; igen<gen.size(); igen++){
      // no genparticles needed //       // Following the new status scheme from: https://github.com/cms-sw/cmssw/pull/7791
      // no genparticles needed //       
      // no genparticles needed //       //cout << "Particle " << igen << " has " << gen[igen].numberOfDaughters() << ", pdgId " << gen[igen].pdgId() << " and status " << gen[igen].status() << ", pt " << gen[igen].pt() << ", eta " << gen[igen].eta() << ", phi " << gen[igen].phi() << ". isHardProcess is " << gen[igen].isHardProcess() << ", and isPromptFinalState is " << gen[igen].isPromptFinalState() << endl;
      // no genparticles needed //       //if(!gen[igen].isHardProcess() && !gen[igen].isPromptFinalState()) continue;
      // no genparticles needed // 
      // no genparticles needed //       
      // no genparticles needed //       int absid=abs(gen[igen].pdgId());
      // no genparticles needed //       if(absid==6 /*&& gen[igen].isHardProcess()*/){ // particles of the hardest subprocess 22 : intermediate (intended to have preserved mass)
      // no genparticles needed //         hasTop=true;
      // no genparticles needed //         //if(isTTbarMC){
      // no genparticles needed //         //  if(gen[igen].get("id") > 0) tPt=gen[igen].pt();
      // no genparticles needed //         //  else                        tbarPt=gen[igen].pt();
      // no genparticles needed //         //}
      // no genparticles needed //       }
      // no genparticles needed // 
      // no genparticles needed //       //if(!gen[igen].isPromptFinalState() ) continue;
      // no genparticles needed // 
      // no genparticles needed //       if(absid==11 || absid==13) ngenLeptonsStatus3++;
      // no genparticles needed //       if(absid==15             ) ngenTausStatus3++; // This should be summed to ngenLeptonsStatus3 for the dilepton final states, not summed for the single lepton final states.
      // no genparticles needed //       if(absid<=5              ) ngenQuarksStatus3++;
      // no genparticles needed //     }
      // no genparticles needed //     
      // no genparticles needed //     // Dileptons:
      // no genparticles needed //     //    ttbar dileptons --> 1
      // no genparticles needed //     //    ttbar other     --> 2
      // no genparticles needed //     if(mctruthmode==1 && (ngenLeptonsStatus3+ngenTausStatus3!=2 || !hasTop )) continue;
      // no genparticles needed //     if(mctruthmode==2 && (ngenLeptonsStatus3+ngenTausStatus3==2 || !hasTop )) continue;
      // no genparticles needed //     // FIXME: port tt+bb splitting from 8 TeV (check the reference to the matched genjet)
      // no genparticles needed //     //if(mcTruthMode==1 && (ngenLeptonsStatus3!=2 || !hasTop || ngenBQuarksStatus23>=4)) continue;
      // no genparticles needed //     //if(mcTruthMode==2 && (ngenLeptonsStatus3==2 || !hasTop || ngenBQuarksStatus23>=4)) continue;
      // no genparticles needed //     //if(mcTruthMode==3 && (ngenBQuarksStatus23<4 || !hasTop))                           continue;
      // no genparticles needed // 
      // no genparticles needed //     // lepton-tau:
      // no genparticles needed //     //    ttbar ltau      --> 3
      // no genparticles needed //     //    ttbar dileptons --> 4
      // no genparticles needed //     //    ttbar ljets     --> 5
      // no genparticles needed //     //    ttbar hadrons   --> 6
      // no genparticles needed //     if(mctruthmode==3 && (ngenLeptonsStatus3!=1 || ngenTausStatus3!=1  || !hasTop )) continue;
      // no genparticles needed //     if(mctruthmode==4 && (ngenLeptonsStatus3!=2                        || !hasTop )) continue;
      // no genparticles needed //     if(mctruthmode==5 && (ngenLeptonsStatus3+ngenTausStatus3!=1        || !hasTop )) continue;
      // no genparticles needed //     if(mctruthmode==6 && (ngenLeptonsStatus3!=0 || ngenTausStatus3!=0  || !hasTop )) continue;
      // no genparticles needed // 
      // no genparticles needed //   }
      mon.fillHisto("initNorm", tags, 2., 1.);

      //      if(tPt>0 && tbarPt>0 && topPtWgt)
      //        {
      //          topPtWgt->computeWeight(tPt,tbarPt);
      //          topPtWgt->getEventWeight(wgtTopPt, wgtTopPtUp, wgtTopPtDown);
      //          wgtTopPtUp /= wgtTopPt;
      //          wgtTopPtDown /= wgtTopPt;
      //        }

      pat::MuonCollection muons;
      fwlite::Handle < pat::MuonCollection > muonsHandle;
      muonsHandle.getByLabel (ev, "slimmedMuons");
      if (muonsHandle.isValid() ) muons = *muonsHandle;

      pat::ElectronCollection electrons;
      fwlite::Handle < pat::ElectronCollection > electronsHandle;
      electronsHandle.getByLabel (ev, "slimmedElectrons");
      if (electronsHandle.isValid() ) electrons = *electronsHandle;

      pat::JetCollection jets;
      fwlite::Handle < pat::JetCollection > jetsHandle;
      jetsHandle.getByLabel (ev, "slimmedJets");
      if (jetsHandle.isValid() ) jets = *jetsHandle;

      /*
        pat::PhotonCollection photons;
        fwlite::Handle < pat::PhotonCollection > photonsHandle;
        photonsHandle.getByLabel (ev, "slimmedPhotons");
        if (photonsHandle.isValid() ) photons = *photonsHandle;
      */

      pat::METCollection mets;
      fwlite::Handle < pat::METCollection > metsHandle;
      metsHandle.getByLabel (ev, "slimmedMETs");
      if (metsHandle.isValid() ) mets = *metsHandle;
      LorentzVector met = mets[0].p4 ();

      if(debug ){
        // MET try:
        double mypt = mets[0].shiftedPt(pat::MET::METUncertainty::JetEnUp);
        cout << "MET = " << mets[0].pt() << ", JetEnUp: " << mypt << endl;
        LorentzVector myshiftedMet = mets[0].shiftedP4(pat::MET::METUncertainty::JetEnUp);
        cout << "MET = " << mets[0].pt() << ", JetEnUp: " << myshiftedMet.pt() << endl;
      }

      pat::TauCollection taus;
      fwlite::Handle < pat::TauCollection > tausHandle;
      tausHandle.getByLabel (ev, "slimmedTaus");
      if (tausHandle.isValid() ) taus = *tausHandle;

      if (isV0JetsMC)
        {
          fwlite::Handle < LHEEventProduct > lheEPHandle;
          lheEPHandle.getByLabel (ev, "externalLHEProducer");
          mon.fillHisto ("nup", "", lheEPHandle->hepeup ().NUP, 1);
          if (lheEPHandle->hepeup ().NUP > 5)  continue;
          mon.fillHisto ("nupfilt", "", lheEPHandle->hepeup ().NUP, 1);
        }
      
      //
      // DERIVE WEIGHTS TO APPLY TO SAMPLE
      //

      //pileup weight
      double weight = 1.0;
      double TotalWeight_plus = 1.0;
      double TotalWeight_minus = 1.0;
      double puWeight (1.0);

      if(isMC)
        {
          int ngenITpu = 0;
          
          fwlite::Handle < std::vector < PileupSummaryInfo > >puInfoH;
          puInfoH.getByLabel (ev, "addPileupInfo");
          for (std::vector < PileupSummaryInfo >::const_iterator it = puInfoH->begin (); it != puInfoH->end (); it++)
            {
              if (it->getBunchCrossing () == 0) ngenITpu += it->getPU_NumInteractions ();
            }
          
          puWeight = LumiWeights->weight (ngenITpu) * PUNorm[0];
          weight = 1.;//Weight; //* puWeight; // Temporarily disabled PU reweighing, it's wrong to scale to the 2012 data distribution.
          TotalWeight_plus =  PuShifters[utils::cmssw::PUUP]  ->Eval (ngenITpu) * (PUNorm[2]/PUNorm[0]);
          TotalWeight_minus = PuShifters[utils::cmssw::PUDOWN]->Eval (ngenITpu) * (PUNorm[1]/PUNorm[0]);
        }
      
      
      //
      //
      // BELOW FOLLOWS THE ANALYSIS OF THE MAIN SELECTION WITH N-1 PLOTS
      //
      //


      
      //
      // LEPTON ANALYSIS
      //
      
      //start by merging electrons and muons
      std::vector < patUtils::GenericLepton > leptons;
      for(size_t l = 0; l < electrons.size (); l++) leptons.push_back (patUtils::GenericLepton (electrons[l] ));
      for(size_t l = 0; l < muons.size (); l++)     leptons.push_back (patUtils::GenericLepton (muons[l]     ));
      std::sort (leptons.begin (), leptons.end (), utils::sort_CandidatesByPt);

      LorentzVector muDiff (0, 0, 0, 0);
      std::vector < patUtils::GenericLepton > selLeptons; // Different main lepton definitions
      double nVetoLeptons(0);
      for (size_t ilep = 0; ilep < leptons.size (); ilep++)
        {
          bool 
            passKin(true),             passId(true),              passIso(true),
            passVetoKin(true),          passVetoId(true),          passVetoIso(true);
          int lid = leptons[ilep].pdgId();
          
          //apply muon corrections
          // if (abs (lid) == 13)
          //   {
          //     if (muCor)
          //       {
          //         TLorentzVector p4 (leptons[ilep].px(), leptons[ilep].py(), leptons[ilep].pz(), leptons[ilep].energy());
          //         muCor->applyPtCorrection (p4, lid < 0 ? -1 : 1);
          //         if (isMC) muCor->applyPtSmearing (p4, lid < 0 ? -1 : 1, false);
          //         muDiff -= leptons[ilep].p4();
          //         leptons[ilep].setP4(LorentzVector(p4.Px(), p4.Py(), p4.Pz(), p4.E()));
          //         muDiff += leptons[ilep].p4();
          //       }
          //   }
          
          //no need for charge info any longer
          lid = abs (lid);
          TString lepStr(lid == 13 ? "mu" : "e");
          
          // don't want to mess with photon ID // //veto nearby photon (loose electrons are many times photons...)
          // don't want to mess with photon ID // double minDRlg(9999.);
          // don't want to mess with photon ID // for(size_t ipho=0; ipho<selPhotons.size(); ipho++)
          // don't want to mess with photon ID //   minDRlg=TMath::Min(minDRlg,deltaR(leptons[ilep].p4(),selPhotons[ipho].p4()));
          // don't want to mess with photon ID // if(minDRlg<0.1) continue;
          
          //kinematics
          double leta = fabs (lid == 11 ? leptons[ilep].el.superCluster ()->eta() : leptons[ilep].eta());
          
          // Single lepton main + veto kin
          if (leptons[ilep].pt () < (lid==11 ? 30. : 25.))     passKin = false;
          if (leta > (lid == 11 ? 2.5 : 2.1))                { passKin = false; passVetoKin = false; }
          if (lid == 11 && (leta > 1.4442 && leta < 1.5660)) { passKin = false; passVetoKin = false; } // Crack veto
          
          // Single lepton veto kin
          if (leptons[ilep].pt () < (lid==11 ? 20. : 10.))   passVetoKin = false;

          //Cut based identification 
          passId = lid == 11 ? patUtils::passId(leptons[ilep].el, vtx[0], patUtils::llvvElecId::Loose) : patUtils::passStdId (leptons[ilep].mu, vtx[0], patUtils::llvvMuonId::Tight);
          passVetoId = passId;

          //isolation
          passIso = lid == 11 ? patUtils::passIso (leptons[ilep].el, patUtils::llvvElecIso::Tight) : patUtils::passIso (leptons[ilep].mu, patUtils::llvvMuonIso::Tight); // Try tight iso for dilepton
          passVetoIso = passIso;
          
          if (passKin && passId && passIso) selLeptons.push_back(leptons[ilep]);
          else if(passVetoKin && passVetoId && passVetoIso) nVetoLeptons++;// "else if" is for having nVetoLeptons to be the number of *additional* leptons in the event.
          
        }
      std::sort(selLeptons.begin(),   selLeptons.end(),   utils::sort_CandidatesByPt);
      LorentzVector recoMET = met - muDiff;
      
      //select the taus
      pat::TauCollection selTaus;
      for (size_t itau = 0; itau < taus.size(); ++itau)
        {
          pat::Tau & tau = taus[itau];
          if (tau.pt() < 20. || fabs (tau.eta()) > 2.3) continue;
          
          bool overlapWithLepton(false);
          for(int l1=0; l1<(int)selLeptons.size();++l1){
            if(deltaR(tau, selLeptons[l1])<0.1){overlapWithLepton=true; break;}
          }
          if(overlapWithLepton) continue;
          
          //      if(!tau.isPFTau()) continue; // Only PFTaus // It should be false for slimmedTaus
          //      if(tau.emFraction() >=2.) continue;
          
          if(!tau.tauID("decayModeFindingNewDMs")) continue; // High pt tau. Otherwise, OldDMs
          // Anyways, the collection of taus from miniAOD should be already afer decayModeFinding cut (the tag - Old or New - is unspecified in the twiki, though).
          
          if (!tau.tauID ("byMediumCombinedIsolationDeltaBetaCorr3Hits")) continue;
          // Independent from lepton rejection algos performance
          // if (!tau.tauID ("againstMuonTight3"))                           continue;
          // if (!tau.tauID ("againstElectronMediumMVA5"))                   continue;
         
          selTaus.push_back(tau);
        }
      std::sort (selTaus.begin(), selTaus.end(), utils::sort_CandidatesByPt);
      
      
      
      //
      //JET/MET ANALYSIS
      //
      //add scale/resolution uncertainties and propagate to the MET      
      //utils::cmssw::updateJEC(jets,jesCor,totalJESUnc,rho,vtx.size(),isMC);  //FIXME if still needed
      //std::vector<LorentzVector> met=utils::cmssw::getMETvariations(recoMet,jets,selLeptons,isMC); //FIXME if still needed
      
      //select the jets
      pat::JetCollection
        selWJetsJets, selQCDJets;
      for (size_t ijet = 0; ijet < jets.size(); ijet++)
        {
          if (jets[ijet].pt() < 15 || fabs (jets[ijet].eta()) > 4.7) continue;
          
          //mc truth for this jet
          const reco::GenJet * genJet = jets[ijet].genJet();
          TString jetType (genJet && genJet->pt() > 0 ? "truejetsid" : "pujetsid");
          
          //cross-clean with selected leptons and photons
          double minDRlj(9999.);
          for (size_t ilep = 0; ilep < selLeptons.size(); ilep++)
            minDRlj = TMath::Min(minDRlj, deltaR (jets[ijet], selLeptons[ilep]));
          
          //jet id
          bool passPFloose = true;      //FIXME --> Need to be updated according to te latest recipe;
          float PUDiscriminant = jets[ijet].userFloat ("pileupJetId:fullDiscriminant");
          bool passLooseSimplePuId = true;      //FIXME --> Need to be updated according to the latest recipe
          if (!passPFloose || !passLooseSimplePuId || jets[ijet].pt() <20 || fabs(jets[ijet].eta()) > 2.5) continue;

          selQCDJets.push_back(jets[ijet]);
          if (minDRlj < 0.7) continue;
          selWJetsJets.push_back(jets[ijet]);
        }
      std::sort (selWJetsJets.begin(),  selWJetsJets.end(),  utils::sort_CandidatesByPt);
      std::sort (selQCDJets.begin(),  selQCDJets.end(),  utils::sort_CandidatesByPt);
      
      
      // Event classification and analyses
      if(muTrigger)  tags.push_back("wjets");
      if(jetTrigger) tags.push_back("qcd"  );
      if(muTrigger && jetTrigger) nMultiChannel++;
      if(!muTrigger && !jetTrigger) nNoChannel++;
      
      // W+jet full analysis
      if(muTrigger){
        // Selection mimicking AN2014_008_v11
        // - HLT_IsoMu24 trigger
        // - An event vertex with Ndof>=4, |Zvtx|<24cm, |r|<2cm
        // - A muon Pt>25GeV, |eta|<2.1, passing deltabeta-corrected isolation I<0.06ptmu  (Tight muon)
        // - Transverse mass Mt>50GeV (muon+MET)
        // - At least one jet pt>20GeV, |eta|<2.5 not overlapping with the muon withing DeltaR<0.7
        
        
        // At least one event vertex
        bool passVtxSelection(vtx.size()>0); // Ask someone about the offlineSkimmedPrimaryVertices collection
        // One lepton
        bool passLeptonSelection(selLeptons.size()==1); passLeptonSelection = (passLeptonSelection && (abs(selLeptons[0].pdgId()) == 13) );
        // Transverse mass
        double mt(0.);
        if(selLeptons.size()>0)
          {
            LorentzVector leptonformt = selLeptons[0].p4();
            mt =utils::cmssw::getMT<const LorentzVector,LorentzVector>(leptonformt,recoMET);
          }
        bool passMtSelection(mt>50 );
        // At least one jet not overlapping with the muon
        bool passJetSelection(selWJetsJets.size()>0);

       
        // Setting up control categories and fill up event flow histo
        std::vector < TString > ctrlCats; ctrlCats.clear ();
        mon.fillHisto("initNorm", tags, 3., 1.);
                                                                                           { ctrlCats.push_back("step1"); mon.fillHisto("eventflow", tags, 0, weight);}
        if(passVtxSelection)                                                               { ctrlCats.push_back("step2"); mon.fillHisto("eventflow", tags, 1, weight);}
        if(passVtxSelection && passLeptonSelection)                                        { ctrlCats.push_back("step3"); mon.fillHisto("eventflow", tags, 2, weight);}
        if(passVtxSelection && passLeptonSelection && passMtSelection )                    { ctrlCats.push_back("step4"); mon.fillHisto("eventflow", tags, 3, weight);}
        if(passVtxSelection && passLeptonSelection && passMtSelection && passJetSelection) { ctrlCats.push_back("step5"); mon.fillHisto("eventflow", tags, 4, weight);}
        
        
        // Fill the control plots
        for(size_t k=0; k<ctrlCats.size(); ++k){
          
          TString icat(ctrlCats[k]);
          if(icat!="step5") continue; // Only for final selection step, for a quick test
          
          
          // Fake rate: 
          // fr = (pt_jet>20 && |eta_jet| <2.3 && pt_tau>20 && |eta_tau|<2.3 && DM-finding && tauID) / (pt_jet>20 && |eta_jet| <2.3)
          for(pat::JetCollection::iterator jet=selWJetsJets.begin(); jet!=selWJetsJets.end(); ++jet)
            {
              if(abs(jet->eta())>2.3) continue;
              
              double jetWidth( ((jet->etaetaMoment()+jet->phiphiMoment())> 0) ? sqrt(jet->etaetaMoment()+jet->phiphiMoment()) : 0.);
              
              mon.fillHisto(icat+"pt_denominator",     tags, jet->pt() , weight); // Variable number of bins to be implemented
              mon.fillHisto(icat+"eta_denominator",    tags, jet->eta(), weight);
              mon.fillHisto(icat+"radius_denominator", tags, jetWidth  , weight);
              mon.fillHisto(icat+"nvtx_denominator",   tags, vtx.size(), weight);

              // Match taus
              //cross-clean with selected leptons and photons
              double minDRtj(9999.);
              pat::Tau theTau;
              for (pat::TauCollection::iterator tau=selTaus.begin(); tau!=selTaus.end(); ++tau)
                {
                  minDRtj = TMath::Min(minDRtj, deltaR(*jet, *tau));
                  theTau=*tau;
                }
              if(minDRtj>0.4) continue;
              if(theTau.pt()>20. || theTau.eta()>2.3) continue; // Numerator has both requirements (jet and tau) for pt and eta
              mon.fillHisto(icat+"pt_numerator",       tags, jet->pt() , weight); // Variable number of bins to be implemented
              mon.fillHisto(icat+"eta_numerator",      tags, jet->eta(), weight);
              mon.fillHisto(icat+"radius_numerator",   tags, jetWidth  , weight);
              mon.fillHisto(icat+"nvtx_numerator",     tags, vtx.size(), weight);

            }


          // Some control plots, mostly on event selection
          mon.fillHisto(icat+"nvtx",    tags,  vtx.size(), weight);
          if(selLeptons.size()>0)
            mon.fillHisto(icat+"ptmu",    tags,  selLeptons[0].pt(), weight);
          for(pat::JetCollection::iterator jet=selWJetsJets.begin(); jet!=selWJetsJets.end(); ++jet)
            mon.fillHisto(icat+"jetpt",   tags, jet->pt(), weight);
          mon.fillHisto(icat+"met",     tags,  met.pt(),     weight);
          mon.fillHisto(icat+"recoMet", tags,  recoMET.pt(), weight);
          mon.fillHisto(icat+"mt",      tags,  mt, weight);
        }
        
        
      } // End WJets full analysis
      
      // QCD full analysis
      if(jetTrigger){
        
        // - HLT_PfJet320 (450)
        // - An event vertex with Ndof>=4, |Zvtx|<24cm, |r|<2cm
        // - At least two jets pt>20GeV, |eta|<2.5. At least one of the jets is required to be matched within DeltaR<0.3 to the jet firing the jet trigger.
        // - If there is only one jet matching the trigger requirement in the event, the jet is excluded from the fake-rate computation


        // At least one event vertex
        bool passVtxSelection(vtx.size()>0); // Ask someone about the offlineSkimmedPrimaryVertices collection
        // At least two jets
        // Insert trigger matching here, which will fix the incomplete jet selection step
        bool passJetSelection(selQCDJets.size()>1);
        
        
        // Setting up control categories and fill up event flow histo
        std::vector < TString > ctrlCats;
        ctrlCats.clear ();
        mon.fillHisto("initNorm", tags, 3., 1.);
                                                 { ctrlCats.push_back ("step1"); mon.fillHisto("eventflowslep", tags, 0, weight);}
        if(passVtxSelection   )                  { ctrlCats.push_back ("step2"); mon.fillHisto("eventflowslep", tags, 1, weight);}
        if(passVtxSelection && passJetSelection) { ctrlCats.push_back ("step3"); mon.fillHisto("eventflowslep", tags, 2, weight);}
        
        
        // Fill the control plots
        for(size_t k=0; k<ctrlCats.size(); ++k){
          
          TString icat(ctrlCats[k]);
          if(icat!="step3") continue; // Only for final selection step, for a quick test
          
          // Fake rate: 
          // fr = (pt_jet>20 && |eta_jet| <2.3 && pt_tau>20 && |eta_tau|<2.3 && DM-finding && tauID) / (pt_jet>20 && |eta_jet| <2.3)
          for(pat::JetCollection::iterator jet=selWJetsJets.begin(); jet!=selWJetsJets.end(); ++jet)
            {
              if(abs(jet->eta())>2.3) continue;
              
              double jetWidth( ((jet->etaetaMoment()+jet->phiphiMoment())> 0) ? sqrt(jet->etaetaMoment()+jet->phiphiMoment()) : 0.);
              
              mon.fillHisto(icat+"pt_denominator",     tags, jet->pt() , weight); // Variable number of bins to be implemented
              mon.fillHisto(icat+"eta_denominator",    tags, jet->eta(), weight);
              mon.fillHisto(icat+"radius_denominator", tags, jetWidth  , weight);
              mon.fillHisto(icat+"nvtx_denominator",   tags, vtx.size(), weight);
              
              // Match taus
              //cross-clean with selected leptons and photons
              double minDRtj(9999.);
              pat::Tau theTau;
              for (pat::TauCollection::iterator tau=selTaus.begin(); tau!=selTaus.end(); ++tau)
                {
                  minDRtj = TMath::Min(minDRtj, deltaR(*jet, *tau));
                  theTau=*tau;
                }
              if(minDRtj>0.4) continue;
              if(theTau.pt()>20. || theTau.eta()>2.3) continue; // Numerator has both requirements (jet and tau) for pt and eta
              mon.fillHisto(icat+"pt_numerator",       tags, jet->pt() , weight); // Variable number of bins to be implemented
              mon.fillHisto(icat+"eta_numerator",      tags, jet->eta(), weight);
              mon.fillHisto(icat+"radius_numerator",   tags, jetWidth  , weight);
              mon.fillHisto(icat+"nvtx_numerator",     tags, vtx.size(), weight);
              
            }
          // Some control plots, mostly on event selection
          mon.fillHisto(icat+"nvtx",    tags,  vtx.size(), weight);
          if(selLeptons.size()>0)
            mon.fillHisto(icat+"ptmu",    tags,  selLeptons[0].pt(), weight);
          for(pat::JetCollection::iterator jet=selQCDJets.begin(); jet!=selQCDJets.end(); ++jet)
            mon.fillHisto(icat+"jetpt",   tags, jet->pt(), weight);
          mon.fillHisto(icat+"met",     tags,  met.pt(),     weight);
          mon.fillHisto(icat+"recoMet", tags,  recoMET.pt(), weight);
          // mon.fillHisto(icat+"mt",      tags,  mt, weight); // No mt for QCD selection (not requiring any lepton)
        }
        
      } // End single lepton full analysis
      
      continue; // Quick break (statistical analysis will come later)
      
      //
      // HISTOS FOR STATISTICAL ANALYSIS (include systematic variations)
      //
      //Fill histogram for posterior optimization, or for control regions
      for (size_t ivar = 0; ivar < nvarsToInclude; ivar++)
        {
          double iweight = weight;       //nominal

          //energy scale/resolution
          bool varyJesUp (varNames[ivar] == "_jesup");
          bool varyJesDown (varNames[ivar] == "_jesdown");
          bool varyJerUp (varNames[ivar] == "_jerup");
          bool varyJerDown (varNames[ivar] == "_jerdown");
          bool varyUmetUp (varNames[ivar] == "_umetup");
          bool varyUmetDown (varNames[ivar] == "_umetdown");
          bool varyLesUp (varNames[ivar] == "_lesup");
          bool varyLesDown (varNames[ivar] == "_lesdown");

          //pileup variations
          if (varNames[ivar] == "_puup")   iweight *= TotalWeight_plus;
          if (varNames[ivar] == "_pudown") iweight *= TotalWeight_minus;

          //btag
          bool varyBtagUp (varNames[ivar] == "_btagup");
          bool varyBtagDown (varNames[ivar] == "_btagdown");

          //Here were the Q^2 variations on VV pT spectum


          //recompute MET/MT if JES/JER was varied
          LorentzVector zvv = mets[0].p4();
          //FIXME
          //      if(varyJesUp)    zvv = mets[0].shiftedP4(pat::MET::METUncertainty::JetEnUp);
          //      if(varyJesDown)  zvv = mets[0].shiftedP4(pat::MET::METUncertainty::JetEnDown);
          //      if(varyJerUp)    zvv = mets[0].shiftedP4(pat::MET::METUncertainty::JetResUp);
          //      if(varyJerDown)  zvv = mets[0].shiftedP4(pat::MET::METUncertainty::JetResDown);
          //      if(varyUmetUp)   zvv = mets[0].shiftedP4(pat::MET::METUncertainty::UnclusteredEnUp);
          //      if(varyUmetDown) zvv = mets[0].shiftedP4(pat::MET::METUncertainty::UnclusteredEnDown);
          //      if(varyLesUp)    zvv = met[utils::cmssw::LESUP]; //FIXME  must vary all leptons separately: MuonEnUp/MuonEnDown/ElectronEnUp/ElectronEnDown/TauEnUp/TauEnDown
          //      if(varyLesDown)  zvv = met[utils::cmssw::LESDOWN];

          pat::JetCollection tightVarJets;
          bool passLocalBveto (true);///passBtags);
          for (size_t ijet = 0; ijet < jets.size(); ijet++)
            {

              double eta = jets[ijet].eta();
              if (fabs (eta) > 4.7) continue;
              double pt = jets[ijet].pt();
              //FIXME
              //        if(varyJesUp)    pt=jets[ijet].getVal("jesup");
              //        if(varyJesDown)  pt=jets[ijet].getVal("jesdown");
              //        if(varyJerUp)    pt=jets[ijet].getVal("jerup");
              //        if(varyJerDown)  pt=jets[ijet].getVal("jerdown");
              if (pt < 30) continue;

              //cross-clean with selected leptons and photons
              double minDRlj (9999.), minDRlg (9999.);
              for (size_t ilep = 0; ilep < selLeptons.size(); ilep++)
                minDRlj = TMath::Min (minDRlj, double(deltaR(jets[ijet].p4(), selLeptons[ilep].p4())));
              // don't want to mess with photon ID // for(size_t ipho=0; ipho<selPhotons.size(); ipho++)
              // don't want to mess with photon ID //   minDRlg = TMath::Min( minDRlg, deltaR(jets[ijet].p4(),selPhotons[ipho].p4()) );
              if (minDRlj < 0.4 /*|| minDRlg<0.4 */ ) continue;

              //jet id
              bool passPFloose = true;  //FIXME
              int simplePuId = true;    //FIXME
              bool passLooseSimplePuId = true;  //FIXME
              if (!passPFloose || !passLooseSimplePuId) continue;

              //jet is selected
              tightVarJets.push_back (jets[ijet]);

              //check b-tag
              if (pt < 30 || fabs (eta) > 2.5) continue;
              if (!isMC) continue;
              if (!varyBtagUp && !varyBtagDown) continue;
              int flavId = jets[ijet].partonFlavour();
              bool hasCSVtag (jets[ijet].bDiscriminator("combinedSecondaryVertexBJetTags") > 0.405);
              if (varyBtagUp)
                {
                  if (abs (flavId) == 5)      btsfutil.modifyBTagsWithSF(hasCSVtag, sfb + sfbunc,     beff);
                  else if (abs (flavId) == 4) btsfutil.modifyBTagsWithSF(hasCSVtag, sfb/5 + 2*sfbunc, beff);
                  else                        btsfutil.modifyBTagsWithSF(hasCSVtag, sfl + sflunc,     leff);
                }
              else if (varyBtagDown)
                {
                  if (abs (flavId) == 5)      btsfutil.modifyBTagsWithSF(hasCSVtag, sfb - sfbunc,     beff);
                  else if (abs (flavId) == 4) btsfutil.modifyBTagsWithSF(hasCSVtag, sfb/5 - 2*sfbunc, beff);
                  else                        btsfutil.modifyBTagsWithSF(hasCSVtag, sfl - sflunc,     leff);
                }
              passLocalBveto |= hasCSVtag;
            }


          //re-assign the event category to take migrations into account
          //      TString evCat  = eventCategoryInst.GetCategory(tightVarJets,dileptonSystem);
          //for (size_t ich = 0; ich < chTags.size(); ich++)
          //  {
          //    
          //    //TString tags_full = chTags[ich];  //+evCat;
          //    //double chWeight (iweight);
          //    
          //    //update weight and mass for photons
          //    //LorentzVector idileptonSystem (dileptonSystem);
          //    
          //    //updet the transverse mass
          //    //double mt = higgs::utils::transverseMass (idileptonSystem, zvv, true);
          //    
          //  }
        }
    }
  if(nSkipped>0) cout << "Warning! There were " << nSkipped << " skipped because of trigger events out of " << totalEntries << " events!" << endl;
  if(nMultiChannel>0) cout << "Warning! There were " << nMultiChannel << " multi-channel events out of " << totalEntries << " events!" << endl;
  if(nNoChannel>0) cout << "Warning! There were " << nNoChannel << " no-channel events out of " << totalEntries << " events!" << endl;
  printf ("\n");

  //##############################################
  //########     SAVING HISTO TO FILE     ########
  //##############################################
  //save control plots to file
  outUrl += "/";
  outUrl += outFileUrl + ".root";
  printf ("Results save in %s\n", outUrl.Data());

  //save all to the file
  TFile *ofile = TFile::Open (outUrl, "recreate");
  mon.Write();
  ofile->Close();

  if (outTxtFile)
    fclose (outTxtFile);
}
