#!/bin/python
import sys
import copy
import ROOT
import argparse 

ROOT.gStyle.SetOptTitle( 0 )
ROOT.gStyle.SetOptStat( 0 )
ROOT.gROOT.SetBatch( ROOT.kTRUE )

parser = argparse.ArgumentParser()
parser.add_argument( '--plotbkg', action = 'store_true', dest = 'bkgdfit', default = False, help = 'Plot the background component to the fit' )
parser.add_argument( '--twosigfit', action = 'store_true', dest = 'twosigfit', default = False, help = 'Plot SYY 850 signal in the fit' )
parser.add_argument( '--bkgonlyfit', action = 'store_true', dest = 'bkgonlyfit', default = False, help = 'Plot background only fit results' )
parser.add_argument( '--compshapes', action = 'store_true', dest = 'compshapes', default = False, help = 'Plot individual shapes of bkgds' )
parser.add_argument( '--mass1', dest = 'mass1', default = "450", help = 'Set mass for first reference signal' )
parser.add_argument( '--mass2', dest = 'mass2', default = "850", help = 'Set mass for second reference signal' )
parser.add_argument( '--model1', dest = 'model1', default = "RPV", help = 'Set model for first reference signal' )
parser.add_argument( '--model2', dest = 'model2', default = "SYY", help = 'Set model for second reference signal' )

parser.add_argument( '--path', dest = 'path', default="./Fits/", help = 'Input path' )

args = parser.parse_args()

#Define some global arrays that will be used later in the plotting script
snnBinList                  = [ "D1", "D2", "D3", "D4" ]
textArray                   = [ "S_{NN,1}", "S_{NN,2}", "S_{NN,3}", "S_{NN,4}" ]
borderSize                  = 0.20
yearList                    = [ ("Combo16","Y16_"), ("Combo17", "Y17_"), ("Combo18pre", "Y18pre_"), ("Combo18post", "Y18post_") ]
lumi                        = 0.0

#Variables for pads that are derived from the border size
pad1and4Size                = 1.0+borderSize
pad2and3Size                = 1.0
totalPadSize                = 2*pad1and4Size + 2*pad2and3Size

