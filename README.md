# PixelResolution

Exercises for producing pixel resolution plots

## Setup
In order to be able to run the codes first run:
```
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh
```
## PixelTriplet code

The PixelTriplet code (https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets) will be used. It requires CMSSW, you can follow the setup:
```
export SCRAM_ARCH=slc7_amd64_gcc900
cmsrel CMSSW_12_4_10
cd CMSSW_12_4_10/src
cmsenv
git clone https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets.git SiPixelTools/PixelTriplets
scram b -j 8
cd SiPixelTools/PixelTriplets/

```

You can run the code localy with cmsRun:
```
cmsRun pxl_BPIX_FPIX_genIBC.py
```
In the code you can change ``inputFiles`` and ``outputFile``. Use ``/Muon/Run2022C-SiPixelCalSingleMuonTight-PromptReco-v1/ALCARECO`` for this exercise. If you check the config of the production you see that the global tag used was ``124X_dataRun3_Prompt_v4``. Changed this is in the ``pxl_BPIX_FPIX_genIBC.py`` code. One example root file to test locally would be  ``root://eoscms.cern.ch///eos/cms/store/data/Run2022C/Muon/ALCARECO/SiPixelCalSingleMuonTight-PromptReco-v1/000/356/530/00000/a645e716-b4cc-427b-b404-e4b3296db733.root``. 

The code is best run with CRAB. An example for a CRAB submit file is included. Change the following lines:
```
config.General.requestName
config.Data.inputDataset
config.Data.outputDatasetTag
config.Data.outLFNDirBase
config.Site.storageSite
```
As input dataset you can use: ``/Muon/Run2022C-SiPixelCalSingleMuonTight-PromptReco-v1/ALCARECO``. ``storageSite`` needs the tier3 you have access to.
Because running CRAB jobs is not feasible in this tutorial an example output file is provided.


## Producing pixel resolution plots for single run

The output files from the PixelTriplet code (https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets), run for example on CRAB are required. An example run from Run3 is provided for the exercise.

FitAndPlot.C fits the distributions with a student t-fit and produces the pixel resolution plots. There is a python wrapper available for easier use, ``runFits.py``. It takes as input the folder where the root files with the runs are stored. If you only wish to process one run also give the run number, otherwise the code is run on all runs in the input folder and stores the results in json files which can later be used to produce trend plots.
Because the root file is saved in the same directory as the code you can simply run:

```
python runFits.py --input . --run 361512
```

## Producing trend plots

In order to produce trend plots the fit code from the previous exercise must be run on all runs from the time range which should be shown in the trend plots. An example from Run3 with the points merged with bins of 0.5 finv is provided in trend-plotter/inputs. In the same directory are also json files with runs where the HV, the calibration or the gain was changed. These changes have an impact on the pixel resolution and are therefore visualized as lines in the plots.

The trend plotter (https://gitlab.cern.ch/paconnor/trend-plotter) is used. First compile libTrend:

```
 g++ -g -Wall -O3 -std=c++1z -fPIC -DDEBUG $(root-config --cflags) -shared Trend.cc Trend.h $(root-config --libs) -o libTrend.so
```

Compile the code with:
```
make FPIX_LA
make BPIX_LA
```

Then the trend plots for FPIX or BPIX can be run with:
```
./FPIX_LA
./BPIX_LA
```


optional exercise:

For the trend plots the lumi per run is required. First one needs to get the luminosity of all 2022 runs using brilcalc. If you are not familiar with brilcalc follow the setup here: http://opendata.cern.ch/docs/cms-guide-luminosity-calculation
```
brilcalc lumi --amodetag PROTPHYS -u /fb --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_BRIL.json --begin "01/01/22 00:00:00" --end "12/31/22 23:59:59" --output-style csv > stable2022.csv
```
The ``stable2022.csv`` file is also provided. In order to get a more suitable format ``skimLumiFile.py`` can be used:

``
python skimLumiFile.py --inputFiles stable2022.csv --outputFile deliveredLumiRun3.txt`--lumiType delivered
``