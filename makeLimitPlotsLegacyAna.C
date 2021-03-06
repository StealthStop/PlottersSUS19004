#include <iostream>
#include "TSystem.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TPaveText.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TGraphAsymmErrors.h"
#include "TGraph.h"
#include "TFrame.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLine.h"
#include "TLegend.h"

//
// Global variables
//

TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = false;
TString extraText   = "Preliminary";
float extraTextFont = 52;  // default is helvetica-italics

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
float lumiTextSize     = 0.65; //0.6;
float lumiTextOffset   = 0.2;
float cmsTextSize      = 0.9; //0.75;
float cmsTextOffset    = 0.1;  // only used in outOfFrame version

float relPosX    = 0.045;
float relPosY    = 0.045;//0.035;
float relExtraDY = 1.2;

// ratio of "CMS" and extra text size
float extraOverCmsTextSize  = 0.76;

TString lumi_13TeV = "137 fb^{-1}";
TString lumi_sqrtS = "";

bool drawLogo      = false;

void CMS_lumi( TPad* pad, int iPeriod, int iPosX, bool writeExtraText )
{            
    bool outOfFrame    = false;
    if( iPosX/10==0 ) 
    {
        outOfFrame = true;
    }
    int alignY_=3;
    int alignX_=2;
    if( iPosX/10==0 ) alignX_=1;
    if( iPosX==0    ) alignX_=1;
    if( iPosX==0    ) alignY_=1;
    if( iPosX/10==1 ) alignX_=1;
    if( iPosX/10==2 ) alignX_=2;
    if( iPosX/10==3 ) alignX_=3;
    //if( iPosX == 0  ) relPosX = 0.12;
    int align_ = 10*alignX_ + alignY_;

    float H = pad->GetWh();
    float W = pad->GetWw();
    float l = pad->GetLeftMargin();
    float t = pad->GetTopMargin();
    float r = pad->GetRightMargin();
    float b = pad->GetBottomMargin();
    //  float e = 0.025;

    pad->cd();

    TString lumiText;
    if ( iPeriod==4 )
    {
        lumiText += lumi_13TeV;
        lumiText += " (13 TeV)";
    }
    else if ( iPeriod==7 )
    { 
        if( outOfFrame ) lumiText += "#scale[0.85]{";
        lumiText += lumi_13TeV; 
        lumiText += " (13 TeV)";
        if( outOfFrame) lumiText += "}";
    }
   
    std::cout << lumiText << endl;

    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);    

    float extraTextSize = extraOverCmsTextSize*cmsTextSize;

    latex.SetTextFont(42);
    latex.SetTextAlign(31); 
    latex.SetTextSize(lumiTextSize*t);    
    latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

    if( outOfFrame )
    {
        latex.SetTextFont(cmsTextFont);
        latex.SetTextAlign(11); 
        latex.SetTextSize(cmsTextSize*t);    
        latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
    }
  
    pad->cd();

    float posX_=0;
    if( iPosX%10<=1 )
    {
        posX_ =   l + relPosX*(1-l-r);
    }
    else if( iPosX%10==2 )
    {
        posX_ =  l + 0.5*(1-l-r);
    }
    else if( iPosX%10==3 )
    {
        posX_ =  1-r - relPosX*(1-l-r);
    }
    float posY_ = 1-t - relPosY*(1-t-b);
    if( !outOfFrame )
    {
        if( drawLogo )
        {
            posX_ =   l + 0.045*(1-l-r)*W/H;
            posY_ = 1-t - 0.045*(1-t-b);
            float xl_0 = posX_;
            float yl_0 = posY_ - 0.15;
            float xl_1 = posX_ + 0.15*H/W;
            float yl_1 = posY_;
            TASImage* CMS_logo = new TASImage("CMS-BW-label.png");
            TPad* pad_logo = new TPad("logo","logo", xl_0, yl_0, xl_1, yl_1 );
            pad_logo->Draw();
            pad_logo->cd();
            CMS_logo->Draw("X");
            pad_logo->Modified();
            pad->cd();
        }
        else
        {
            latex.SetTextFont(cmsTextFont);
            latex.SetTextSize(cmsTextSize*t);
            latex.SetTextAlign(align_);
            latex.DrawLatex(posX_, posY_, cmsText);
            if( writeExtraText ) 
            {
                latex.SetTextFont(extraTextFont);
                latex.SetTextAlign(align_);
                latex.SetTextSize(extraTextSize*t);
                latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
            }
        }
    }
    else if( writeExtraText )
    {
        if( iPosX==0) 
        {
            posX_ =   l +  relPosX*(1-l-r);
            posY_ =   1-t+lumiTextOffset*t;
        }
        latex.SetTextFont(extraTextFont);
        latex.SetTextSize(extraTextSize*t);
        latex.SetTextAlign(align_);
        latex.DrawLatex(posX_, posY_, extraText);      
    }
    return;
}

