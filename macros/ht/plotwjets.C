
#include <iostream>

#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TText.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TString.h>

using namespace std;

void plotwjets(){

  gStyle->SetOptStat(0);

  std::vector<TFile*> f;
  f.push_back(new TFile("macros/ht/MC13TeV_WJets_LO_inc.root"));
  f.push_back(new TFile("macros/ht/MC13TeV_WJets_LO_100To200.root"));
  f.push_back(new TFile("macros/ht/MC13TeV_WJets_LO_200To400.root"));
  f.push_back(new TFile("macros/ht/MC13TeV_WJets_LO_400To600.root"));
  f.push_back(new TFile("macros/ht/MC13TeV_WJets_LO_600ToInf.root"));

  TString num_name("wjet_step6byLooseCombinedIsolationDeltaBetaCorr3Hitspt_numerator");
  TString den_name("wjet_step6pt_denominator");
  //den_name="all_lheHt";
  TString htname("all_lheHt");
  //htname="wjet_step5pt_denominator";

  TH1* n_t = NULL;
  TH1* d_t = NULL;


  TLegend* leg= new TLegend(0.6,0.6,0.9,0.9);

  TCanvas* c = new TCanvas("c","c",2000,2000);

  std::vector<TH1*> hts;
  
  c->Divide(3,2);
  for(size_t i=0; i<f.size(); ++i)
    {
      cout << "Doing file " << f[i]->GetName() << endl;
      c->cd(i+1);
      gPad->SetLogy();
      TH1* n = (TH1*) f[i]->Get(num_name);
      TH1* d = (TH1*) f[i]->Get(den_name);
      
      //if(n)  n->Scale(2318.35);
      //if(d)  d->Scale(2318.35);
      if(!n || !d) continue;
      if(i==0)
        {
          TH1* ntemp = (TH1*) f[i]->Get(num_name);
          TH1* dtemp = (TH1*) f[i]->Get(den_name);
          n_t = (TH1*) ntemp->Clone("n_total");
          d_t = (TH1*) dtemp->Clone("d_total");

        }
      else
        {
          n_t->Add((TH1*) f[i]->Get(num_name));
          d_t->Add((TH1*) f[i]->Get(den_name));
        }

      if(n && d){
        n->Divide(d);
        n->SetName(TString(n->GetName())+TString(f[i]->GetName()));
   
        n->SetTitle(f[i]->GetName());
      }
      if(d) d->SetName(TString(d->GetName())+TString(f[i]->GetName()));   

      TText *t = new TText(.5,.5,f[i]->GetName());
      if(n){
        n->SetMinimum(0.0001);
        n->SetMaximum(1.0);
        d->DrawCopy("hist");
        
      }
      gPad->Update();
      delete n;
      delete d;
      delete t;
    }

  c->cd(f.size()+1);
  gPad->SetLogy();
  n_t->SetMinimum(0.0001);
  n_t->SetMaximum(1.0);
  n_t->Divide(d_t);

  TFile* nostitch = new TFile("macros/ht/MC13TeV_W_Jets_LO_inc_nostitch.root");
  TH1* nosh = (TH1*) nostitch->Get(htname);
  TH1* normHist = (TH1*) nostitch->Get("all_initNorm");
  TH1* ptnosh = (TH1*) nostitch->Get(den_name);
  cout << "Number of generated WJets events: " << normHist->GetBinContent(1) << endl;
  cout << "No stitch has maximum " << nosh->GetMaximum() << ", and minimum " << nosh->GetMinimum() << endl;
  double noshnorm(50690.0/normHist->GetBinContent(1)); // 24184766. is the number of generated events from https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset%3D%2FWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8%2FRunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1%2FMINIAODSIM
  nosh->Scale(noshnorm);
  ptnosh->Scale(1000*noshnorm);
  
  cout << "Maxs: " << d_t->GetMaximum() << ", " << ptnosh->GetMaximum() << endl;
  cout << "Mins: " << d_t->GetMinimum() << ", " << ptnosh->GetMinimum() << endl;
  double intRatio(d_t->Integral()/ptnosh->Integral());
  cout << "Integral ratio: " << intRatio << endl;
  d_t->DrawCopy("hist");
  ptnosh->SetLineColor(2);
  ptnosh->DrawCopy("samehist");
  gPad->Update();

  c->Update();
  c->Print("mycanva.png");


  TCanvas* oc = new TCanvas("oc","oc",2000,2000);
  oc->cd();
  gPad->SetLogy();

  //TFile* nostitch = new TFile("macros/ht/MC13TeV_W_Jets_LO_inc_nostitch.root");
  //TH1* nosh = (TH1*) nostitch->Get(htname);
  //TH1* normHist = (TH1*) nostitch->Get("all_initNorm");
  //cout << "Number of generated WJets events: " << normHist->GetBinContent(1) << endl;
  //cout << "No stitch has maximum " << nosh->GetMaximum() << ", and minimum " << nosh->GetMinimum() << endl;
  //double noshnorm(50690.0/normHist->GetBinContent(1)); // 24184766. is the number of generated events from https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset%3D%2FWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8%2FRunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1%2FMINIAODSIM
  //nosh->Scale(noshnorm);
  nosh->SetLineColor(1);
  nosh->SetLineWidth(3);
  nosh->SetMaximum(nosh->GetMaximum()*1000);
  nosh->SetMinimum(1);
  nosh->Draw("hist");
  leg->AddEntry(nosh, "Inclusive nostitch", "l");
  TH1* sumstitch = NULL;
  double integral(0);
  for(size_t i=0; i<f.size(); ++i)
    {
      TH1* ht = (TH1*) f[i]->Get(htname);
      ht->SetName(TString(ht->GetName())+TString(f[i]->GetName()));
      leg->AddEntry(ht, f[i]->GetName(), "l");
      ht->SetLineColor(i+1);
      ht->SetLineWidth(3);
      ht->SetLineStyle(2);
      if(i==0) ht->SetLineStyle(4);
      cout << ht->GetName() << " has maximum " << ht->GetMaximum() << ", and minimum " << ht->GetMinimum() << endl;
      //hts->SetMaximum(1.1*n->FindMaximum());

      //ht->Scale(2318.35); 
      ht->DrawCopy("histsame");
      if(i==0)
        {
          ht->Scale(1./1000.);
          sumstitch = (TH1*) ht->Clone("stitchsum");
          cout << "Using " << ht->GetName() << " as base with integral " << ht->Integral() << endl;
          integral+=ht->Integral();
        }
      else if(i!=0)
        {
          //continue;
          sumstitch->Add(ht);
          cout << "Adding" << ht->GetName() << " with integral " << ht->Integral() << endl;
          integral+=ht->Integral();
        }

     
    }
  
  cout << "\n" << endl;
  cout << "No stitch integral: " << nosh->Integral() << endl;
  cout << "Sum of integrals: " << integral << endl;
  cout << "Stitched sum integral: " << (sumstitch ? sumstitch->Integral() : 0) << endl;
  //nosh->SetMaximum(sumstitch->GetMaximum()*1.2);
  if(sumstitch)
    {
      //sumstitch->Scale(2318.35);
      sumstitch->SetLineColor(2);
      sumstitch->SetLineStyle(1);
      sumstitch->SetLineColor(3);
      leg->AddEntry(sumstitch, "Stitched sum", "l");
      sumstitch->Draw("histsame");
    }
  
  leg->Draw();
  
  oc->Modified();
  oc->Update();
  oc->Print("myothercanva.png");

}
