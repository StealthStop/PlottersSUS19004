#!/bin/python
import sys
import copy
import os.path
import ROOT

ROOT.gStyle.SetOptTitle( 0 )
ROOT.gStyle.SetOptStat( 0 )
ROOT.gROOT.SetBatch( ROOT.kTRUE )

from optparse import OptionParser

parser = OptionParser()

parser.add_option( '-i', '--inputFileName', action = 'store', 
                    dest = 'inputFileName', type = 'string', 
                    default = 'qcdcr_study.root', help = 'Name of input ROOT file with the histograms needed to create Figure 3' )

parser.add_option( '-o', '--outputDirName', action = 'store', 
                    dest = 'outputDirName', type = 'string', 
                    default = 'Figure3', help = 'Name of output directory' )

(options, args) = parser.parse_args()


# Define some global arrays that will be used later in the plotting script
yearList                    = [ "2016", "2017", "2018pre", "2018post" ]
njetList                    = [ "7j", "8j", "9j", "10j", "11j" ]
borderSize                  = 0.25

def main() :

    # Check if input file exists
    if not os.path.exists( options.inputFileName ) :
        print( "Input ROOT file does not exist. Exiting now." )
        return 0
    
    # Check if output directory exists - if not, make output directory
    outputDir                   = options.outputDirName
    if not os.path.exists( outputDir ):
        print( "Output directory does not exist, so making output directory:", outputDir)
        os.makedirs( outputDir )
    
   
    inputRootFile                   = ROOT.TFile.Open( options.inputFileName )
    
    for year in yearList :
        lumi                        = getLumi( year )

        for njet in njetList :
            c1, p1                  = createCanvasAndPad()
            dummyHist               = makeDummyHistogram()

            qcdHist                 = ROOT.TGraphAsymmErrors()
            ttHist                  = ROOT.TGraphAsymmErrors()
            dataHist                = ROOT.TGraphAsymmErrors() 
            
            qcdHist                 = inputRootFile.Get( year+"_gr_inv_qcd_"+njet )
            ttHist                  = inputRootFile.Get( year+"_gr_inv_tt_"+njet )
            dataHist                = inputRootFile.Get( year+"_gr_inv_dat_"+njet )

            dummyHist.Draw("HIST")

            qcdHist.Draw("P SAME")
            ttHist.Draw("P SAME")
            dataHist.Draw("P SAME")

            # Create and draw the legend.
            l1                          = ROOT.TLegend( 0.41, 0.70, 0.85, 0.85 )
            l1.SetTextSize( 0.05 )
            l1.SetNColumns( 2 )
            l1.AddEntry( qcdHist, "QCD_{CR}^{MC}", "pl" )
            l1.AddEntry( ttHist, "t#bar{t}_{ SR}^{ MC}", "pl")
            l1.AddEntry( dataHist, "Data_{CR}", "pl" )
            l1.SetBorderSize(0)

            l1.Draw()

            #Add CMS, Preliminary, Luminosity, and N_jets label
            mark                        = ROOT.TLatex()
            mark.SetNDC( ROOT.kTRUE )
            mark.SetTextAlign( 11 )
            mark.SetTextSize( 0.055 )
            mark.SetTextFont( 61 )
            mark.DrawLatex( ROOT.gPad.GetLeftMargin(), 1- (ROOT.gPad.GetTopMargin() - 0.02), "CMS")
            
            #mark.SetTextSize( 0.044 )
            #mark.SetTextFont( 52 )
            #mark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.135, 0.91, "Preliminary")
            
            mark.SetTextFont( 42 )
            mark.SetTextAlign( 31 )
            mark.SetTextSize( 0.045 )
            mark.DrawLatex( 1 - ROOT.gPad.GetRightMargin(), 1 - (ROOT.gPad.GetTopMargin() - 0.02), lumi+" fb^{-1} (13 TeV)" )

            newMark                 = ROOT.TLatex()
            newMark.SetNDC( ROOT.kTRUE )
            newMark.SetTextSize( 0.045 )
            newMark.SetTextFont( 62 )
            newMark.SetTextAlign( 11 )
            newMark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.045, 0.225, "N_{jets} = "+njet[0:-1] )

            c1.Update()
            c1.SaveAs(outputDir+"/"+year+"_ratio_"+njet+"_qcd_cr_temp.pdf")

            del dummyHist

def makeDummyHistogram():
    
    dummyHist                   = ROOT.TH1D( "dummyHist", "dummyHist", 7, -.1, 1.1 )
    dummyHist.GetYaxis().SetRangeUser( 0.0, 2.0 )
    
    dummyHist.SetXTitle( "S_{NN}" )
    dummyHist.GetXaxis().SetLabelSize( 0.045 )
    dummyHist.GetXaxis().SetLabelFont( 42 )
    dummyHist.GetXaxis().SetTitleSize( 0.055 )
    dummyHist.GetXaxis().SetTitleFont( 42 )
    
    dummyHist.SetYTitle( "R_{M}" )
    dummyHist.GetYaxis().SetLabelSize( 0.045 )
    dummyHist.GetYaxis().SetLabelFont( 42 )
    dummyHist.GetYaxis().SetTitleSize( 0.055 )
    dummyHist.GetYaxis().SetTitleFont( 42 )
    dummyHist.GetYaxis().SetTitleOffset( 1.15 )
    
    dummyHist.SetStats( 0 )
    
    return dummyHist

def createCanvasAndPad() :

    c1                       = ROOT.TCanvas( "c1", "c1", 0, 0, 400, 400 )
    c1.cd()
    p1                       = ROOT.TPad( "p1", "p1", 0, 0, 1.0, 1.0 )
    p1.Draw()
    p1.cd()
    
    p1.SetFrameFillStyle( 1001 )
    p1.SetTicks()
    p1.SetFillColor( 0 )
    
    p1.SetLeftMargin( 0.15 )
    p1.SetTopMargin( 0.1 )
    p1.SetBottomMargin( 0.16 )
    p1.SetRightMargin( 0.10 )

    return c1, p1

def getLumi( year ):

    lumi                        = "0.0"
    if year == "2016" :
        lumi                    = "35.9"
    elif year == "2017" :
        lumi                    = "41.5"
    elif year == "2018pre" :
        lumi                    = "21.1"
    elif year == "2018post" :
        lumi                    = "38.7"

    return lumi

if __name__ == "__main__" :
    main()
