#!/bin/bash

TAG=hps 
#TAG=oldw 
#TAG=oldwo 
#TAG=neww 
#TAG=newwo

mkdir -p devel/${TAG}

cp test/results/*.pdf        devel/${TAG}
cp test/results/*.png        devel/${TAG}
cp test/results/plotter*root devel/${TAG}

WEBDIR="${HOME}/www/13TeV_tauFakes/"
mkdir -p ${WEBDIR}
cp ~/www/HIG-13-026/index.php devel/
cp ~/www/HIG-13-026/index.php devel/${TAG}
rm -r ${WEBDIR}/devel/
cp -r devel/ ${WEBDIR}
