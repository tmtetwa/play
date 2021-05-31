#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include "TStyle.h"
#include <iostream>
#include "FairLogger.h"
#include "TRDBase/Digit.h"
#include "TRDBase/TRDSimParam.h"
#include "TRDBase/TRDCommonParam.h"
#include "DataFormatsTRD/Constants.h"
#endif
using namespace o2::trd;
constexpr int kMINENTRIES = 100;

void padrsumDigits(std::string digifile = "trddigits.root",
                 std::string hitfile = "o2sim_HitsTRD.root",
                 std::string inputGeom = "",
                 std::string paramfile = "o2sim_par.root")
{
  TFile* fin = TFile::Open(digifile.data());
  TTree* digitTree = (TTree*)fin->Get("o2sim");
  std::vector<Digit>* digitCont = nullptr;
  //std::vector<Digit>* digits = nullptr;
  digitTree->SetBranchAddress("TRDDigit", &digitCont);
  //digitTree->SetBranchAddress("TRDDigit", &digits);
  int nev = digitTree->GetEntries();

  TH1F* padrsum = new TH1F("padrsum", "padrsum", 100, 0, 3000);
  TH1F* padrmax = new TH1F("padrmax", "padrmax", 100, 0, 3000);
  TH1F* padrhi = new TH1F("padrhi", "padrhi", 100, 0, 3000);
  TH1F* padrlo = new TH1F("padrlo", "padrlo", 100, 0, 3000);

  LOG(INFO) << nev << " entries found";
  for (int iev = 0; iev < nev; ++iev) {
    digitTree->GetEvent(iev);
      int padr[540][30][16];
      for (const auto& digit : *digitCont) {
          // loop over det, pad, row?
          auto adcs = digit.getADC();
          int det = digit.getDetector();
          int row = digit.getRow();
          int pad = digit.getPad();

          for (int tb = 0; tb < o2::trd::constants::TIMEBINS; ++tb) {
            ADC_t adc = adcs[tb];
            if (adc == (ADC_t)SimParam::instance()->getADCoutRange()) {
              // LOG(INFO) << "Out of range ADC " << adc;
              continue;
            }
            //int rowb[det][pad][tb];
            padr[det][tb][row] += adc;
            padrsum->Fill(padr[det][tb][row]);
          }

        }// end digitcont

        for(int d=0; d<540; d++)
        {
          for(int t=0; t<30; t++)
          {
            for(int r=0; r<16; r++)
            {
              if (padr[d][t][r]>padr[d][t][r-1] && padr[d][t][r]>padr[d][t][r+1])
              {
                if (padr[d][t][r-1] > padr[d][t][r+1])
                {
                  padrmax->Fill(padr[d][t][r]);
                  padrhi->Fill(padr[d][t][r-1]);
                  padrlo->Fill(padr[d][t][r+1]);

                //  cout<< "padsum max value : " << padr[d][r][t] << endl;
                }
                else
                {
                  padrmax->Fill(padr[d][t][r]);
                  padrhi->Fill(padr[d][t][r+1]);
                  padrlo->Fill(padr[d][t][r-1]);

                  //cout<< "padsum max value : " << padr[d][r][t] << endl;
                }// end else
              }// end if (padr[d][r][t]>padr[d][r][t-1] && padr[d][r][t]>padr[d][r][t+1])
            }// end for t
          }// end for r
        }// end for d

      } // event loop (should be TF loop)

        TCanvas* c6 = new TCanvas("c6", "padrsums ", 600, 600);

        std::vector<int> dete;
        for (int d = 1; d < 10; ++d) {
          if (hPadvRow1[d]->GetEntries() < kMINENTRIES) {
            continue;

          }
          // if(count == 0)
          // {
            hPadvRow1[d]->Draw("COLZ");
            hPadvRow1[d]->Draw("TEXT90,SAME");
          //hPadvRow1[d]->SetLabelFormat("%txt");
          //   if (max < hPadvRow1[d]->GetMaximum()) {
          //     max = hPadvRow1->GetMaximum();
          //   }
          // count++;
          gStyle->SetOptFit();
          TLatex *txt = new TLatex(0, 144, "max") ;
          txt->SetTextFont(42);
          txt->SetTextColor(kRed);
          txt->Draw();
        //}
        }

        //gStyle->SetOptFit();
        padrmax->SetLineColor(kRed);
        padrlo->SetLineColor(kBlue);
        padrhi->SetLineColor(kGreen);
        padrsum->SetLineColor(kBlack);

        padrsum->Draw("COLZ");
        padrmax->Draw("SAME");
        padrhi->Draw("SAME");
        padrlo->Draw("SAME");

        TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
        leg->SetBorderSize(0); // no border
        leg->SetFillStyle(0);
        leg->SetFillColor(0); // Legend background should be white
        leg->SetTextFont(42);
        leg->SetTextSize(0.03); // Increase entry font size!
        leg->AddEntry(padrsum, "padrsum", "l");
        leg->AddEntry(padrmax, "padrmax", "l");
        leg->AddEntry(padrhi, "padrhi", "l");
        leg->AddEntry(padrlo, "padrlo", "l");
        leg->Draw();

        c6->SaveAs("padrsum.pdf");
}// end of macro