def main() :

    for year, prefix in yearList :
        #Name of root file that takes Owen's output code and makes the histogram
        fitType = "b" if args.bkgonlyfit else "s"
        inputRootFileName           = args.path+"/KELVIN_RPV"+args.mass1+year+fitType+".root"
        inputRootFile               = ROOT.TFile.Open( inputRootFileName )
    
        if year == "2016" or year == "Combo16" :
            lumi                    = "35.9"
        elif year == "2017" or year == "Combo17" :
            lumi                    = "41.5"
        elif year == "2018pre" or year == "Combo18pre" :
            lumi                    = "21.1"
        elif year == "2018post" or year == "Combo18post" :
            lumi                    = "38.7"
    
        c1, topPadArray, ratioPadArray = makeCanvasAndPads( year, prefix )
        c1, topPadArray, ratioPadArray = formatCanvasAndPads( c1, topPadArray, ratioPadArray )
    
        #Arrays needed for easy plotting (save a copy of all legends and fit histograms derived
        #    from the graphs)
        legendArray1                = []
        legendArray2                = []
        fitHistArray                = []
        lineArray                   = []
       
        dummyPullHist, dummyPullHist_D4 = makeDummyPullHistograms()
    
        yearText = ""; filename = ""
        if year == "Combo16" :
            yearText = "2016"
            filename = "Figure_004-a.pdf"
        if year == "Combo17" :
            yearText = "2017"
            filename = "Figure_004-b.pdf"
        if year == "Combo18pre" :
            yearText = "2018A"
            filename = "Figure_004-c.pdf"
        if year == "Combo18post" :
            yearText = "2018B" 
            filename = "Figure_004-d.pdf"

        outfile = ROOT.TFile.Open("PlotsForLegacyAna/Paper/%s"%(filename.replace(".pdf",".root")), "RECREATE")
        for itBin in xrange( 0, len(snnBinList) ):    
    
            #Define the name of the bin, which is used in the naming convention of the root file
            snnBin                      = prefix+snnBinList[ itBin ]
    
            sigRefHist1                 = inputRootFile.Get( "sigRefHist1_"+snnBin ); sigRefHist1.SetName( "Signal_%s%s_Ref1_"%(args.model1,args.mass1)+snnBin ); sigRefHist1.SetTitle( "Signal_%s%s_Ref1_"%(args.model1,args.mass1)+snnBin )
            sigRefHist2                 = inputRootFile.Get( "sigRefHist2_"+snnBin ); sigRefHist2.SetName( "Signal_%s%s_Ref2_"%(args.model2,args.mass2)+snnBin ); sigRefHist2.SetTitle( "Signal_%s%s_Ref2_"%(args.model2,args.mass2)+snnBin )
            sigHist                     = inputRootFile.Get( "sigHist_"+snnBin );     sigHist.SetName( "Fit_Signal_Component_"+snnBin );                          sigHist.SetTitle( "Fit_Signal_Component_"+snnBin )
            ttHist                      = inputRootFile.Get( "ttHist_"+snnBin )
            qcdHist                     = inputRootFile.Get( "qcdHist_"+snnBin )
            ttxHist                     = inputRootFile.Get( "ttxHist_"+snnBin )
            otherHist                   = inputRootFile.Get( "otherHist_"+snnBin )
            bkgdHist                    = inputRootFile.Get( "bkgdHist_"+snnBin )
            fitGraph                    = inputRootFile.Get( "Fit_"+snnBin );         fitGraph.SetName( "Total_Fit_"+snnBin ); fitGraph.SetTitle( "Total_Fit_"+snnBin )
            dataGraph                   = inputRootFile.Get( "Nobs_"+snnBin );        dataGraph.SetName( "Nobs_"+snnBin );     dataGraph.SetTitle( "Nobs_"+snnBin )
            fitHist                     = ROOT.TH1D( "fitHist_"+snnBinList[itBin], "fitHist_"+snnBinList[itBin], 6, 0, 6 )
    
            #Make the fit graph (TGraphAsymmErrors object) into a histogram object so that axes can be changed accordingly (can also use dummy histogram here).
            #Bands will be drawn using the TGraphAsymmErrors, so the bin error here is just for some consistency ( but is not really used ).
            for itHistBin in xrange( 0, fitGraph.GetN() ) :
                fitHist.SetBinContent( itHistBin + 1, fitGraph.GetY()[itHistBin] )
                fitHist.SetBinError( itHistBin + 1, fitGraph.GetEYhigh()[itHistBin] )
            
            #Fit colors determined by previous iteration of the code.
            fitHist.SetLineColor( 4 )
            fitHist.SetLineWidth( 2 )
            
            #Special care has to be done to the first bit since it defines the y-axis
            #TO DO: Parameters have not been fully made a function of the border size yet (or the aspect ratio)
            if itBin == 0 :
                fitHist.SetYTitle( "Events" )
                fitHist.SetTitleSize( 0.075, "y" )
                fitHist.SetTitleOffset( 1.3, "y" )
                fitHist.SetLabelSize( 0.065, "y" )
           
            #Save a verstion of the fitHist for plotting later
            fitHistArray.append( copy.deepcopy( fitHist ) )
            

            if( args.compshapes ): 
                #Range has been set by previous analysis plots to be between 0.05 and 1e5
                fitHistArray[itBin].GetYaxis().SetRangeUser( 0.05, 1e7 )
            else:
                fitHistArray[itBin].GetYaxis().SetRangeUser( 0.05, 1e6 )
   
            #This is the graphs for where the pulls are calculated
            pullsGraph                  = inputRootFile.Get( "pulls_"+snnBin )
            pullsErrGraph               = inputRootFile.Get( "pullsErr_"+snnBin )
            
            pullsGraph.SetLineWidth( 1 )
            pullsErrGraph.SetLineWidth( 1 )
    
            #Begin plotting for the top pad for each NN bin.
            topPadArray[itBin].cd()
    
            fitHistArray[itBin].Draw( "HIST" ) #Hist plot necessary to get the right borders
            fitGraph.Draw( "2 SAME" ) #Need to be drawn next to get any asymmetric errors (though they seem symmetric in spot checks)
            fitGraph.Write()
            fitHistArray[itBin].Draw( "HIST SAME" ) #Hist plot again to make the right order for plotting
            ROOT.gPad.RedrawAxis()
    
            #Draw the TText for the naming of the NN bin. Size and location is dependent on border size
            ttext                       = ROOT.TLatex()
            if itBin == 0:
                ttext.SetTextSize( 0.099*pad2and3Size/pad1and4Size )
                ttext.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.1, fitHistArray[itBin].GetMaximum()/5.0, textArray[itBin] )
            elif itBin == len(snnBinList) - 1 :
                ttext.SetTextSize( 0.105*pad2and3Size/pad1and4Size )
                ttext.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.3, fitHistArray[itBin].GetMaximum()/5.0, textArray[itBin] )
            else:
                ttext.SetTextSize( 0.095 )
                ttext.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.3, fitHistArray[itBin].GetMaximum()/5.0, textArray[itBin] ) 

            #Add CMS Preliminary (work in progress)
            mark                        = ROOT.TLatex()
            mark.SetNDC( ROOT.kTRUE )
            mark.SetTextAlign( 11 )

            if itBin == 0 :
                mark.SetTextSize( 0.065*pad1and4Size/pad2and3Size )
                mark.SetTextFont( 60 )
                mark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.015, 0.91, "CMS")
                mark.SetTextSize( 0.044*pad1and4Size/pad2and3Size )
                mark.SetTextFont( 52 )
                mark.SetTextSize( 0.055*pad1and4Size/pad2and3Size )
                mark.SetTextFont( 62 )
                mark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.025, 0.21, yearText+" data")

            if itBin == len(snnBinList) - 1:
                mark.SetTextFont( 42 )
                mark.SetTextSize( 0.065*pad1and4Size/pad2and3Size )
                mark.SetTextAlign( 31 )
                mark.DrawLatex( 1 - ROOT.gPad.GetRightMargin(), 0.91, lumi+" fb^{-1} (13 TeV)" )
    
            #Draw and save the legend on in the S_{NN,4} bin.
            if itBin == len(snnBinList) - 1 :
                
                l1_yStart                   = 0.71
                l2_yStart                   = 0.55

                if( args.bkgdfit ):
                    l1_yStart               = l1_yStart - 0.25
                if( args.twosigfit ) :
                    l1_yStart               = l1_yStart - 0.20
    
                l1                          = ROOT.TLegend( 0.22, l1_yStart, 0.80-(borderSize)/2, 0.85 )
                l2                          = ROOT.TLegend( 0.22, l2_yStart, 0.80-(borderSize)/2, 0.70 )

                l1.SetTextSize(0.08*pad2and3Size/pad1and4Size)
                l2.SetTextSize(0.08*pad2and3Size/pad1and4Size)
           
                l1.AddEntry( fitHist, "Bkg Fit" )
                l1.AddEntry( dataGraph, "N observed", "pl" )
                if( args.bkgonlyfit ) :
                    l1.AddEntry( sigRefHist1, args.model1+" m_{#tilde t} = "+args.mass1+" GeV" )
                else :
                    l1.AddEntry( sigHist, "Fit Signal" )
                if( args.twosigfit ) :
                    l1.AddEntry( sigRefHist2, args.model2+" m_{#tilde t} = "+args.mass2+" GeV" )
                if( args.bkgdfit ) :
                    l1.AddEntry( bkgdHist, "Bkgd" )
                if( args.compshapes ):
                    l2.SetNColumns(2)
                    l2.AddEntry(ttHist, "TT")
                    l2.AddEntry(qcdHist, "QCD")
                    l2.AddEntry(ttxHist, "TTX")
                    l2.AddEntry(otherHist, "OTHER")

                l1.SetBorderSize(0)
                l2.SetBorderSize(0)
                
                l1.Draw()
    
                if( args.compshapes ):
                    l2.Draw()

            outfile.cd()

            dataGraph = SetEx( dataGraph, 0.0 )
            dataGraph.Draw( "P SAME" ); dataGraph.Write()
            if( args.bkgdfit ) :
                bkgdHist.Draw( "HIST SAME" ); bkgdHist.Write()
                fitHistArray[itBin].Draw( "HIST SAME" ); fitHistArray[itBin].Write()
            if( args.bkgonlyfit ) :
                sigRefHist1.Draw( "HIST SAME" ); sigRefHist1.Write()
            else :
                sigHist.Draw( "HIST SAME" ); sigHist.Write()
            if( args.twosigfit ) :
                sigRefHist2.Draw( "HIST SAME" ); sigRefHist2.Write()
            if( args.compshapes ):
                ttHist.Draw("HIST SAME")
                qcdHist.Draw("HIST SAME")
                ttxHist.Draw("HIST SAME")
                otherHist.Draw("HIST SAME")

            #Draw pulls here
            ratioPadArray[itBin].cd()
              
            myline = ROOT.TLine( 0.0, 0.0, 6.0, 0.0 ) 
            myline.SetLineWidth( 1 )
            lineArray.append( copy.deepcopy( myline ) )
            
            if itBin == len(snnBinList) - 1:
                dummyPullHist_D4.Draw()
            else:
                dummyPullHist.Draw()
            
            pullsErrGraph.Draw("2 SAME")
            ROOT.gPad.RedrawAxis()
            lineArray[-1].Draw("SAME")
            dummyPullHist.Draw("AL SAME")
            pullsGraph = SetEx( pullsGraph, 0.0 )
            pullsGraph.Draw("P SAME")
            
            c1.Update()
    
        c1.SaveAs("PlotsForLegacyAna/Paper/%s"%(filename))
        outfile.Close()

