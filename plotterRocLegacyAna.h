#ifndef PLOTTER_H
#define PLOTTER_H

#include "TH1.h"
#include "THStack.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TF1.h"
#include "HistInfoCollectionRocLegacyAna.h"

#include <memory>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <map>

class rocInfo
{
public:
    std::vector<double> rocVec;
    std::string legEntry;
    int color;
};

class Plotter
{
private:
    //Collection of histInfo
    HistInfoCollection hc_;
    std::shared_ptr<TH1> hbgSum_;
    std::map< std::string, HistInfoCollection > mhc_;
    std::vector<std::shared_ptr<TH1>> hbgSumVec_;
    std::string outpath_;

public:
    Plotter(HistInfoCollection&& hc, const std::string& outpath = "outputPlots") : hc_(hc), outpath_(outpath){}
    Plotter(std::map< std::string, HistInfoCollection >&& mhc, const std::string& outpath = "outputPlots") : mhc_(mhc), outpath_(outpath) {}

    void plotRocFisher(std::string histCut, const std::string fileName, const std::string& xAxisLabel, const std::string& yAxisLabel = "Events", const bool firstOnly = false, const bool isLogY = false, int rebin = -1, const double xmin = 999.9, const double xmax = -999.9, const std::string& year = "2016")
    {
        //This is a magic incantation to disassociate opened histograms from their files so the files can be closed
        TH1::AddDirectory(false);

        //create the canvas for the plot
        TCanvas *c = new TCanvas("c1", "c1", 800, 800);
        c->cd();

        //Set Canvas margin (gPad is root magic to access the current pad, in this case canvas "c")
        gPad->SetLeftMargin(0.12);
        gPad->SetRightMargin(0.06);
        gPad->SetTopMargin(0.08);
        gPad->SetBottomMargin(0.12);
        gPad->SetTicks(1,1);

        //Create TLegend
        TLegend *leg = new TLegend(0.13, 0.73, 0.88, 0.88);
        leg->SetFillStyle(0);
        leg->SetBorderSize(0);
        leg->SetLineWidth(1);
        leg->SetNColumns(3);
        leg->SetTextFont(42);

        //create a dummy histogram to act as the axes
        histInfo dummy(new TH1D("dummy", "dummy", 1000, 0, 1));
        dummy.draw();

        //Switch to logY if desired
        gPad->SetLogy(isLogY);

        //get maximum from histos and fill TLegend
        double min = 0.0;
        double max = 1.0;
        double lmax = 1.0;

        // --------------------------
        // -  Make Roc Info and Plot
        // --------------------------
        std::vector<rocInfo> bgSumRocInfoVec;
        std::vector<TGraph*> graphVec;
        std::string histName;
        for(auto& mhc : mhc_)
        {
            histName = histCut;
            THStack* bgStack = new THStack();
            std::shared_ptr<TH1> hbgSum;
            mhc.second.setUpBG(histName, rebin, bgStack, hbgSum, false);
            delete bgStack;
            mhc.second.setUpSignal(histName, rebin);
            rocInfo bgSumRocInfo = { makeFisherVec(hbgSum), "AllBG", mhc.second.bgVec_[0].color};
            std::vector<rocInfo> rocBgVec  = makeRocVec(mhc.second.bgVec_);
            std::vector<rocInfo> rocSigVec = makeRocVec(mhc.second.sigVec_);
            if(firstOnly) rocBgVec.emplace(rocBgVec.begin(), bgSumRocInfo);
            int lineStyle = kSolid;
            int markStyle = (mhc.first == "No 350") ?  kFullCircle : kFullSquare;
            drawRocCurve(graphVec, rocBgVec, rocSigVec, firstOnly, leg, lineStyle, markStyle);
        }

        TF1* line1 = new TF1( "line1","1",0,1);
        line1->SetLineColor(kBlack);
        line1->Draw("same");
        TF1* line2 = new TF1( "line2","x",0,1);
        line2->SetLineColor(kBlack);
        line2->SetLineStyle(kDotted);
        line2->Draw("same");
        
        //plot legend
        leg->Draw("same");

        //Draw dummy hist again to get axes on top of histograms
        setupDummy(dummy, leg, "", xAxisLabel, yAxisLabel, isLogY, xmin, xmax, min, max, lmax);
        dummy.draw("AXIS");

        //Draw CMS and lumi lables
        //std::cout<<lumi<<std::endl;
	    drawLables(year);

        TLatex mark;
        mark.SetNDC(true);

        //Draw lumistamp
        mark.SetTextFont(42);
        mark.SetTextAlign(11);
        mark.SetTextSize(0.030);
        mark.DrawLatex(gPad->GetLeftMargin() + 0.02, 1 - (gPad->GetTopMargin() + 0.05), "arXiv:XXXX.XXXXX");        

        //save new plot to file
        if(firstOnly) 
        {
            c->Print((outpath_ + "/" + fileName + ".pdf").c_str());
        }
        else
        {
            c->Print((outpath_ + "/" + fileName + ".pdf").c_str());
        }

        //clean up dynamic memory
        delete c;
        delete leg;
        for(auto* g : graphVec) delete g;
    }

