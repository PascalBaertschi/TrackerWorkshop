# PixelResolution

Exercises for producing pixel resolution plots

##Setup
In order to be able to run the codes run:
```
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh
```

## Producing pixel resolution plots for single run

The output files from the PixelTriplet code (https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets), run for example on CRAB are required. An example run from Run3 is provided for the exercise.

FitAndPlot.C produces the pixel resolution plots, there is a python wrapper available for easier use:
```
python runFits.py -i 361512.root -f Template/Generic -r BPIX/FPix
```
Choose between Template or Generic and BPix and FPix.

## Producing trend plots

If a list of runs is given as input to the code it saves the results as json, which then can be used for trend plots. An example from Run3 with the points merged with bins of 0.5 finv is provided in trend-plotter/inputs.

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
