## Making Legacy Plots for SUS-19-004

Input ROOT files for making data vs MC stack plots and signal vs background plots, as found in Fig. 2 of the paper and in Fig. 7, 8, 9, and 10 of the supplementary material twiki, are located at:
`/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/DataVsMC`

Once copied the `DataVsMC` folder locally to the `condor/hadded` path with `Analyzer/Analyzer/test`, plots can be made using `plot_1l_LegacyAna`.

### Making Figure 2 of the Paper

```
./plot_1l_LegacyAna -y 2016 -t DataVsMC -a 1
./plot_1l_LegacyAna -y 2020 -t DataVsMC -a 1 # Here 2020 is synonymous with 2017+2018
```

### Making Figure 9 and 10 of Supplementary Material

```
./plot_1l_LegacyAna -y 2016 -t DataVsMC -a 1 -s 1
./plot_1l_LegacyAna -y 2017 -t DataVsMC -a 1 -s 1
```

### Making Figure 7 and 8 or Supplementary Material

```
python ttVsSigNNLegacyAna.py --year 2016 --approved --inputDir ./condor/hadded/DataVsMC
python ttVsSigNNLegacyAna.py --year 2017 --approved --inputDir ./condor/hadded/DataVsMC
```

### Making Figure 1 of the Supplementary Material

```
python makeBinEdgePlotLegacyAna.py --approved
```

## Making Legacy Results Plots for SUS-19-004

Input ROOT files for making the limit plots and p-value plots, found in the paper, as well as nuisance parameter plots, found in the paper and supplementary material are located at:
`/eos/uscms/store/user/lpcsusyhad/StealthStop/PlotInputs/SUS19004/LimitsAndPvalues/FullRun2_Unblinded_Jun15`. Copy the `FullRun2_Unblinded_Jun15` folder locally.

To make the NP plots, the `makeNPplotsLegacyAna.py` script is used.

### Making NP plots for Figure 8 in paper and Figure 4, 5, and 6 in Supplementary

```
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2016/output-files/RPV_400_2016/fitDiagnostics2016RPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2017/output-files/RPV_400_2017/fitDiagnostics2017RPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2018pre/output-files/RPV_400_2018pre/fitDiagnostics2018preRPV400.root --approved
python makeNPplotsLegacyAna.py ./FullRun2_Unblinded_Jun15/Fit_Data_2018post/output-files/RPV_400_2018post/fitDiagnostics2018postRPV400.root --approved
```

### Making limit plots in Figure 6 of the paper

```
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "RPV", true)'
root -b -l -q 'makeLimitPlotsLegacyAna.C("Jun15_2020", "./FullRun2_Unblinded_Jun15/Fit_Data_Combo/output-files/", "Combo", "SYY", true)'
```

### Making p-value plots in Figure 7 of the paper

```
python ./condor/table_signal_strength.py --basedir ./FullRun2_Unblinded_Jun15/ --pdfName Jun15_2020 --approved
```
