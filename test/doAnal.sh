#!/bin/bash

# Main analysis script.

# 1: run the analysis (must merge submit script here)

if [ "${1}" = "submit" ]; then
    JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples.json
    #JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples_all.json
    OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results_spring15/

    # cleanup (comment it out if you have smaller jsons for running only on a few sets while the others are OK
    ### rm -r ${OUTDIR}
    # recreate
    mkdir -p ${OUTDIR}
    
    runAnalysisOverSamples.py -e runTauFakesStudy -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh
    
elif [ "${1}" = "lumi" ]; then
    echo "PLEASE USE the lcr2 script." 
    exit 0
    JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
    OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results_lumi
    # cleanup (comment it out if you have smaller jsons for running only on a few sets while the others are OK
    # rm -r ${OUTDIR}
    # recreate
    mkdir -p ${OUTDIR}
    runAnalysisOverSamples.py -e extractLumiJSON -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh

elif [ "${1}" = "plot" ]; then
    DIR=~/www/13TeV_tauFakes_spring15/
    mkdir -p ${DIR}
    mkdir -p ~/www/temptemp/
    mv ${DIR}*vischia*pdf ~/www/temptemp/
    rm -r ${DIR}*
    mv ~/www/temptemp/* ${DIR}
    cp ~/www/HIG-13-026/index.php ${DIR}
    
    LUMIWJETS=700
    # should be 470
    LUMIQCD=4
    # should be 309
    JSONFILEWJETS=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjets_samples.json
    JSONFILEQCD=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd_samples.json
    #INDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results/
    INDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results_spring15/
    PLOTTERWJETS=${DIR}plotter_wjet.root
    PLOTTERQCD=${DIR}plotter_qcd.root
    ONLYWJETS="--onlyStartsWith wjet"
    ONLYQCD="--onlyStartsWith qcd"
    
    ## Create plotter files from which the ratio for fake rate will be computed
    # WJets
    runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYWJETS}
    runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .pdf ${ONLYWJETS}
    # QCD
    runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .png ${ONLYQCD}
    runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --forceMerge --no2D --noPowers --plotExt .pdf ${ONLYQCD}
 
    # Now run test/harvest.sh by hand please
    # root -l -b bin/macros/plotFR.C  and sh set.sh. Currently coding this step into runFakeRate.cc
    exit 0
    # Lessen the burden on the web browser
    for CHAN in qcd wjet
    do
        mkdir ${DIR}temp${CHAN}/
        mv ${DIR}${CHAN}* ${DIR}temp${CHAN}/
        mv ${DIR}temp${CHAN}/ ${DIR}${CHAN}/
        cp ~/www/HIG-13-026/index.php ${DIR}${CHAN}/
    done
fi

# Done!
exit 0

