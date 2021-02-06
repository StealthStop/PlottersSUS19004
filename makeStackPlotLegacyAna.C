#include "plotterLegacyAna.h"
#include <iostream>
#include <getopt.h>

// This plotting script is setup to make Fig. 2 of the SUS-19-004 paper
// as well as the supplementary plots in Fig. 9 and 10 found on the twiki page
// https://twiki.cern.ch/twiki/bin/view/CMS/SUS19004SupplementaryMaterial

class FisherHolder
{
public:
    std::vector<std::string> cutNames_;
    std::string plotName_;
};

void setHistInfo(const std::string& path, std::vector<histInfo>& data, std::vector<histInfo>& bg, std::vector<histInfo>& sig, std::vector<histInfo>& syst, std::vector<histInfo>& rsyst, const std::string& year, const bool supplementary = false, const bool doQCD = false)
{
    //entry for data
    //this uses the initializer syntax to initialize the histInfo object
    //               leg entry root file                 draw options  draw color
    data = {
        {"Data" , path + "/"+year+"_Data.root", "PEX0", kBlack},
    };
    
    if (!doQCD) {
        bg = {
            {"t#bar{t} + X", path + "/"+year+"_TTX.root",   "hist", kOrange + 2  , 1.0, true, 0},
            {"QCD multijet", path + "/"+year+"_QCD.root",   "hist", kGreen + 1   , 1.0, true, 0},
            {"Other",        path + "/"+year+"_Other.root", "hist", kMagenta + 2 , 1.0, true, 0},        
            {"t#bar{t}",     path + "/"+year+"_TT.root",    "hist", kBlue - 6    , 1.0, true, 0},
        };
    } else {
        bg = {
            {"t#bar{t} + X", path + "/"+year+"_TTX.root",   "hist", kOrange + 2  , 1.0, true, 0},
            {"Other",        path + "/"+year+"_Other.root", "hist", kMagenta + 2 , 1.0, true, 0},        
            {"t#bar{t}",     path + "/"+year+"_TT.root",    "hist", kBlue - 6    , 1.0, true, 0},
            {"QCD multijet", path + "/"+year+"_QCD.root",   "hist", kGreen + 1   , 1.0, true, 0},
        };
    }

    if (not supplementary)
    {
        sig = {        
            {"RPV m_{#tilde{t}} = 450 GeV (#sigma_{#tilde{t} #bar{#tilde{t}}} #times 4)",          path + "/"+year+"_RPV_2t6j_mStop-450.root",        "hist", kRed,    4.0,  false, 2},
            {"Stealth SYY m_{#tilde{t}} = 850 GeV (#sigma_{#tilde{t} #bar{#tilde{t}}} #times 16)", path + "/"+year+"_StealthSYY_2t6j_mStop-850.root", "hist", kCyan+1, 16.0, false, 9},        
        };
    } else {
        sig = {        
            {"RPV m_{#tilde{t}} = 450 GeV",         path + "/"+year+"_RPV_2t6j_mStop-450.root",        "hist", kRed,  1.0, false, 2},
            {"Stealth SYY m_{#tilde{t}} = 850 GeV", path + "/"+year+"_StealthSYY_2t6j_mStop-850.root", "hist", kCyan+1,     1.0, false, 9},        
        };
    }

    if (!doQCD) {
        syst = {
            {"SYST",   path + "/"+year+"_MC_Syst_wNormUncSept14.root", "hist", kBlack, 1.0, true, 0},
        };

        rsyst = {
            {"RSYST",  path + "/"+year+"_MC_Ratio_Syst_wNormUncSept14.root", "hist", kBlack, 1.0, true, 0},
        };
    }
}