void makeLimitPlotsLegacyAna(const string year = "2017", const string model = "RPV", const bool approved = false, const string fitType = "AsymptoticLimits") 
{
    // =============================================================
    TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");
  
    // For the canvas:
    tdrStyle->SetCanvasBorderMode(0);
    tdrStyle->SetCanvasColor(kWhite);
    tdrStyle->SetCanvasDefH(600); //Height of canvas
    tdrStyle->SetCanvasDefW(600); //Width of canvas
    tdrStyle->SetCanvasDefX(0);   //POsition on screen
    tdrStyle->SetCanvasDefY(0);

    // For the Pad:
    tdrStyle->SetPadBorderMode(0);
    tdrStyle->SetPadColor(kWhite);
    tdrStyle->SetPadGridX(false);
    tdrStyle->SetPadGridY(false);
    tdrStyle->SetGridColor(0);
    tdrStyle->SetGridStyle(3);
    tdrStyle->SetGridWidth(1);

    // For the frame:
    tdrStyle->SetFrameBorderMode(0);
    tdrStyle->SetFrameBorderSize(1);
    tdrStyle->SetFrameFillColor(0);
    tdrStyle->SetFrameFillStyle(0);
    tdrStyle->SetFrameLineColor(1);
    tdrStyle->SetFrameLineStyle(1);
    tdrStyle->SetFrameLineWidth(1);
  
    // For the histo:
    tdrStyle->SetHistLineColor(1);
    tdrStyle->SetHistLineStyle(0);
    tdrStyle->SetHistLineWidth(1);

    tdrStyle->SetEndErrorSize(2);
    tdrStyle->SetMarkerStyle(20);

    //For the fit/function:
    tdrStyle->SetOptFit(1);
    tdrStyle->SetFitFormat("5.4g");
    tdrStyle->SetFuncColor(2);
    tdrStyle->SetFuncStyle(1);
    tdrStyle->SetFuncWidth(1);

    //For the date:
    tdrStyle->SetOptDate(0);

    // For the statistics box:
    tdrStyle->SetOptFile(0);
    tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
    tdrStyle->SetStatColor(kWhite);
    tdrStyle->SetStatFont(42);
    tdrStyle->SetStatFontSize(0.025);
    tdrStyle->SetStatTextColor(1);
    tdrStyle->SetStatFormat("6.4g");
    tdrStyle->SetStatBorderSize(1);
    tdrStyle->SetStatH(0.1);
    tdrStyle->SetStatW(0.15);

    // Margins:
    tdrStyle->SetPadTopMargin(0.05);
    tdrStyle->SetPadBottomMargin(0.13);
    tdrStyle->SetPadLeftMargin(0.16);
    tdrStyle->SetPadRightMargin(0.03);

    // For the Global title:

    tdrStyle->SetOptTitle(0);
    tdrStyle->SetTitleFont(42);
    tdrStyle->SetTitleColor(1);
    tdrStyle->SetTitleTextColor(1);
    tdrStyle->SetTitleFillColor(10);
    tdrStyle->SetTitleFontSize(0.05);

    // For the axis titles:
    tdrStyle->SetTitleColor(1, "XYZ");
    tdrStyle->SetTitleFont(42, "XYZ");
    tdrStyle->SetTitleSize(0.06, "XYZ");
    tdrStyle->SetTitleXOffset(0.9);
    tdrStyle->SetTitleYOffset(1.25);
  
    // For the axis labels:
    tdrStyle->SetLabelColor(1, "XYZ");
    tdrStyle->SetLabelFont(42, "XYZ");
    tdrStyle->SetLabelOffset(0.007, "XYZ");
    tdrStyle->SetLabelSize(0.05, "XYZ");

    // For the axis:
    tdrStyle->SetAxisColor(1, "XYZ");
    tdrStyle->SetStripDecimals(kTRUE);
    tdrStyle->SetTickLength(0.03, "XYZ");
    tdrStyle->SetNdivisions(510, "XYZ");
    tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
    tdrStyle->SetPadTickY(1);

    // Change for log plots:
    tdrStyle->SetOptLogx(0);
    tdrStyle->SetOptLogy(0);
    tdrStyle->SetOptLogz(0);

    // Postscript options:
    tdrStyle->SetPaperSize(20.,20.);

    tdrStyle->SetHatchesLineWidth(5);
    tdrStyle->SetHatchesSpacing(0.05);

    tdrStyle->cd();

    // =============================================================


    // Settings for CMS_lumi macro
    if (!approved) writeExtraText = true;
    string extraText = "Preliminary";
    int iPeriod = 4;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV 
    if     (year=="2016")      lumi_13TeV = "35.9 fb^{-1}";
    else if(year=="2017")      lumi_13TeV = "41.5 fb^{-1}";
    else if(year=="2018pre")   lumi_13TeV = "21.1 fb^{-1}";
    else if(year=="2018post")  lumi_13TeV = "38.7 fb^{-1}";
    else if(year=="2016_2017") lumi_13TeV = "77.4 fb^{-1}";

    // second parameter in example_plot is iPos, which drives the position of the CMS logo in the plot
    // iPos=11 : top-left, left-aligned
    // iPos=33 : top-right, right-aligned
    // iPos=22 : center, centered
    // mode generally : 
    //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
    int iPos = 11;

    // **** Set the following each time before running ****
    string date = "Jan17_19";
    string ssave = "---------------------------------------------------------------";
    string ssave_base = "./sigBrLim_"+model+"_"+year+"_";
    bool DRAW_OBS = true;
    bool DRAW_LOGOS = true;

    // ****************************************************

    int W = 800;
    int H = 600;
    int W_ref = 800; 
    int H_ref = 600; 
    // references for T, B, L, R
    float T = 0.08*H_ref;
    float B = 0.13*H_ref; 
    float L = 0.13*W_ref;
    float R = 0.04*W_ref;

    gStyle->SetCanvasDefH      (H);
    gStyle->SetCanvasDefW      (W);
    gStyle->SetTitleOffset( 1, "y");

    // *****
    // Extract limit results from set of root files produced by Higgs Combine tool    
    int minFitMass =  300;
    int maxFitMass = 1200;
    int step = 50;
    std::vector<double> xpoints;
    for(int i = minFitMass-step; i < maxFitMass; xpoints.push_back(i+=step));
    const int npoints = xpoints.size();

    // Arrays for storing results
    // The following are the values of r from the fitter, where r is
    //  the number of signal events / number of expected signal events
    std::vector<double> limits_obs(npoints,0);
    std::vector<double> limits_obsErr(npoints,0);
    std::vector<double> limits_m2s(npoints,0);
    std::vector<double> limits_m1s(npoints,0);
    std::vector<double> limits_mean(npoints,0);
    std::vector<double> limits_p1s(npoints,0);
    std::vector<double> limits_p2s(npoints,0);

    // Loop over mass points
    for (int i=0; i<npoints; i++) 
    {
        const string& mass = std::to_string(int(xpoints[i]));
        const string& fitter_file = "./LimitsAndPvalues/FullRun2_Unblinded_Jun15/Fit_Data_"+year+"/output-files/"+model+"_"+mass+"_"+year+"/higgsCombine"+year+"."+fitType+".mH"+mass+".MODEL"+model+".root";
    
        std::cout << fitter_file << std::endl;
        // Load the root file and read the tree and leaves
        TFile *f = new TFile(fitter_file.c_str());

        if(f->IsZombie() ||  !f->GetListOfKeys()->Contains("limit"))
        {
            limits_m2s[i] = 0;
            limits_m1s[i] = 0;
            limits_mean[i] = 0;
            limits_p1s[i] = 0;
            limits_p2s[i] = 0;
            limits_obs[i] = 0;
            limits_obsErr[i] = 0;        
            continue;
        }

        TTreeReader reader("limit",f);
        TTreeReaderValue<double> lim(reader,"limit");
        TTreeReaderValue<double> lim_err(reader,"limitErr");

        reader.Next();
        limits_m2s[i] = *lim;
        reader.Next();
        limits_m1s[i] = *lim;
        reader.Next();
        limits_mean[i] = *lim;
        reader.Next();
        limits_p1s[i] = *lim;
        reader.Next();
        limits_p2s[i] = *lim;
        reader.Next();
        limits_obs[i] = *lim;
        limits_obsErr[i] = *lim_err;

        f->Close();
    }

    // Define the cross section times branching fraction (called sigBr here):
    std::vector<double> sigBr;
    std::vector<double> sigBr1SPpercent;
    std::vector<double> sigBr1SMpercent;
  
    // cross sections and uncertainties from
    //  https://twiki.cern.ch/twiki/bin/view/LHCPhysics/SUSYCrossSections13TeVstopsbottom
    const std::vector<double>& stop_pair_Br =           { 10.00, 4.43, 2.15, 1.11,  0.609, 0.347, 0.205, 0.125, 0.0783, 0.0500, 0.0326, 0.0216,  0.0145, 
                                                          0.00991, 0.00683, 0.00476, 0.00335, 0.00238, 0.00170, 0.00122, 0.000887, 0.000646, 0.000473,
    };
    const std::vector<double>& stop_pair_Br1SPpercent = {  6.65, 6.79, 6.99, 7.25,  7.530, 7.810, 8.120, 8.450, 8.8000, 9.1600, 9.5300, 9.9300, 10.3300, 
                                                          10.76, 11.2, 11.65, 12.12, 12.62, 13.13, 13.66, 14.21, 14.78, 15.37,
    };
    const std::vector<double>& stop_pair_Br1SMpercent = stop_pair_Br1SPpercent;

    // For stop pair production
    sigBr = stop_pair_Br;
    sigBr1SPpercent = stop_pair_Br1SPpercent;
    sigBr1SMpercent = stop_pair_Br1SMpercent;

    std::vector<double> sigBr1SP(npoints,0);
    std::vector<double> sigBr1SM(npoints,0);
    for (int i=0; i<npoints; i++) 
    {
        sigBr1SP[i] = sigBr[i]*sigBr1SPpercent[i]/100.0;
        sigBr1SM[i] = sigBr[i]*sigBr1SMpercent[i]/100.0;
    }
    double textSize = 0.050;

    bool projectingRLimitLogY = true;
    //double projectingXmin = 250, projectingXmax = 950;
    double projectingXmin = xpoints.front()-50, projectingXmax = xpoints.back()+50;
    double projectingRLimitYmin = 0.0005, projectingRLimitYmax = 100;
    std::string projectingRLimitXYtitles = ";m_{ #tilde{t}} [GeV]; 95% CL upper limit on #sigma_{#it{#tilde{t} #bar{#tilde{t}}}} [pb]";
    if (model == "RPV")
    {
        ssave = "Figure_006-a";
    } else {
        ssave = "Figure_006-b";
    }

    std::vector<double> limits_exp(npoints,0);
    for(int n=0; n<npoints; n++)
    {
        limits_m2s[n]=limits_m2s[n]*sigBr[n];
        limits_m1s[n]=limits_m1s[n]*sigBr[n];
        limits_mean[n]=limits_mean[n]*sigBr[n];
        limits_p1s[n]=limits_p1s[n]*sigBr[n];
        limits_p2s[n]=limits_p2s[n]*sigBr[n];

        limits_exp[n]=limits_mean[n];
        limits_obs[n]=limits_obs[n]*sigBr[n];
    }

    //TPaveText* pt = nullptr;
    //if (DRAW_LOGOS) 
    //{
    //    if (model=="RPV")
    //        pt = new TPaveText(0.65, 0.75, 0.6, 0.95, "ndc");
    //    else if (model=="SYY" or model=="StealthSHH")
    //        pt = new TPaveText(0.61, 0.75, 0.56, 0.95, "ndc");

    //    pt->SetBorderSize(0);
    //    pt->SetFillStyle(0);
    //    pt->SetTextAlign(12);
    //    pt->SetTextFont(42);
    //    pt->SetTextSize(textSize);
    //} 
    //else 
    //{
    //    if (model=="RPV")
    //        pt = new TPaveText(0.65, 0.75, 0.6, 0.95, "ndc");
    //    else if (model=="SYY" or model=="StealthSHH")
    //        pt = new TPaveText(0.58, 0.75, 0.53, 0.95, "ndc");

    //    pt->SetBorderSize(0);
    //    pt->SetFillStyle(0);
    //    pt->SetTextAlign(12);
    //    pt->SetTextFont(42);
    //    pt->SetTextSize(textSize);
    //}

    //if (model=="RPV")
    //    pt->AddText("pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t #tilde{#chi}^{0}_{1},  #tilde{#chi}^{0}_{1} #rightarrow jjj");
    //else if (model=="SYY")
    //    pt->AddText("pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t#tilde{S}g, #tilde{S} #rightarrow S#tilde{G}, S #rightarrow gg");
    //else if (model=="StealthSHH")
    //    pt->AddText("pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t#tilde{S}, #tilde{S} #rightarrow S#tilde{G}, S #rightarrow b#bar{b}");

    std::cout << "npoints = " << npoints << std::endl;
    for (int n=0; n<npoints; n++)
    {
        std::cout << "limitx_m2s = " << limits_m2s[n] << std::endl;
        std::cout << "limitx_m1s = " << limits_m1s[n] << std::endl;
        std::cout << "limitx_exp = " << limits_exp[n] << std::endl;
        std::cout << "limitx_p1s = " << limits_p1s[n] << std::endl;
        std::cout << "limitx_p2s = " << limits_p2s[n] << std::endl;
        std::cout << "limitx_obs = " << limits_obs[n] << std::endl;
        std::cout << "xpoints = "    << xpoints[n]    << std::endl;
        std::cout << std::endl;
    }

    TCanvas *cCanvas = new TCanvas(ssave.c_str(),"Canvas");
    TString stmp = "hframe"; stmp += ssave;
    TH1F *hframe= new TH1F(stmp, projectingRLimitXYtitles.c_str(), 1000, projectingXmin, projectingXmax);
    hframe->SetMinimum(projectingRLimitYmin);
    hframe->SetMaximum(projectingRLimitYmax);
    hframe->SetStats(0);
    hframe->SetFillStyle(1);
    hframe->Draw(" ");
        
    cCanvas->SetLogy(projectingRLimitLogY);

    TGraph *grMean = new TGraph(npoints, xpoints.data(), limits_exp.data()); 
    TGraph *grYellow = new TGraph(2*npoints);
    for(int n=0; n<npoints; n++)
    {
        grYellow->SetPoint(n, xpoints[n], limits_p2s[n]);
        grYellow->SetPoint(npoints+n, xpoints[npoints-n-1], limits_m2s[npoints-n-1]);
    }
    grYellow->SetFillColor(kOrange);
    grYellow->SetLineColor(kBlack);
    grYellow->SetLineStyle(2);
    grYellow->SetLineWidth(2);
    grYellow->Draw("f");

    TGraph *grGreen = new TGraph(2*npoints);
    for(int n=0; n<npoints; n++)
    {
        grGreen->SetPoint(n, xpoints[n], limits_p1s[n]);
        grGreen->SetPoint(npoints+n, xpoints[npoints-n-1], limits_m1s[npoints-n-1]);
    }
    grGreen->SetFillColor(kGreen+1);
    grGreen->SetLineColor(kBlack);
    grGreen->SetLineStyle(2);
    grGreen->SetLineWidth(2);
    grGreen->Draw("f");

    TLine *lineOne = new TLine(projectingXmin,1, projectingXmax, 1);
    lineOne->SetLineWidth(2);
    lineOne->SetLineStyle(1);
    lineOne->SetLineColor(kBlack);
    lineOne->Draw("same");

    grMean->SetLineWidth(2);
    grMean->SetLineStyle(2);
    grMean->SetLineColor(kBlue);
    grMean->SetMarkerSize(0);
    grMean->Draw("lp");

    TGraph* grObs = nullptr;
    if (DRAW_OBS) 
    {
        grObs=new TGraph(npoints, xpoints.data(), limits_obs.data());
        grObs->SetMarkerStyle(20);
        grObs->SetMarkerColor(kBlack);
        grObs->SetLineWidth(2);
        grObs->SetLineWidth(1);
        grObs->SetLineColor(kBlack);
        grObs->Draw("lp");
    }
    //pt->Draw();

    TPaveText *br01 = new TPaveText(0.362, 0.240, 0.357, 0.258+0.066, "ndc");
    br01->SetBorderSize(0);
    br01->SetFillStyle(0);
    br01->SetTextAlign(12);
    br01->SetTextFont(42);
    br01->SetTextSize(textSize);
    if (model=="RPV")
        br01->AddText("m_{#tilde{#chi}^{0}_{1}} = 100 GeV");
    else if (model=="SYY")
        br01->AddText("m_{#tilde{S}} = 100 GeV");
    else if (model=="StealthSHH")
        br01->AddText("m_{#tilde{S}} = 100 GeV");
    //br01->Draw("same");

    TPaveText *br1 = new TPaveText(0.15, 0.317, 0.357, 0.317+0.066, "ndc");
    br1->SetBorderSize(0);
    br1->SetFillStyle(0);
    br1->SetTextAlign(12);
    br1->SetTextFont(42);
    br1->SetTextSize(textSize);

    if (model=="RPV")
        br1->AddText("#bf{#it{#Beta}}(#tilde{t} #rightarrow t #tilde{#chi}^{0}_{1}) = 1.0");
    else if (model=="SYY")
        br1->AddText("#bf{#it{#Beta}}(#tilde{t} #rightarrow t#tilde{S}g) = 1.0");
    else if (model=="StealthSHH")
        br1->AddText("#bf{#it{#Beta}}(#tilde{t} #rightarrow t#tilde{S}) = 1.0");

    br1->Draw("same");

    TPaveText *br20 = new TPaveText(0.362, 0.215, 0.357, 0.215+0.066, "ndc");
    br20->SetBorderSize(0);
    br20->SetFillStyle(0);
    br20->SetTextAlign(12);
    br20->SetTextFont(42);
    br20->SetTextSize(textSize);
    if (model=="RPV")
        br20->AddText("");
    else if (model=="SYY")
        br20->AddText("m_{#tilde{G}} = 1 GeV, m_{S} = 90 GeV");
    else if (model=="StealthSHH")
        br20->AddText("m_{#tilde{G}} = 1 GeV, m_{S} = 90 GeV");
    //br20->Draw("same");

    TPaveText *br2 = new TPaveText(0.15, 0.240, 0.357, 0.240+0.066, "ndc");
    br2->SetBorderSize(0);
    br2->SetFillStyle(0);
    br2->SetTextAlign(12);
    br2->SetTextFont(42);
    br2->SetTextSize(textSize);
    if (model=="RPV")
        br2->AddText("#bf{#it{#Beta}}(#tilde{#chi}^{0}_{1} #rightarrow jjj) = 1.0");
    else if (model=="SYY")
        br2->AddText("#bf{#it{#Beta}}(#tilde{S} #rightarrow S#tilde{G}) = 1.0, #bf{#it{#Beta}}(S #rightarrow gg) = 1.0");
    else if (model=="StealthSHH")
        br2->AddText("#bf{#it{#Beta}}(#tilde{S} #rightarrow S#tilde{G}) = 1.0, #bf{#it{#Beta}}(S #rightarrow bb) = 1.0");

    br2->Draw("same");

    TPaveText *br3 = new TPaveText(0.15, 0.16, 0.357, 0.16+0.066, "ndc");
    br3->SetBorderSize(0);
    br3->SetFillStyle(0);
    br3->SetTextAlign(12);
    br3->SetTextFont(42);
    br3->SetTextSize(textSize);
    if (model=="RPV")
        br3->AddText("m_{#tilde{#chi}^{0}_{1}} = 100 GeV");        
    if (model=="SYY")
        br3->AddText("m_{#tilde{S}} = 100 GeV, m_{#tilde{G}} = 1 GeV, m_{S} = 90 GeV");
    if (model=="StealthSHH")
        br3->AddText("m_{#tilde{S}} = 100 GeV, m_{#tilde{G}} = 1 GeV, m_{S} = 90 GeV");
    br3->Draw("same");

    if (DRAW_LOGOS) 
    {
        cCanvas->SetLeftMargin( L/W );
        cCanvas->SetRightMargin( R/W );
        cCanvas->SetTopMargin( T/H );
        cCanvas->SetBottomMargin( B/H );
    }

    TGraphAsymmErrors *grTheoryErr = new TGraphAsymmErrors(npoints,xpoints.data(),sigBr.data(),nullptr,nullptr,sigBr1SM.data(),sigBr1SP.data());
    grTheoryErr->SetLineColor(2);
    grTheoryErr->SetLineWidth(2);
    grTheoryErr->SetFillColor(42);
    TGraph *grTheory = new TGraph(npoints,xpoints.data(),sigBr.data());
    grTheory->SetLineColor(2);
    grTheory->SetLineWidth(2);

    string header = "";
    if (model=="RPV")
        header = "pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t #tilde{#chi}^{0}_{1},  #tilde{#chi}^{0}_{1} #rightarrow jjj";
    else if (model=="SYY")
        header = "pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t#tilde{S}g, #tilde{S} #rightarrow S#tilde{G}, S #rightarrow gg";
    else if (model=="StealthSHH")
        header = "pp #rightarrow #tilde{t} #bar{#tilde{t}}, #tilde{t} #rightarrow t#tilde{S}, #tilde{S} #rightarrow S#tilde{G}, S #rightarrow b#bar{b}";

    TLegend *legend = nullptr;

    if (model=="RPV")
        legend = new TLegend(0.50, 0.58, 0.95, 0.88);
    else if (model=="SYY" or model=="StealthSHH")
        legend = new TLegend(0.40, 0.58, 0.85, 0.88);

    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->SetTextAlign(12);
    legend->SetTextFont(42);
    legend->SetHeader(header.c_str());
    legend->SetTextSize(textSize);
    legend->AddEntry(grGreen,"68% expected", "fl");
    legend->AddEntry(grYellow,"95% expected", "fl");
    if(DRAW_OBS) legend->AddEntry(grObs,"Observed limit", "lp");

    if (model=="RPV")
        legend->AddEntry(grTheoryErr,"#sigma_{#tilde{t} #bar{#tilde{t}}} (NNLO+NNLL)", "lf");
    else if (model=="SYY")
        legend->AddEntry(grTheoryErr,"#sigma_{#tilde{t} #bar{#tilde{t}}} (NNLO+NNLL)", "lf");
    else if (model=="StealthSHH")
        legend->AddEntry(grTheoryErr,"#sigma_{#tilde{t} #bar{#tilde{t}}} (NNLO+NNLL)", "lf");

    legend->Draw();

    grTheoryErr->Draw("3,same");
    grTheory->Draw("l,same");

    // redraw mean, so that it appears over the signal lines
    grMean->Draw("lp");

    // redraw obs, so that it appears over the expected lines
    if (DRAW_OBS) grObs->Draw("lp");

    lineOne->Delete();

    cCanvas->cd();

    CMS_lumi(cCanvas,iPeriod,iPos,writeExtraText);
    cCanvas->Update();
    cCanvas->RedrawAxis();
    cCanvas->GetFrame()->Draw();

    TPaveText *cmstext = nullptr;
    if (!DRAW_LOGOS) 
    {
        TPaveText* cmstext = new TPaveText(0.308789, 0.958188, 0.806516, 0.996516, "ndc");
        cmstext->SetBorderSize(0);
        cmstext->SetFillStyle(0);
        cmstext->SetTextAlign(12);
        cmstext->SetTextFont(42);
        cmstext->SetTextSize(0.035);

        if      (year=="2016")     cmstext->AddText("CMS Preliminary, #sqrt{s}=13 TeV, L_{Int}=35.9 fb^{-1}");
        else if (year=="2017")     cmstext->AddText("CMS Preliminary, #sqrt{s}=13 TeV, L_{Int}=41.5 fb^{-1}");
        else if (year=="2018pre")  cmstext->AddText("CMS Preliminary, #sqrt{s}=13 TeV, L_{Int}=21.1 fb^{-1}");
        else if (year=="2018post") cmstext->AddText("CMS Preliminary, #sqrt{s}=13 TeV, L_{Int}=38.7 fb^{-1}");    
        else if (year=="Combo")    cmstext->AddText("CMS Preliminary, #sqrt{s}=13 TeV, L_{Int}=35.9 + 41.5 + 21.1 + 38.7 fb^{-1}");
        cmstext->Draw("same");
    }

    std::string approvalStr = "";
    if (not approved) approvalStr = "_prelim";

    std::string sroot = "PlotsForLegacyAna/Paper/"+ssave+".root";
    std::string spdf  = "PlotsForLegacyAna/Paper/"+ssave+".pdf";
    cCanvas->Print(sroot.c_str());
    cCanvas->Print(spdf.c_str());

}
