#ifndef HISTINFOCOLLECTIONROCLEGACYANA_H
#define HISTINFOCOLLECTIONROCLEGACYANA_H

#include "TH1.h"
#include "THStack.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TF1.h"

#include <memory>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <map>

//Class to hold TH1* with various helper functions 
class histInfo
{
public:
    std::string legName, legEntry, histFile, histName, drawOptions;
    int color, rebin;
    double nEvents, scale; 
    bool printNEvents;
    std::shared_ptr<TH1> h;

    //helper function to get histogram from file and configure its optional settings
    void retrieveHistogram()
    {
        //Open the file for this histogram
        TFile *f = TFile::Open(histFile.c_str());

        //check that the file was opened successfully
        if(!f)
        {
            printf("File \"%s\" could not be opened!!!\n", histFile.c_str());
            h = nullptr;
            return;
        }

        //get the histogram from the file
        h.reset(static_cast<TH1*>(f->Get(histName.c_str())));

        //with the histogram retrieved, close the file
        f->Close();
        delete f;

        //check that the histogram was retireved from the file successfully
        if(!h)
        {
            printf("Histogram \"%s\" could not be found in file \"%s\"!!!\n", histName.c_str(), histFile.c_str());
            return;
        }

        //set the histogram color
        h->SetLineColor(color);
        h->SetLineWidth(3);
        h->SetMarkerColor(color);
        h->SetMarkerStyle(20);
        h->Scale(scale);
        
        //Get number of events and save it as a member variable
        nEvents  = h->Integral();

        //Add number of events to legend
        if(printNEvents) 
        {
            legEntry = legName + " ("+std::to_string(int(nEvents))+") ";
        }
        else
        {
            legEntry = legName;
        }        

        // rebin the histogram if desired
        if(rebin >0) h->Rebin(rebin);
    }

    //helper function for axes
    void setupAxes(double xOffset, double yOffset, double xTitle, double yTitle, double xLabel, double yLabel)
    {
        h->SetStats(0);
        h->SetTitle(0);
        h->GetXaxis()->SetTitleOffset(xOffset);
        h->GetYaxis()->SetTitleOffset(yOffset);
        h->GetXaxis()->SetTitleSize(xTitle);
        h->GetYaxis()->SetTitleSize(yTitle);
        h->GetXaxis()->SetLabelSize(xLabel);
        h->GetYaxis()->SetLabelSize(yLabel);
        if(h->GetXaxis()->GetNdivisions() % 100 > 5) h->GetXaxis()->SetNdivisions(6, 5, 0);
    }

    //helper function for pads
    void setupPad(double left, double right, double top, double bottom)
    {
        gPad->SetLeftMargin(left);
        gPad->SetRightMargin(right);
        gPad->SetTopMargin(top);
        gPad->SetBottomMargin(bottom);
        gPad->SetTicks(1,1);
    }

    //wrapper to draw histogram
    void draw(const std::string& additionalOptions = "", bool noSame = false) const
    {
        h->Draw(((noSame?"":"same " + drawOptions + " " + additionalOptions)).c_str());
    }

    void setFillColor(int newColor = -1)
    {
        if(newColor >= 0) h->SetFillColor(newColor);
        else              h->SetFillColor(color);
    }

    void setLineStyle(int style = 1)
    {
        h->SetLineStyle(style);
    }

    histInfo(const std::string& legName, const std::string& histFile, const std::string& drawOptions, const int color, const bool printNEvents = true, const double scale = 1.0) : legName(legName), histFile(histFile), histName(""), drawOptions(drawOptions), color(color), rebin(-1), nEvents(-1), scale(scale), printNEvents(printNEvents), h(nullptr)
    {
    }

    histInfo(TH1* h) : legName(h->GetName()), histFile(""), histName(h->GetName()), drawOptions(""), color(kWhite), rebin(0), nEvents(-1), scale(1.0), printNEvents(true), h(h)
    {
    }

    ~histInfo()
    {
    }
};

class HistInfoCollection
{

public:
    std::vector<histInfo> dataVec_, bgVec_, sigVec_;

    HistInfoCollection(std::vector<histInfo>&& dataVec, std::vector<histInfo>&& bgVec, std::vector<histInfo>&& sigVec) : dataVec_(dataVec), bgVec_(bgVec), sigVec_(sigVec) {}
    HistInfoCollection(std::vector<histInfo>& dataVec, std::vector<histInfo>& bgVec, std::vector<histInfo>& sigVec) : dataVec_(dataVec), bgVec_(bgVec), sigVec_(sigVec) {}
    HistInfoCollection() {};

    ~HistInfoCollection() {}

    void setUpBG(const std::string& histName, int rebin, THStack* bgStack, std::shared_ptr<TH1>& hbgSum, const bool& setFill = true, const bool& scale = false)
    {
        double dNum = 1, bNum = 1;
        if(scale)
        {
            dNum = 0;
            bNum = 0;
            for(auto& entry : dataVec_)
            {
                entry.histName = histName;
                entry.rebin = rebin;
                entry.retrieveHistogram();            
                dNum += entry.h->Integral(); 
            }
            for(auto& entry : bgVec_)
            {
                entry.histName = histName;
                entry.rebin = rebin;
                entry.retrieveHistogram();
                bNum += entry.h->Integral();
            } 
        }
        
        bool firstPass = true;
        for(auto& entry : bgVec_)
        {
            entry.histName = histName;
            entry.rebin = rebin;
            entry.retrieveHistogram();
            entry.h->Scale(dNum/bNum);

            bgStack->Add(entry.h.get(), entry.drawOptions.c_str());
            if(firstPass) 
            {
                hbgSum.reset( static_cast<TH1*>(entry.h->Clone()) );
                firstPass = false;
            }
            else 
            {
                hbgSum->Add(entry.h.get());
            }
            
            if(setFill)
            {
                entry.setFillColor();
            }
        }
    }

    void setUpSignal(const std::string& histName, int rebin)
    {
        for(auto& entry : sigVec_)
        {
            entry.histName = histName;
            entry.rebin = rebin;
            entry.retrieveHistogram();
            entry.setLineStyle(2);    
        }
    }

    void setUpData(const std::string& histName, int rebin)
    {
        for(auto& entry : dataVec_)
        {
            entry.histName = histName;
            entry.rebin = rebin;
            entry.retrieveHistogram();
        }
    }
};

#endif
