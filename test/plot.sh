#!/bin/bash

DIR=~/www/13TeV_tauFakes/
mkdir -p ${DIR}
rm -rf ${DIR}*
cp ~/www/HIG-13-026/index.php ${DIR}

LUMI=1000
JSONFILEWJETS=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjets_samples.json
JSONFILEQCD=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd_samples.json
INDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results/
PLOTTERWJETS=${INDIR}plotter_wjet.root
PLOTTERQCD=${INDIR}plotter_qcd.root
ONLYWJETS=""
ONLYQCD=""


## Create plotter files from which the ratio for fake rate will be computed
# WJets
runFixedPlotter --iEcm 13 --iLumi ${LUMI} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYWJETS}
# QCD
runFixedPlotter --iEcm 13 --iLumi ${LUMI} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYQCD}

## Now compute the rates
runFakeRate --infile plotter.root --outdir blah



# Lessen the burden on the web browser

for CHAN in emu ee mumu singlemu singlee
do
    mkdir ${DIR}temp${CHAN}/
    mv ${DIR}${CHAN}* ${DIR}temp${CHAN}/
    mv ${DIR}temp${CHAN}/ ${DIR}${CHAN}/
    cp ~/www/HIG-13-026/index.php ${DIR}${CHAN}/
done