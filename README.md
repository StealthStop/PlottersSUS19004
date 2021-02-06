## Setup Area for Running

Get a release of CMSSW

```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv
```

Then run `make` to compile the `plot_1l_LegacyAna.C` script

## Making Legacy Plots for SUS-19-004 Paper

Input ROOT files for making data vs MC stack plots and signal vs background plots, as found in Fig. 2 of the paper and in Fig. 7, 8, 9, and 10 of the supplementary material twiki, are located at:
`/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/DataVsMC`

Once copied the `DataVsMC` folder locally to the `condor/hadded` path with `Analyzer/Analyzer/test`, plots can be made using `plot_1l_LegacyAna`.

Input ROOT files for making the limit plots and p-value plots, found in the paper, as well as nuisance parameter plots, found in the paper and supplementary material are located at:
`/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/LimitsAndPvalues/`.
Copy the `LimitsAndPvalues` folder locally.
Additionally, one can copy the `Fits` folder from `/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/Fits`

`mkdir PlotsForLegacyAna/{Paper,Supplementary}`

### Making Figure 2 of the Paper

```
./plot_1l_LegacyAna -y 2016 -t DataVsMC -a 1
./plot_1l_LegacyAna -y 2020 -t DataVsMC -a 1 # Here 2020 is synonymous with 2017+2018
```

### Making Figure 3 of the Paper

Make sure you copy the qcdcr\_study.root file from EOS, and then run the following commands
```
python makeDataDrivenQcdCrPlotsLegacyAna.py
```

### Making Figure 4 of the Paper

You will need to run the fit results formatting using a special script (#JOSH - please elucidate here). 
Then, the plots are made by running:

```
python makeFitPlotsLegacyAna.py --bkgonly --twosigfit
```

### Making Figure 5 of the paper

```
python njetsStackLegacyAna.py --approved --inputDir ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/
```

### Making limit plots in Figure 6 of the paper

```
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "RPV", true)'
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "SYY", true)'
```

### Making p-value plots in Figure 7 of the paper

```
python makePvaluesPlotLegacyAna.py --basedir ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/ --approved 
```

### Making NP pulls plot in Figure 8 of the paper

```
python makeNPplotsLegacyAna.py ./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/RPV_400_Combo/fitDiagnosticsComboRPV400.root --approved
```

## Making Legacy Plots for SUS-19-004 Supplementary

### Making Figure 1 of the Supplementary Material

```
python makeBinEdgePlotLegacyAna.py --approved
```

### Making Figure 2 of the Supplementary Material

```
./plot_1l_RocLegacyAna -y 2016
./plot_1l_RocLegacyAna -y 2017

```

### Making Figure 3 of the Supplementary Material

```
python makeSNNwithWithoutGRPlotsLegacyAna.py --approved --withGR
python makeSNNwithWithoutGRPlotsLegacyAna.py --approved
```

### Making NP plots for Figure 4, 5, and 6 in Supplementary

```
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2016/output-files/RPV_400_2016/fitDiagnostics2016RPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2017/output-files/RPV_400_2017/fitDiagnostics2017RPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2018pre/output-files/RPV_400_2018pre/fitDiagnostics2018preRPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2018post/output-files/RPV_400_2018post/fitDiagnostics2018postRPV400.root --approved
```
### Making Figure 7 and 8 or Supplementary Material

```
python ttVsSigNNLegacyAna.py --year 2016 --approved --inputDir ./DataVsMC
python ttVsSigNNLegacyAna.py --year 2017 --approved --inputDir ./DataVsMC
```

### Making Figure 9 and 10 of Supplementary Material

```
./plot_1l_LegacyAna -y 2016 -t DataVsMC -a 1 -s 1
./plot_1l_LegacyAna -y 2017 -t DataVsMC -a 1 -s 1
```