def SetEx(  inputTGraphAsymmErrors, uniformErrSize ):
    nPoints = inputTGraphAsymmErrors.GetN()
    for i in xrange( nPoints ):
        inputTGraphAsymmErrors.SetPointEXhigh( i, uniformErrSize )
        inputTGraphAsymmErrors.SetPointEXlow( i, uniformErrSize )
    return inputTGraphAsymmErrors

def makeDummyPullHistograms():
    
    dummyPullHist                   = ROOT.TH1D( "dummyPullHist", "dummyPullHist", 6, 0, 6 )
    dummyPullHist.GetYaxis().SetRangeUser( -3.5, 3.5 )
    dummyPullHist.SetMinimum( -3.5 )
    dummyPullHist.SetMaximum( 3.5 )
    dummyPullHist.SetYTitle( "(data - fit) / #delta" )
    dummyPullHist.SetTitleSize( 0.175, "y" )
    dummyPullHist.SetTitleOffset( 0.5, "y" )
    dummyPullHist.SetLabelOffset( 0.025, "x" )
    dummyPullHist.SetLabelOffset( 0.025, "y" )
    dummyPullHist.SetLabelSize( 0.24, "x" )
    dummyPullHist.SetLabelSize( 0.145, "y" )
    dummyPullHist.SetNdivisions( 404, "y" )
    dummyPullHist.SetStats( 0 )
    
    dummyPullHist_D4                   = ROOT.TH1D( "dummyPullHist_D4", "dummyPullHist_D4", 6, 0, 6 )
    dummyPullHist_D4.GetYaxis().SetRangeUser( -3.5, 3.5 )
    dummyPullHist.SetMinimum( -3.5 )
    dummyPullHist.SetMaximum( 3.5 )
    dummyPullHist_D4.SetXTitle( "N_{jets}" )
    dummyPullHist_D4.SetTitleSize( 0.175, "x" )
    dummyPullHist_D4.SetLabelOffset( 0.025, "x" )
    dummyPullHist_D4.SetLabelSize( 0.24, "x" )
    dummyPullHist_D4.SetNdivisions( 404, "y" )
    dummyPullHist_D4.SetStats( 0 )
    
    for itDummyBin in xrange( 1, dummyPullHist.GetNbinsX()+1 ):
        if itDummyBin == 6 :
            dummyPullHist.GetXaxis().SetBinLabel( itDummyBin, "#geq"+"12" )
            dummyPullHist_D4.GetXaxis().SetBinLabel( itDummyBin, "#geq"+"12" )
        else :
            dummyPullHist.GetXaxis().SetBinLabel( itDummyBin, str( itDummyBin + 6 ) )
            dummyPullHist_D4.GetXaxis().SetBinLabel( itDummyBin, str( itDummyBin + 6 ) )

    return dummyPullHist, dummyPullHist_D4

