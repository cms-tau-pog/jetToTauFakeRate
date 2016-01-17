#!/bin/bash

# Main analysis script.

# 1: run the analysis (must merge submit script here)

JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjetsonly.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples_all.json#

QUEUE=1nh
OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/2015fakes/
#QUEUE=8nm
#OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/2015fakes8nm/

if [ "${1}" = "submit" ]; then
    # cleanup (comment it out if you have smaller jsons for running only on a few sets while the others are OK
    ### rm -r ${OUTDIR}
    # recreate
    mkdir -p ${OUTDIR}
    
    runAnalysisOverSamples.py -e runTauFakesStudy -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s ${QUEUE}
    
elif [ "${1}" = "lumi" ]; then
    rm qcd_lumi.json
    rm wjet_lumi.json
    cat ${OUTDIR}/*JetHT*json > qcd_lumi.json
    cat ${OUTDIR}/*SingleMuon*json > wjet_lumi.json

    sed -i -e "s#}{#, #g"  qcd_lumi.json; 
    sed -i -e "s#, ,#, #g" qcd_lumi.json;

    sed -i -e "s#}{#, #g"  wjet_lumi.json; 
    sed -i -e "s#, ,#, #g" wjet_lumi.json;


    echo "Files qcd_lumi.json and wjet_lumi.json were regenerated."
    echo "Now running brilcalc according to the luminosity group recommendation:"
    echo "brilcalc lumi -i qcd_lumi.json -n 0.962"
    export PATH=$HOME/.local/bin:/opt/brilconda/bin:$PATH    
    brilcalc lumi -i qcd_lumi.json -n 0.962
    echo "brilcalc lumi -i wjet_lumi.json -n 0.962"
    export PATH=$HOME/.local/bin:/opt/brilconda/bin:$PATH    
    brilcalc lumi -i wjet_lumi.json -n 0.962
    echo "To be compared with the output of the full json:"
    echo "brilcalc lumi -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-254349_13TeV_PromptReco_Collisions15_JSON_v2.txt -n 0.962"
    brilcalc lumi -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-254349_13TeV_PromptReco_Collisions15_JSON_v2.txt -n 0.962

    exit 0
    JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
    OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results_lumi
    # cleanup (comment it out if you have smaller jsons for running only on a few sets while the others are OK
    # rm -r ${OUTDIR}
    # recreate
    mkdir -p ${OUTDIR}
    runAnalysisOverSamples.py -e extractLumiJSON -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh

elif [ "${1}" = "plot" ]; then
    DIR=~/www/13TeV_tauFakes_25ns/
    mkdir -p ${DIR}
    mkdir -p ~/www/temptemp/
    mv ${DIR}*vischia*pdf ~/www/temptemp/
    rm -r ${DIR}*
    mv ~/www/temptemp/* ${DIR}
    cp ~/www/HIG-13-026/index.php ${DIR}
    
    LUMIWJETS=2136
    LUMIQCD=2136

    JSONFILEWJETS=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjets_samples.json
    JSONFILEQCD=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd_samples.json
    #INDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results/
    INDIR=${OUTDIR}
    PLOTTERWJETS=${DIR}plotter_wjet.root
    PLOTTERQCD=${DIR}plotter_qcd.root
    ONLYWJETS="--onlyStartWith wjet"
    ONLYQCD="--onlyStartWith qcd"
    PLOTEXT=" --plotExt .png --plotExt .pdf --plotExt .C "
    
    ## Create plotter files from which the ratio for fake rate will be computed
    # WJets
    runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYWJETS}

    # QCD
    runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYQCD}

    DIR=~/www/13TeV_tauFakes_25ns_split/
    mkdir -p ${DIR}
    mkdir -p ~/www/temptemp/
    mv ${DIR}*vischia*pdf ~/www/temptemp/
    rm -r ${DIR}*
    mv ~/www/temptemp/* ${DIR}
    cp ~/www/HIG-13-026/index.php ${DIR}
    JSONFILEWJETS=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjets_ttsplit_samples.json
    JSONFILEQCD=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd_ttsplit_samples.json

    ## Create plotter files from which the ratio for fake rate will be computed
    # WJets
    runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYWJETS}

    # QCD
    runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYQCD}


 
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

