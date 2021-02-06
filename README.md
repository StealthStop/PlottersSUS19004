## Setup Area for Running

Get a release of CMSSW

```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv

git clone https://github.com/StealthStop/PlottersSUS19004
cd PlottersSUS19004

make -j 4
```

## Making Legacy Plots for SUS-19-004 Paper

Input ROOT files for making the legacy plots for the paper and supplementary material are located at:

```
/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/
```

Copy from this location the following folders for plotting:
```
DataVsMC         # Fig. 2 of the paper and in Fig. 7, 8, 9, and 10 of the supplementary material
LimitsAndPvalues # Fig. 6, 7, 8 of the paper and Fig. 4, 5, 6
Fits             # Fig. 4 of the paper
Training         # Fig. 2, 3 of the supplementary material
QCDCR            # Fig. 3 of the paper
```
Before continuing, make an output folder that everything will go to

```
mkdir -p PlotsForLegacyAna/{Paper,Supplementary}
```

### Making Figure 2

```
./makeStackPlotLegacyAna -y 2016 -a 1
./makeStackPlotLegacyAna -y 2020 -a 1 # Here 2020 is synonymous with 2017+2018
```

### Making Figure 3

```
python makeDataDrivenQcdCrPlotsLegacyAna.py
```

### Making Figure 4

```
python makeFitPlotsLegacyAna.py --bkgonly --twosigfit
```

### Making Figure 5

```
python makeNjetsStackLegacyAna.py --approved
```

### Making Figure 6

```
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "RPV", true)'
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "SYY", true)'
```

### Making Figure 7

```
python makePvaluesPlotLegacyAna.py --approved 
```

### Making Figure 8

```
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/RPV_400_Combo/fitDiagnosticsComboRPV400.root --approved
```

## Making Legacy Plots for SUS-19-004 Supplementary

### Making Figure 1

```
python makeBinEdgePlotLegacyAna.py --approved
```

### Making Figure 2

```
./makeRocPlotLegacyAna -y 2016
./makeRocPlotLegacyAna -y 2017
```

### Making Figure 3

```
python makeSNNwithWithoutGRPlotsLegacyAna.py --approved --withGR
python makeSNNwithWithoutGRPlotsLegacyAna.py --approved
```

### Making Figure 4, 5, and 6

```
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_2016/output-files/RPV_400_2016/fitDiagnostics2016RPV400.root --approved
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_2017/output-files/RPV_400_2017/fitDiagnostics2017RPV400.root --approved
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_2018pre/output-files/RPV_400_2018pre/fitDiagnostics2018preRPV400.root --approved
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_2018post/output-files/RPV_400_2018post/fitDiagnostics2018postRPV400.root --approved
```
### Making Figure 7 and 8

```
python makettVsSigNNLegacyAna.py --year 2016 --approved
python makettVsSigNNLegacyAna.py --year 2017 --approved
```

### Making Figure 9 and 10

```
./makeStackPlotLegacyAna -y 2016 -a 1 -s 1
./makeStackPlotLegacyAna -y 2017 -a 1 -s 1
```
