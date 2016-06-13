
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TText.h>

void plotShapes(){

  std::vector<TFile*> f;
  
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCDMuEnriched.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-1000to1400.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-120to170.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-1400to1800.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-15to30.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-170to300.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-300to470.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-30to50.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-470to600.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-50to80.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-600to800.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-800to1000.root"));
  //f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCD_Pt-80to120.root"));
  
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_WJets_LO_inc.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_WJets_LO_100To200.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_WJets_LO_200To400.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_WJets_LO_400To600.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_WJets_LO_600ToInf.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_QCDMuEnriched.root"));
  f.push_back(new TFile("../test/2015fakes_2/MC13TeV_TTJets_inclusive.root"));
  TString histname("wjet_step5met_denominator");
  //histname="wjet_step5byLooseCombinedIsolationDeltaBetaCorr3Hitsmet_numerator";
  TH1* wjets = (TH1*) f[0]->Get(histname);
  wjets->SetName("wjets");
  wjets->Add( (TH1*) f[1]->Get(histname));
  cout << "1" << endl;
  wjets->Add( (TH1*) f[2]->Get(histname));
  cout << "2" << endl;
  wjets->Add( (TH1*) f[3]->Get(histname));
  cout << "3" << endl;
  wjets->Add( (TH1*) f[4]->Get(histname));
  cout << "4" << endl;
  TH1* qcd = (TH1*) f[5]->Get(histname);
  qcd->SetName("qcd");
  TH1* ttbar = (TH1*) f[6]->Get(histname);
  ttbar->SetName("ttbar");

  wjets->SetLineColor(kBlack);
  ttbar->SetLineColor(kRed);
  qcd->SetLineColor(kBlue);

  wjets->Scale(1./wjets->Integral());
  ttbar->Scale(1./ttbar->Integral());
  qcd->Scale(1./qcd->Integral());


  TCanvas* c = new TCanvas("c","c",2000,2000);
  c->cd();
  gPad->SetLogy();
  
  wjets->Draw("hist");
  ttbar->Draw("histsame");
  qcd->Draw("histsame");
  
  c->Print("mycanva.png");

}
//../test/2015fakes_2/Data13TeV_JetHT2015B_05Oct2015.root
//../test/2015fakes_2/Data13TeV_JetHT2015C_05Oct2015.root
//../test/2015fakes_2/Data13TeV_JetHT2015D_05Oct2015.root
//../test/2015fakes_2/Data13TeV_SingleMuon2015B_05Oct2015.root
//../test/2015fakes_2/Data13TeV_SingleMuon2015C_05Oct2015.root
//../test/2015fakes_2/Data13TeV_SingleMuon2015D_05Oct2015.root
//../test/2015fakes_2/Data13TeV_SingleMuon2015D_PromptReco.root

//../test/2015fakes_2/MC13TeV_QCD_Pt-1000to1400.root
//../test/2015fakes_2/MC13TeV_QCD_Pt-120to170.root
//../test/2015fakes_2/MC13TeV_QCD_Pt-1400to1800.root
//../test/2015fakes_2/MC13TeV_QCD_Pt-15to30.root
//../test/2015fakes_2/MC13TeV_QCD_Pt-170to300.root
//../test/2015fakes_2/MC13TeV_QCD_Pt-300to470.root