def formatCanvasAndPads( c1, topPadArray, ratioPadArray ) :

    for iPad in xrange( 0, len(topPadArray) ) :
        topPadArray[iPad].SetLogy(1)
        topPadArray[iPad].SetBottomMargin(0.0)

        if iPad == 0 :
            topPadArray[iPad].SetLeftMargin( borderSize )
            ratioPadArray[iPad].SetLeftMargin( borderSize )
        else :
            topPadArray[iPad].SetLeftMargin( 0.0 )
            ratioPadArray[iPad].SetLeftMargin( 0.0 )

        if iPad == (len(topPadArray) - 1) :
            topPadArray[iPad].SetRightMargin( borderSize - 0.1 )
            ratioPadArray[iPad].SetRightMargin( borderSize - 0.1)
        else :
            topPadArray[iPad].SetRightMargin( 0.0 )
            ratioPadArray[iPad].SetRightMargin( 0.0 )
        
        topPadArray[iPad].Draw()

        ratioPadArray[iPad].SetTopMargin( 0.0 )
        ratioPadArray[iPad].SetBottomMargin( 0.40 )
        ratioPadArray[iPad].SetGridx(1)
        ratioPadArray[iPad].SetGridy(1)
        ratioPadArray[iPad].Draw()

    return c1, topPadArray, ratioPadArray

