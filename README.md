# jetToTauFakeRate

Package for measurement of jet -> tau fake-rate in data and comparison to MC


Fake rate
---------

The task is to compute the fake rate for MC and data for runII.



Data-to-data comparison
-----------------------

The task is to compare data reconstructed with 53X and wioth 740, and perhaps some simple fake rate measurement.


A) Base comparison:
   * Test data in the new release: /JetHT/CMSSW_7_4_0_pre9_ROOT6-GR_R_74_V8_1Apr_RelVal_jht2012D-v1/RECO (I suppose I can run on MINIAOD from 740)
     they contain re-reco data from the following 2012D runs:
          208307
          208339
          208341
          208351
          208353
   * Reference dataset: the corresponding runs in JetHT/Run2012D-22Jan2013-v1/RECO or /JetHT/Run2012D-22Jan2013-v1/AOD 

B) Further comparison, to check OOT pileup (for these, all 2012D has been reprocessed:
   * Test data in the new release: /JetHT25ns/CMSSW_7_4_0_pre9_ROOT6-GR_R_74_V8_1Apr_RelVal_jht25ns2012D-v2/RECO
   * Reference dataset: /JetHT25ns/Run2012D-PromptReco-v1/RECO (or AOD)



Code structure:
     - 740 in master branch
     - 53X in runI branch


Todo:
        - cleaning unnecessary libraries