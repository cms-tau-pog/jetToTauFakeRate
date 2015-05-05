#!/bin/bash

#TAG=hps 
#TAG=oldw 
#TAG=oldwo 
TAG=neww 
#TAG=newwo

mkdir -p cmsweek/${TAG}

cp test/results/*.pdf        cmsweek/${TAG}
cp test/results/*.png        cmsweek/${TAG}
cp test/results/plotter*root cmsweek/${TAG}

