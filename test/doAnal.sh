#!/bin/bash

# Main analysis script.

# 1: run the analysis (must merge submit script here)

JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/samples_wjet.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/qcd.json
### JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/wjetsonly.json
#JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples_all.json#

QUEUE=1nh
OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/2015fakes/
#QUEUE=8nm
#OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/2015fakes8nm/


BASEWEBDIR=~/www/13TeV_tauFakes_25ns


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
    
    STARTINGJSON="data/cert/Cert_246908-258750_13TeV_PromptReco_Collisions15_25ns_JSON.txt"
    sed -i -e "s#}{#,#g"  qcd_lumi.json; 
    sed -i -e "s#, ,#,#g" qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#,,#,#g"  qcd_lumi.json;
    sed -i -e "s#{,#{#g"  qcd_lumi.json;

    sed -i -e "s#}{#,#g"  wjet_lumi.json; 
    sed -i -e "s#, ,#,#g" wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#,,#,#g"  wjet_lumi.json;
    sed -i -e "s#{,#{#g"  wjet_lumi.json;

    export PATH=$HOME/.local/bin:/afs/cern.ch/cms/lumi/brilconda-1.0.3/bin:$PATH 
    ### Official v1 brilcalc lumi --normtag /afs/cern.ch/user/c/cmsbril/public/normtag_json/OfflineNormtagV1.json -i myjson.json
    brilcalc lumi --normtag ~lumipro/public/normtag_file/OfflineNormtagV2.json  -i wjet_lumi.json
    echo "DONE WJET"
    brilcalc lumi --normtag ~lumipro/public/normtag_file/OfflineNormtagV2.json  -i qcd_lumi.json
    echo "DONE QCD"
    echo "Take care. This uses the offline tag V2, which is not yet blessed by Physics Coordinators https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/544.html "
    echo "To be compared with the output of the full json:"
    echo "brilcalc lumi --normtag ${STARTINGJSON}"
    brilcalc lumi --normtag ~lumipro/public/normtag_file/OfflineNormtagV2.json -i ${STARTINGJSON}
    echo "DONE FULL"
    echo "Take care. This uses the offline tag V2, which is not yet blessed by Physics Coordinators https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/544.html "

    exit 0
    JSONFILE=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/data/data_samples.json
    OUTDIR=$CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/results_lumi
    # cleanup (comment it out if you have smaller jsons for running only on a few sets while the others are OK
    # rm -r ${OUTDIR}
    # recreate
    mkdir -p ${OUTDIR}
    runAnalysisOverSamples.py -e extractLumiJSON -j ${JSONFILE} -o ${OUTDIR} -d  /dummy/ -c $CMSSW_BASE/src/TauAnalysis/JetToTauFakeRate/test/runAnalysis_cfg.py.templ -p "@useMVA=False @saveSummaryTree=False @runSystematics=False @automaticSwitch=False @is2011=False @jacknife=0 @jacks=0" -s 8nh

elif [ "${1}" = "plot" ]; then
    DIR="${BASEWEBDIR}/"
    mkdir -p ${DIR}
    mkdir -p ~/www/temptemp/
    mv ${DIR}*vischia*pdf ~/www/temptemp/
    rm -r ${DIR}*
    mv ~/www/temptemp/* ${DIR}
    cp ~/www/HIG-13-026/index.php ${DIR}

    # Different tests
    LUMIWJETS=2136
    LUMIQCD=2136
    LUMIWJETS=307
    LUMIQCD=277
    LUMIWJETS=336
    LUMIQCD=729

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
    runFixedPlotter --iEcm 13 --forceMerge --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYWJETS}  &

    # QCD
    runFixedPlotter --iEcm 13 --forceMerge --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYQCD}   &

    DIR="${BASEWEBDIR}_split/"
    PLOTTERWJETS=${DIR}plotter_wjet.root
    PLOTTERQCD=${DIR}plotter_qcd.root
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
    runFixedPlotter --iEcm 13 --iLumi ${LUMIWJETS} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERWJETS} --json ${JSONFILEWJETS} --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYWJETS}  &

    # QCD
    runFixedPlotter --iEcm 13 --iLumi ${LUMIQCD} --inDir ${INDIR} --outDir ${DIR} --outFile ${PLOTTERQCD}   --json ${JSONFILEQCD}   --cutflow all_initNorm --no2D --noPowers ${PLOTEXT} ${ONLYQCD}   &


 
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

elif [ "${1}" = "harvest" ]; then
    # Fix. --plotExt does not really impact (extensions are multiple and hardcoded)
    # Configurable input directory
    runFakeRate --inDir ${BASEWEBDIR}/ --outDir fakerate --plotExt .png

fi

# Done!
exit 0

