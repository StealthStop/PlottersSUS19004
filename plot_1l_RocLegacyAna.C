#include "plotterRocLegacyAna.h"
#include <iostream>
#include <getopt.h>

class FisherHolder
{
public:
    std::vector<std::string> cutNames_;
    std::string plotName_;
};

void setHistInfo(const std::string& path, std::vector<histInfo>& data, std::vector<histInfo>& bg, std::vector<histInfo>& sig, const std::string& year)
{
    std::vector<int> bgColor  = {kRed, kBlack, kBlue};
    std::vector<int> sigColor = {kBlack, kBlue, kRed};

    bg = {
        {"Triboson",        path + "/"+year+"_Triboson.root",        "hist", kGray       , false},
        {"Diboson",         path + "/"+year+"_Diboson.root",         "hist", kMagenta + 1, false},
        {"DYJetsToLL_M-50", path + "/"+year+"_DYJetsToLL_M-50.root", "hist", kOrange + 2 , false},        
        {"TTX",             path + "/"+year+"_TTX.root",             "hist", kCyan + 1   , false},
        {"WJets",           path + "/"+year+"_WJets.root",           "hist", kYellow + 1 , false},
        {"ST",              path + "/"+year+"_ST.root",              "hist", kRed + 1    , false},
        {"QCD",             path + "/"+year+"_QCD.root",             "hist", kGreen + 1  , false},
        {"T#bar{T}",        path + "/"+year+"_TT.root",              "hist", kBlue - 6   , false},
    };

    sig = {        
        {"RPV 300", path + "/"+year+"_RPV_2t6j_mStop-300.root",      "hist", kRed    ,   false},
        {"RPV 450", path + "/"+year+"_RPV_2t6j_mStop-450.root",      "hist", kGreen+2,   false},        
        {"RPV 550", path + "/"+year+"_RPV_2t6j_mStop-550.root",      "hist", kBlack  ,   false},        
        {"RPV 650", path + "/"+year+"_RPV_2t6j_mStop-650.root",      "hist", kMagenta+2, false},        
        {"RPV 850", path + "/"+year+"_RPV_2t6j_mStop-850.root",      "hist", kOrange+1,  false},
        {"RPV 1200", path + "/"+year+"_RPV_2t6j_mStop-1200.root",    "hist", kBlue,      false},
    };
}

int main(int argc, char *argv[])
{
    TH1::AddDirectory(false);

    int opt, option_index = 0;    
    std::string year = "2017";

    static struct option long_options[] = {
        {"year",     required_argument, 0, 'y'},
    };

    while((opt = getopt_long(argc, argv, "y:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
        case 'y': year = optarg; break;
        }
    }

    std::string subfig = "";
    std::string path;
    if     (year=="2016")     
    {
        path= "Training/RocPlots/Analyze1Lep_2016_v1.2";
        subfig = "a";
    } else if(year=="2017")     
    { 
        path= "Training/RocPlots/Analyze1Lep_2017_v1.2";
        subfig = "b";
    } else if(year=="2018pre")  
    {
        path= "Training/RocPlots/Analyze1Lep_2018pre_v1.2";
    } else if(year=="2018post") 
    {
        path= "Training/RocPlots/Analyze1Lep_2018post_v1.2";
    }
    std::vector<histInfo> data, bg, sig;
    setHistInfo(path, data, bg, sig, year);
    HistInfoCollection histInfoCollection(data, bg, sig);

    // vector of histInfoCollection for Roc Curves
    std::map< std::string, HistInfoCollection > rocMap = {{year, histInfoCollection}};

    //make plotter object with the required sources for histograms specified
    Plotter pltRoc( std::move(rocMap) ,"PlotsForLegacyAna/Supplementary/");

    std::vector<std::string> mycuts_1l = {
    	"_1l_HT300_ge7j_ge1b_Mbl",
    };

    for(std::string mycut : mycuts_1l)
    {
        pltRoc.plotRocFisher("h_deepESM"+mycut, "CMS-SUS-19-004_Figure-aux_002-" + subfig, "Background Efficiency","Signal Efficiency", true, false, -1, 999.9, -999.9, year);        
    }
}
