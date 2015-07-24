#!/bin/bash

TAG=hps 
#TAG=oldw 
#TAG=oldwo 
#TAG=neww 
#TAG=newwo

rm -r devel/
mkdir -p devel/${TAG}

cp test/results_spring15/*.pdf        devel/${TAG}
cp test/results_spring15/*.png        devel/${TAG}
cp test/results_spring15/plotter*root devel/${TAG}

WEBDIR="${HOME}/www/13TeV_tauFakes_spring15/"
mkdir -p ${WEBDIR}
cp ~/www/HIG-13-026/index.php devel/
cp ~/www/HIG-13-026/index.php devel/${TAG}
rm -r ${WEBDIR}/devel/
cp -r devel/ ${WEBDIR}
