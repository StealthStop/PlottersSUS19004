import numpy as np
import ROOT
import argparse
ROOT.gStyle.SetOptTitle( 0 )
ROOT.gStyle.SetOptStat( 0 )
ROOT.gROOT.SetBatch( ROOT.kTRUE )

def main(approved, year, withGR):

    fName = None
    if withGR: fName = "Training/Keras_Tensorflow_%s_v1.2/deepESMbin_dis_nJet.npy"%(year)
    else:      fName = "Training/Keras_Tensorflow_%s_v1.2/noGR/deepESMbin_dis_nJet.npy"%(year)

    outName = None
    if withGR:
        outName = "./PlotsForLegacyAna/Supplementary/CMS-SUS-19-004_Figure-aux_003-b"
    else:
        outName = "./PlotsForLegacyAna/Supplementary/CMS-SUS-19-004_Figure-aux_003-a"

    data = np.load(fName, allow_pickle=True, encoding='latin1').item()
    prefix = ""
    lumi = None
    if year == "2016": lumi = "35.9"
    else:              lumi = "41.5"
    maskNames = [("mask_07",ROOT.kRed, "N_{jets} = 7"),("mask_10",ROOT.kBlack, "N_{jets} = 10"),("mask_08",ROOT.kGreen+2, "N_{jets} = 8"),
                 ("mask_11",ROOT.kBlue+1, "N_{jets} #geq 11"),("mask_09",ROOT.kOrange+1,"N_{jets} = 9")]

    histos = []
    for m, c, n in maskNames:
        h = ROOT.TH1D(m, m, 10, 0.0, 1.0)
        h.SetLineColor(c)
        h.SetMarkerColor(c)
        h.SetLineWidth( 3 )
        histos.append( (h, data[m], n) )
    
    for i in range(0, len(data["y"])):
        for h, m, _ in histos:
            if m[i] and bool(data["labels"][i][1]):
                h.Fill(data["y"][i], data["Weight"][i])
    
    c1 = ROOT.TCanvas( "c1", "c1", 0, 0, 470, 400 )    
    #c1.SetLogy(1)
    c1.SetTicks(1,1);
    c1.SetBottomMargin( 0.15 )
    c1.SetLeftMargin( 0.17 )
    c1.Draw()    
    
    l1 = ROOT.TLegend( 0.23, 0.64, 0.87, 0.84 )
    l1.SetTextSize(0.05)
    l1.SetBorderSize(0)
    l1.SetNColumns(2)
    
    dummy = ROOT.TH1F("","",1, 0.0, 1.0)            
    dummy.SetLineColor( ROOT.kBlack )
    dummy.SetYTitle( "A. U." )
    dummy.SetTitleSize( 0.055, "y" )
    dummy.SetTitleOffset( 1.3, "y" )
    dummy.SetLabelSize( 0.045, "y" )           
    dummy.GetYaxis().SetNdivisions(9,5,0);
    dummy.GetYaxis().SetRangeUser( 0.0, 0.4 )
    dummy.SetXTitle( "S_{NN}" )
    dummy.SetTitleSize( 0.055, "x" )
    dummy.SetTitleOffset( 1.0, "x" )
    dummy.SetLabelSize( 0.045, "x" )           
    dummy.GetXaxis().SetNdivisions(6,5,0);
    dummy.Draw( "HIST" )
    for h, _, n in histos:
        h.DrawNormalized("HIST same")
        l1.AddEntry(h, n)

    l1.Draw()                       
    
    mark = ROOT.TLatex()
    mark.SetNDC( ROOT.kTRUE )
    mark.SetTextAlign( 11 )
    mark.SetTextSize( 0.055 )
    mark.SetTextFont( 60 )
    mark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.001, 0.915, "CMS")
    mark.SetTextSize( 0.044 )
    mark.SetTextFont( 52 )
    mark.DrawLatex( ROOT.gPad.GetLeftMargin() + 0.001 + 0.1, 0.915, "Simulation Supplementary")
    mark.SetTextFont( 42 )
    mark.SetTextAlign( 31 )
    mark.DrawLatex( 1 - ROOT.gPad.GetRightMargin(), 0.913, "%s (13 TeV)"%(year) )
    mark.SetTextAlign(11)
    mark.SetTextSize(0.035)
    mark.SetTextFont(42)
    mark.DrawLatex(ROOT.gPad.GetLeftMargin() + 0.03, 1 - (ROOT.gPad.GetTopMargin() + 0.055), "arXiv:2102.06976")
    
    c1.SaveAs(outName + ".pdf")
    c1.SaveAs(outName + ".png")

if __name__ == "__main__" :
    usage = "usage: %prog [options]"
    parser = argparse.ArgumentParser(usage)
    parser.add_argument("--approved", dest="approved", help="Plot is approved",           action="store_true", default=False) 
    parser.add_argument("--year",     dest="year",     help="Which year",                 type=str,            default="2016")
    parser.add_argument("--withGR",   dest="withGR",   help="Results with GR",            action="store_true", default=False)
    args = parser.parse_args()

    main(args.approved, args.year, args.withGR)