int main(int argc, char *argv[])
{
    TH1::AddDirectory(false);

    int opt, option_index = 0;    
    std::string year = "2018post";
    std::string directory = "DataVsMC";
    int approved = 0;
    int supplementary = 0;

    static struct option long_options[] = {
        {"year",      required_argument, 0, 'y'},
        {"directory", required_argument, 0, 't'},
        {"approved",  optional_argument, 0, 'a'},
        {"supplementary",  optional_argument, 0, 's'},
    };

    while((opt = getopt_long(argc, argv, "y:t:a:s:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
            case 't': directory = optarg; break;
            case 'y': year = optarg; break;
            case 'a': approved = std::stoi(optarg); break;
            case 's': supplementary = std::stoi(optarg); break;
        }
    }

    std::string path = directory + "/" + year;

    std::vector<histInfo> data, bg, sig, syst, rsyst;
    setHistInfo(path, data, bg, sig, syst, rsyst, year, supplementary);
    HistInfoCollection histInfoCollection(data, bg, sig, syst, rsyst);

    std::string loc;
    if (supplementary == 0)
    {
        loc = "Paper";
    } else {
        loc = "Supplementary";
    }      

    Plotter plt(std::move(histInfoCollection), "PlotsForLegacyAna/" + loc + "/");
    //make plotter object with the required sources for histograms specified

    std::string fig;
    std::string subfig;
    double lumi = 0.0;
    if      (year == "2016")
    {    
        lumi = 35900.0;
        fig = "009";
        subfig = "a";
    } else if (year == "2017")     
    {
        lumi = 41500.0;
        fig = "010";
        subfig = "b";
    } else if (year == "2018pre")  
    {
        lumi = 21100.0;
    } else if (year == "2018post") 
    {
        lumi = 38700.0;
    } else if (year == "2020")     
    {
        lumi = 101300.0;
        fig = "010";
        subfig = "b";
    }

    // --------------------
    // - Make stack plots
    // --------------------

    if (supplementary == 0)
    {
        plt.plotStack("h_deepESM_1l_HT300_ge7j_ge1b_Mbl",  "Figure_002-" + subfig, "S_{NN}" ,            "Events",        false, 10, true, true, 999.9, -999.9, lumi, false, approved);
    } else {
        plt.plotStack("Jet_cm_pt_1_1l_ge7j_ge1b",   "CMS-SUS-19-004_Figure-aux_"+fig+"-a", "Leading Jet p_{T} [GeV]", "Events / 30 GeV", true,   3, true, true, 0,  1500, lumi, supplementary, approved);
        plt.plotStack("Jet_cm_m_1_1l_ge7j_ge1b",    "CMS-SUS-19-004_Figure-aux_"+fig+"-b", "Leading Jet Mass [GeV]",  "Events / 5 GeV",  false,  5, true, true, 0,  200,  lumi, supplementary, approved);
        plt.plotStack("Jet_cm_phi_1_1l_ge7j_ge1b",  "CMS-SUS-19-004_Figure-aux_"+fig+"-c", "Leading Jet #phi",        "Events / bin",    false,  1, true, true, -4, 4,    lumi, supplementary, approved);
        plt.plotStack("Jet_cm_eta_1_1l_ge7j_ge1b",  "CMS-SUS-19-004_Figure-aux_"+fig+"-d", "Leading Jet #eta",        "Events / bin",    false,  1, true, true, -6, 6,    lumi, supplementary, approved);
        plt.plotStack("jmt_ev0_top6_1l_ge7j_ge1b",  "CMS-SUS-19-004_Figure-aux_"+fig+"-e", "JMT0", "Events / bin", false, 1, true, true, 999.0, -999.9, lumi, supplementary, approved);
        plt.plotStack("jmt_ev1_top6_1l_ge7j_ge1b",  "CMS-SUS-19-004_Figure-aux_"+fig+"-f", "JMT1", "Events / bin", false, 1, true, true, 999.0, -999.9, lumi, supplementary, approved);
        plt.plotStack("fwm2_top6_1l_ge7j_ge1b",     "CMS-SUS-19-004_Figure-aux_"+fig+"-g", "FWM2", "Events / bin", false, 1, true, true, 999.0, -999.9, lumi, supplementary, approved);
        plt.plotStack("fwm4_top6_1l_ge7j_ge1b",     "CMS-SUS-19-004_Figure-aux_"+fig+"-h", "FWM4", "Events / bin", false, 1, true, true, 999.0, -999.9, lumi, supplementary, approved);
    }
}
