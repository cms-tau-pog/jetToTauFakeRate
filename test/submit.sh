#!/bin/bash

JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples.json
OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results

mkdir -p ${OUTDIR}

runLocalAnalysisOverSamples.py -e runTauFakesStudy -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh

