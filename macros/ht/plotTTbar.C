void plotTTbar(){
  gStyle->SetOptStat(0);                                                                                                                              
  TFile* f = TFile::Open("macros/ht/MC13TeV_TTJets_inclusive.root", "READ");

  std::vector<TString> steps;
  std::vector<std::pair<TString, TString> > hists;

  steps.push_back("step1");
  steps.push_back("step2");
  steps.push_back("step3");
  steps.push_back("step4");
  steps.push_back("step5");
  steps.push_back("step6");
  steps.push_back("step7");
  steps.push_back("step8");
  
  hists.push_back(std::make_pair("wjet_","jetpt"));
  hists.push_back(std::make_pair("wjet_","pt_denominator"));
  hists.push_back(std::make_pair("qcd_","jetpt"));
  hists.push_back(std::make_pair("qcd_","pt_denominator"));


  TCanvas* c = new TCanvas("c", "c", 2000, 2000);
  c->Divide(2,2);
  int k=0;
  for(std::vector<std::pair<TString,TString> >::iterator ihist = hists.begin(); ihist!=hists.end(); ++ihist, ++k)
    {
      c->cd(k+1);
      gPad->SetLogy();
      int j=0;
      for(std::vector<TString>::iterator istep = steps.begin(); istep!=steps.end(); ++istep, ++j)
        {
          if(ihist->first=="qcd_" && (*istep)=="step5") break;
          TH1* h = (TH1*) f->Get(ihist->first+(*istep)+ihist->second);
          cout << "Gotten " << h->GetName() << " with integral " << h->Integral() << endl;
          h->SetLineColor(j);
          h->SetLineWidth(3);
          h->Draw(j==0 ? "hist" : "histsame");
          
        }
      
    }
  
  c->Print("ttbarptshape.png");

}