    //This is a helper function which will keep the plot from overlapping with the legend
    void smartMax(const TH1* const h, const TLegend* const l, const TPad* const p, double& gmin, double& gmax, double& gpThreshMax, const bool error)
    {
        //const bool isLog = p->GetLogy();
        double min = 9e99;
        double max = -9e99;
        double pThreshMax = -9e99;
        int threshold = static_cast<int>(h->GetNbinsX()*(l->GetX1() - p->GetLeftMargin())/((1 - p->GetRightMargin()) - p->GetLeftMargin()));

        for(int i = 1; i <= h->GetNbinsX(); ++i)
        {
            double bin = 0.0;
            if(error) bin = h->GetBinContent(i) + h->GetBinError(i);
            else      bin = h->GetBinContent(i);
            if(bin > max) max = bin;
            else if(bin > 1e-10 && bin < min) min = bin;
            if(i >= threshold && bin > pThreshMax) pThreshMax = bin;
        }
        
        gpThreshMax = std::max(gpThreshMax, pThreshMax);
        gmax = std::max(gmax, max);
        gmin = std::min(gmin, min);
    }

    std::vector<double> makeFisherVec(std::shared_ptr<TH1> h)
    {
        h->Scale( 1.0 / h->Integral() );
        std::vector<double> v;
        for(int ii = 0; ii <= h->GetNbinsX(); ii++)
        {
            double val = h->Integral( ii, h->GetNbinsX());
            v.push_back( val );
        }
        return v;
    }

    std::vector<rocInfo> makeRocVec(const std::vector<histInfo>& vec)
    {
        std::vector<rocInfo> rocVec;
        for(const auto& entry : vec)
        {
            std::vector<double> v = makeFisherVec(entry.h);
            rocVec.push_back( {v, entry.legEntry, entry.color} );
        }
        return rocVec;    
}

    void drawRocCurve(std::vector<TGraph*>& graphVec, const std::vector<rocInfo>& rocBgVec, const std::vector<rocInfo>& rocSigVec, const bool firstOnly, TLegend* leg, int lineStyle, int markStyle)
    {
        int index = 0;
        for(const auto& mBg : rocBgVec)
        {
            index++;
            if(index > 2) break;
            for(const auto& mSig : rocSigVec)
            {
                int n = mBg.rocVec.size();
                std::vector<double> x(n, 0.0), y(n, 0.0);
                for(int i = 0; i < n; i++)
                {
                    x[i] = mBg.rocVec[i];
                    y[i] = mSig.rocVec[i];
                }
                TGraph* g = new TGraph (n, x.data(), y.data());
                g->SetLineWidth(3);
                g->SetLineStyle(lineStyle);
                g->SetLineColor( mSig.color );
                g->SetMarkerSize(0.0);
                g->SetMarkerStyle(markStyle);
                g->SetMarkerColor( mSig.color );
                g->Draw("same LP");                
                leg->AddEntry(g, (mSig.legEntry).c_str(), "LP");
                graphVec.push_back(g);
            }
            if(firstOnly) break; 
        }
    }
    
    void setupDummy(histInfo dummy, TLegend *leg, const std::string& histName, const std::string& xAxisLabel, const std::string& yAxisLabel, const bool isLogY, 
                    const double xmin, const double xmax, double min, double max, double lmax)
    {
        dummy.setupAxes(1.2, 1.1, 0.045, 0.045, 0.045, 0.045);
        dummy.h->GetYaxis()->SetTitle(yAxisLabel.c_str());
        dummy.h->GetXaxis()->SetTitle(xAxisLabel.c_str());
        dummy.h->SetTitle(histName.c_str());
        //Set the y-range of the histogram
        if(isLogY)
        {
            double locMin = std::min(0.2, std::max(0.2, 0.05 * min));
            double legSpan = (log10(3*max) - log10(locMin)) * (leg->GetY1() - gPad->GetBottomMargin()) / ((1 - gPad->GetTopMargin()) - gPad->GetBottomMargin());
            double legMin = legSpan + log10(locMin);
            if(log10(lmax) > legMin)
            {
                double scale = (log10(lmax) - log10(locMin)) / (legMin - log10(locMin));
                max = pow(max/locMin, scale)*locMin;
            }
            dummy.h->GetYaxis()->SetRangeUser(locMin, 10*max);
        }
        else
        {
            double locMin = 0.0;
            double legMin = (1.2*max - locMin) * (leg->GetY1() - gPad->GetBottomMargin()) / ((1 - gPad->GetTopMargin()) - gPad->GetBottomMargin());
            if(lmax > legMin) max *= (lmax - locMin)/(legMin - locMin);
            dummy.h->GetYaxis()->SetRangeUser(0.0, max*1.2);
        }
        //set x-axis range
        if(xmin < xmax) dummy.h->GetXaxis()->SetRangeUser(xmin, xmax);
    }

    void drawLables(const std::string& year)
    {
        //Draw CMS and lumi lables
        char lumistamp[128];
        //sprintf(lumistamp, "%.1f fb^{-1} (13 TeV)", lumi / 1000.0);
        sprintf(lumistamp, "%s (13 TeV)", year.c_str());

        TLatex mark;
        mark.SetNDC(true);

        //Draw CMS mark
        mark.SetTextAlign(11);
        mark.SetTextSize(0.050);
        mark.SetTextFont(61);
        mark.DrawLatex(gPad->GetLeftMargin(), 1 - (gPad->GetTopMargin() - 0.017), "CMS");
        mark.SetTextSize(0.040);
        mark.SetTextFont(52);
        mark.DrawLatex(gPad->GetLeftMargin() + 0.11, 1 - (gPad->GetTopMargin() - 0.017), "Simulation Supplementary");

        //Draw lumistamp
        mark.SetTextFont(42);
        mark.SetTextAlign(31);
        mark.DrawLatex(1 - gPad->GetRightMargin(), 1 - (gPad->GetTopMargin() - 0.017), lumistamp);        
    }    
};

#endif
