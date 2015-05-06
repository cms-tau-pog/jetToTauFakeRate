#!/bin/bash

DIR=~/www/13TeV_tauFakes/
mkdir -p ${DIR}
mkdir -p ~/www/temptemp/
mv ${DIR}*vischia*pdf ~/www/temptemp/
rm -r ${DIR}*
mv ~/www/temptemp/* ${DIR}
cp ~/www/HIG-13-026/index.php ${DIR}

LUMIWJETS=700
LUMIQCD=4
JSONFILEWJETS=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjets_samples.json
JSONFILEQCD=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd_samples.json
INDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results/
PLOTTERWJETS=${INDIR}plotter_wjet.root
PLOTTERQCD=${INDIR}plotter_qcd.root
ONLYWJETS="--onlyStartsWith wjet"
ONLYQCD="--onlyStartsWith qcd"


## Create plotter files from which the ratio for fake rate will be computed
# WJets
runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYWJETS}
# QCD
runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYQCD}

# Now run test/harvest.sh by hand please


exit 0
# Lessen the burden on the web browser
for CHAN in qcd wjet
do
    mkdir ${DIR}temp${CHAN}/
    mv ${DIR}${CHAN}* ${DIR}temp${CHAN}/
    mv ${DIR}temp${CHAN}/ ${DIR}${CHAN}/
    cp ~/www/HIG-13-026/index.php ${DIR}${CHAN}/
done
