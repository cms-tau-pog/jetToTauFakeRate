#!/bin/bash

JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results

# cleanup
### rm -r ${OUTDIR}
# recreate
mkdir -p ${OUTDIR}

runLocalAnalysisOverSamples.py -e runTauFakesStudy -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh

