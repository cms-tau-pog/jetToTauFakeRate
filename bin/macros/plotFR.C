void plotFR(){

  vector<TString> analyses; analyses.clear();
  analyses.push_back("wjet");
  analyses.push_back("qcd");

  vector<TString> tauDiscriminators; tauDiscriminators.clear();

  tauDiscriminators.push_back("byLooseCombinedIsolationDeltaBetaCorr3Hits");
  tauDiscriminators.push_back("byMediumCombinedIsolationDeltaBetaCorr3Hits");
  tauDiscriminators.push_back("byTightCombinedIsolationDeltaBetaCorr3Hits");
                          
  tauDiscriminators.push_back("byVLooseIsolationMVA3oldDMwoLT");
  tauDiscriminators.push_back("byLooseIsolationMVA3oldDMwoLT");
  tauDiscriminators.push_back("byMediumIsolationMVA3oldDMwoLT");
  tauDiscriminators.push_back("byTightIsolationMVA3oldDMwoLT");
  tauDiscriminators.push_back("byVTightIsolationMVA3oldDMwoLT");
  tauDiscriminators.push_back("byVVTightIsolationMVA3oldDMwoLT");
  
  tauDiscriminators.push_back("byVLooseIsolationMVA3oldDMwLT");
  tauDiscriminators.push_back("byLooseIsolationMVA3oldDMwLT");
  tauDiscriminators.push_back("byMediumIsolationMVA3oldDMwLT");
  tauDiscriminators.push_back("byTightIsolationMVA3oldDMwLT");
  tauDiscriminators.push_back("byVTightIsolationMVA3oldDMwLT");
  tauDiscriminators.push_back("byVVTightIsolationMVA3oldDMwLT");
  
  tauDiscriminators.push_back("byVLooseIsolationMVA3newDMwoLT");
  tauDiscriminators.push_back("byLooseIsolationMVA3newDMwoLT");
  tauDiscriminators.push_back("byMediumIsolationMVA3newDMwoLT");
  tauDiscriminators.push_back("byTightIsolationMVA3newDMwoLT");
  tauDiscriminators.push_back("byVTightIsolationMVA3newDMwoLT");
  tauDiscriminators.push_back("byVVTightIsolationMVA3newDMwoLT");
  
  tauDiscriminators.push_back("byVLooseIsolationMVA3newDMwLT");
  tauDiscriminators.push_back("byLooseIsolationMVA3newDMwLT");
  tauDiscriminators.push_back("byMediumIsolationMVA3newDMwLT");
  tauDiscriminators.push_back("byTightIsolationMVA3newDMwLT");
  tauDiscriminators.push_back("byVTightIsolationMVA3newDMwLT");
  tauDiscriminators.push_back("byVVTightIsolationMVA3newDMwLT");

  for(vector<TString>::iterator anal=analyses.begin(); anal!=analyses.end(); ++anal)
    {
      cout << "Anal is " << anal->Data() << endl;
      
      TFile* f = TFile::Open(TString("test/results/plotter_")+anal->Data()+TString(".root"));
      
      vector<TString> samples; samples.clear();
      TString step("");
           
      if(*anal == "wjet")
        {
          samples.push_back("W,multijets");
          samples.push_back("QCDMuEnriched");
          samples.push_back("t#bar{t}");
          step=TString("step5");
        }
      else if(*anal == "qcd")
        {
          samples.push_back("t#bar{t}");
          samples.push_back("QCD");
          step=TString("step3");
        }
      
      vector<TH1*> pt_numerator;
      vector<TH1*> eta_numerator;
      vector<TH1*> radius_numerator;
      vector<TH1*> nvtx_numerator;
      
      TH1* pt_denominator = NULL;
      TH1* eta_denominator = NULL;
      TH1* radius_denominator = NULL;
      TH1* nvtx_denominator = NULL;
      
      for(vector<TString>::iterator sample = samples.begin(); sample!=samples.end(); ++sample)
        {
          // Denominator is common (independent on tauID)
          if(!pt_denominator)     pt_denominator     = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("pt_denominator"));     else pt_denominator    ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("pt_denominator")    )); 
          if(!eta_denominator)    eta_denominator    = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("eta_denominator"));    else eta_denominator   ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("eta_denominator")   )); 
          if(!radius_denominator) radius_denominator = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("radius_denominator")); else radius_denominator->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("radius_denominator")));
          if(!nvtx_denominator)   nvtx_denominator   = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("nvtx_denominator"));   else nvtx_denominator  ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("nvtx_denominator")  ));   
        }

      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        {
          TString tcat(tauDiscriminators[l]);
          TH1* temp_pt_numerator     = NULL;
          TH1* temp_eta_numerator    = NULL;
          TH1* temp_radius_numerator = NULL;
          TH1* temp_nvtx_numerator   = NULL;
          for(vector<TString>::iterator sample = samples.begin(); sample!=samples.end(); ++sample)
            {
              if(!temp_pt_numerator)     temp_pt_numerator     = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("pt_numerator"));     else temp_pt_numerator    ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("pt_numerator")    )); 
              if(!temp_eta_numerator)    temp_eta_numerator    = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("eta_numerator"));    else temp_eta_numerator   ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("eta_numerator")   )); 
              if(!temp_radius_numerator) temp_radius_numerator = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+TString("")+tcat+TString("radius_numerator")); else temp_radius_numerator->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("radius_numerator")));
              if(!temp_nvtx_numerator)   temp_nvtx_numerator   = (TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("nvtx_numerator"));   else temp_nvtx_numerator  ->Add((TH1*) f->Get(sample->Data()+TString("/")+anal->Data()+TString("_")+step+tcat+TString("nvtx_numerator")  ));   
            }
          
          cout << "Discriminator " << tcat << " acquired via histos like " << samples[0]+TString("/")+anal->Data()+TString("_")+step+tcat+TString("nvtx_numerator")<< endl;
          if(!temp_pt_numerator    ) cout << "temp_pt_numerator     is NULL" << endl;          
          if(!temp_eta_numerator   ) cout << "temp_eta_numerator    is NULL" << endl;
          if(!temp_radius_numerator) cout << "temp_radius_numerator is NULL" << endl;
          if(!temp_nvtx_numerator  ) cout << "temp_nvtx_numerator   is NULL" << endl;
          

          temp_pt_numerator    ->Sumw2();          
          temp_eta_numerator   ->Sumw2();
          temp_radius_numerator->Sumw2();
          temp_nvtx_numerator  ->Sumw2();
          
          temp_pt_numerator    ->SetMarkerColor(10*l+l+1);
          temp_eta_numerator   ->SetMarkerColor(10*l+l+1);
          temp_radius_numerator->SetMarkerColor(10*l+l+1);
          temp_nvtx_numerator  ->SetMarkerColor(10*l+l+1);
          
          pt_numerator    .push_back(temp_pt_numerator    );
          eta_numerator   .push_back(temp_eta_numerator   );
          radius_numerator.push_back(temp_radius_numerator);
          nvtx_numerator  .push_back(temp_nvtx_numerator  );
        }
      cout << "Done." << endl;
      
      if(!pt_denominator    ) cout << " pt_denominator     is NULL" << endl; else pt_denominator    ->Sumw2();
      if(!eta_denominator   ) cout << " eta_denominator    is NULL" << endl; else eta_denominator   ->Sumw2();
      if(!radius_denominator) cout << " radius_denominator is NULL" << endl; else radius_denominator->Sumw2();
      if(!nvtx_denominator  ) cout << " nvtx_denominator   is NULL" << endl; else nvtx_denominator  ->Sumw2();
      
      
      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        {
          pt_numerator    [l]->Divide( pt_denominator     );
          eta_numerator   [l]->Divide( eta_denominator    );
          radius_numerator[l]->Divide( radius_denominator );
          nvtx_numerator  [l]->Divide( nvtx_denominator   );
        }
      
      TCanvas* c = new TCanvas("fakerate", "fakerate", 800, 800);
      c->Divide(2,2);
      c->cd(1);
      gPad->SetLogy();
      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        l==0 ? pt_numerator[l]->Draw("") : pt_numerator[l]->Draw("same");
      c->cd(2);
      gPad->SetLogy();
      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        l==0 ? eta_numerator[l]->Draw("") : eta_numerator[l]->Draw("same");
      c->cd(3);
      gPad->SetLogy();
      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        l==0 ? radius_numerator[l]->Draw("") : radius_numerator[l]->Draw("same");
      c->cd(4);
      gPad->SetLogy();
      for(size_t l=0; l<tauDiscriminators.size(); ++l)
        l==0 ? nvtx_numerator[l]->Draw("") : nvtx_numerator[l]->Draw("same");

      c->Modified();
      c->Update();
      c->Print(TString("test/results/fakes_")+anal->Data()+TString(".png"));
      
    }

}