def makeCanvasAndPads(year, prefix) :
    tag = year + "_" + prefix

    c1                          = ROOT.TCanvas( "c1_%s"%(tag), "c1_%s"%(tag), 0, 0, 1200, 480 )
    
    p1_D1                       = ROOT.TPad( "p1_D1_%s"%(tag), "p1_D1_%s"%(tag), 0, 0.30, ( pad1and4Size + 0.08 )/totalPadSize, 1.0 )
    p2_D1                       = ROOT.TPad( "p2_D1_%s"%(tag), "p2_D1_%s"%(tag), 0, 0, ( pad1and4Size + 0.08 )/totalPadSize, 0.30 )
    
    p1_D2                       = ROOT.TPad( "p1_D2_%s"%(tag), "p1_D2_%s"%(tag), ( pad1and4Size + 0.08 )/totalPadSize, 0.30, (pad1and4Size + pad2and3Size + 0.09)/totalPadSize, 1.0 )
    p2_D2                       = ROOT.TPad( "p2_D2_%s"%(tag), "p2_D2_%s"%(tag), ( pad1and4Size + 0.08 )/totalPadSize, 0, (pad1and4Size + pad2and3Size + 0.09 )/totalPadSize, 0.30 )
    
    p1_D3                       = ROOT.TPad( "p1_D3_%s"%(tag), "p1_D3_%s"%(tag), (pad1and4Size + pad2and3Size + 0.09 )/totalPadSize, 0.30, (pad1and4Size + 2*pad2and3Size + 0.10 )/totalPadSize, 1.0 )
    p2_D3                       = ROOT.TPad( "p2_D3_%s"%(tag), "p2_D3_%s"%(tag), (pad1and4Size + pad2and3Size + 0.09 )/totalPadSize, 0, (pad1and4Size + 2*pad2and3Size + 0.10 )/totalPadSize, 0.30 )
    
    p1_D4                       = ROOT.TPad( "p1_D4_%s"%(tag), "p1_D4_%s"%(tag), (pad1and4Size + 2*pad2and3Size + 0.10)/totalPadSize, 0.30, 1.0, 1.0 )
    p2_D4                       = ROOT.TPad( "p2_D4_%s"%(tag), "p2_D4_%s"%(tag), (pad1and4Size + 2*pad2and3Size + 0.10)/totalPadSize, 0, 1.0, 0.30 )

    return c1, [p1_D1, p1_D2, p1_D3, p1_D4], [p2_D1, p2_D2, p2_D3, p2_D4]


if __name__ == "__main__" :
    main()
