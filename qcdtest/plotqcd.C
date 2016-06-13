
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TText.h>

void plotqcd(){

  std::vector<TFile*> f;
  
  f.push_back(new TFile("MC13TeV_QCDMuEnriched.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-1000to1400.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-120to170.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-1400to1800.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-15to30.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-170to300.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-300to470.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-30to50.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-470to600.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-50to80.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-600to800.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-800to1000.root"));
  f.push_back(new TFile("MC13TeV_QCD_Pt-80to120.root"));

  TCanvas* c = new TCanvas("c","c",2000,2000);
  c->Divide(4,4);
  for(size_t i=0; i<f.size(); ++i)
    {
      cout << "Doing file " << f[i]->GetName() << endl;
      c->cd(i+1);
      gPad->SetLogy();
      //TH1* p = (TH1*) f[i]->Get("qcd_step3byVLooseIsolationMVA3newDMwLTeta_numerator");
      TH1* p = (TH1*) f[i]->Get("qcd_step3eta_denominator");
      if(p){
        p->SetName(TString(p->GetName())+TString(f[i]->GetName()));
        p->SetTitle(f[i]->GetName());
      }
      TText *t = new TText(.5,.5,f[i]->GetName());
      if(p)
        p->DrawCopy();
      gPad->Update();
      delete p;
      delete t;
    }
  c->Update();
  c->Print("mycanva.png");

}
