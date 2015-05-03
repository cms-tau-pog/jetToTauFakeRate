void plotFR(){

  vector<TString> analyses; analyses.clear();
  analyses.push_back("wjets");
  //  analyses.push_back("qcd");

  
  for(vector<TString>::iterator anal=analyses.begin(); anal!=analyses.end(); ++anal)
    {
      cout << "Anal is " << anal->Data() << endl;

      TFile* f = TFile::Open(TString("test/results/plotter_")+anal->Data()+TString(".root"));
      
      vector<TString> samples; samples.clear();
      
      if(*anal == "wjets")
        {
          samples.push_back("W,multijets");
          samples.push_back("QCDMuEnriched");
          samples.push_back("t#bar{t}");
        }
      else if(*anal == "qcd")
        {
          samples.push_back("t#bar{t}");
          samples.push_back("QCD");
        }
      
      TH1* pt_numerator = NULL;
      TH1* eta_numerator = NULL;
      TH1* radius_numerator = NULL;
      TH1* nvtx_numerator = NULL;
      
      TH1* pt_denominator = NULL;
      TH1* eta_denominator = NULL;
      TH1* radius_denominator = NULL;
      TH1* nvtx_denominator = NULL;
      
      for(vector<TString>::iterator sample = samples.begin(); sample!=samples.end(); ++sample)
        {
          
          if(!pt_numerator)     pt_numerator     = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5pt_numerator"));     else pt_numerator    ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5pt_numerator")    )); 
          if(!eta_numerator)    eta_numerator    = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5eta_numerator"));    else eta_numerator   ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5eta_numerator")   )); 
          if(!radius_numerator) radius_numerator = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5radius_numerator")); else radius_numerator->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5radius_numerator")));
          if(!nvtx_numerator)   nvtx_numerator   = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5nvtx_numerator"));   else nvtx_numerator  ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5nvtx_numerator")  ));   
          
          if(!pt_denominator)     pt_denominator     = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5pt_denominator"));     else pt_denominator    ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5pt_denominator")    )); 
          if(!eta_denominator)    eta_denominator    = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5eta_denominator"));    else eta_denominator   ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5eta_denominator")   )); 
          if(!radius_denominator) radius_denominator = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5radius_denominator")); else radius_denominator->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5radius_denominator")));
          if(!nvtx_denominator)   nvtx_denominator   = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5nvtx_denominator"));   else nvtx_denominator  ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_step5nvtx_denominator")  ));   
          
        }
      
      
      
      if(!pt_numerator      ) cout << " pt_numerator       is NULL" << endl; else pt_numerator      ->Sumw2();
      if(!eta_numerator     ) cout << " eta_numerator      is NULL" << endl; else eta_numerator     ->Sumw2();
      if(!radius_numerator  ) cout << " radius_numerator   is NULL" << endl; else radius_numerator  ->Sumw2();
      if(!nvtx_numerator    ) cout << " nvtx_numerator     is NULL" << endl; else nvtx_numerator    ->Sumw2();
      if(!pt_denominator    ) cout << " pt_denominator     is NULL" << endl; else pt_denominator    ->Sumw2();
      if(!eta_denominator   ) cout << " eta_denominator    is NULL" << endl; else eta_denominator   ->Sumw2();
      if(!radius_denominator) cout << " radius_denominator is NULL" << endl; else radius_denominator->Sumw2();
      if(!nvtx_denominator  ) cout << " nvtx_denominator   is NULL" << endl; else nvtx_denominator  ->Sumw2();
      
      
      
      pt_numerator    ->Divide( pt_denominator     );
      eta_numerator   ->Divide( eta_denominator    );
      radius_numerator->Divide( radius_denominator );
      nvtx_numerator  ->Divide( nvtx_denominator   );
      
      
      TCanvas* c = new TCanvas("fakerate", "fakerate", 800, 800);
      c->Divide(2,2);
      c->cd(1);
      gPad->SetLogy();
      pt_numerator->Draw("");
      c->cd(2);
      eta_numerator->Draw("");
      c->cd(3);
      gPad->SetLogy();
      radius_numerator->Draw("");
      c->cd(4);
      nvtx_numerator->Draw("");

      c->Print(TString("test/results/fakes_")+anal->Data()+TString(".png"));
      
    }

}
