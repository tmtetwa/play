
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file CheckDigits.C
/// \brief Simple macro to check TRD digits

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

void padvtbDigits(std::string digifile = "trddigits.root",
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

  // TH2F* hPadvRow = new TH2F("hPadvRow", "hPadvRow",144, 0, 143, 16, 0, 15);
  // TH2F* hPadvtbfor1[540];
  // for (int d = 0; d < 540; ++d) {
  // TH2F* hPadvtbfor1[d] = new TH2F(Form("hPadvtbfor1_d%", d), Form("hPadvtbfor1 det %d;pad;tb", d), 144, 0, 143, 30, 0, 29);
  // }
  TH2F* hPadvtbfor1 = new TH2F("hPadvtbfor1", "hPadvtbfor1 det ;pad;tb",144, 0, 143,30, 0, 29);
  TH1F* hPadvtbfor1max = new TH1F("hPadvtbfor1max", "hPadvtbfor1 max det ;padsum",100, 0, 1280000);
  TH2F* hPadvtb1[540];
  for (int d = 0; d < 540; ++d) {
    hPadvtb1[d] = new TH2F(Form("hPadvtb1_%d", d), Form("hPadvtb1 Detector = %d;pad;tb", d), 144, 0, 143, 30, 0, 29);
  }

  LOG(INFO) << nev << " entries found";
  for (int iev = 0; iev < nev; ++iev) {
    digitTree->GetEvent(iev);
    int padsum[30];
    for (const auto& digit : *digitCont)
     {
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
        hPadvtb1[det]->Fill(pad,tb,adc);

        if(det == 11)
        {
          if(pad>93 && pad<95)
          {
            if(tb>=11 && tb<=13)
            {
              int sumofpads = 0;
              padsum[tb] +=adc;
              sumofpads = (adc) + (adc+1) +(adc +2);
              //looking for local max of pad value
              if (padsum[tb]>padsum[tb-1] && padsum[tb]>padsum[tb+1])
              {

                cout << " tb " << tb  << "  p: "<< pad << " adc: " << adc
                << " padsum local max: " << padsum[tb] << endl;
              }
              cout << " tb " << tb  << "  p: "<< pad << " adc: " << adc
              << " padsum: " << padsum[tb] << " Sum of three pads: " <<
              sumofpads<< endl;
            }
            hPadvtbfor1->Fill(padsum[tb],adc);
          }
        }
      }// digit loop
    }//iev loop

    TCanvas* c5 = new TCanvas("c5", "hPadvtb1 ", 600, 600);

    //std::vector<int> dete;
    for (int d = 1; d < 12; ++d) {
      if (hPadvtb1[d]->GetEntries() < kMINENTRIES) {
        continue;
      }
      hPadvtb1[d]->Draw("COLZ");
      hPadvtb1[d]->Draw("TEXT90,SAME");

    }
    c5->SaveAs("hPadvtb1.pdf");

    TCanvas* c1 = new TCanvas("c1", "hPadvtbfor1 ", 600, 600);
    // for (int de = 1; de < 10; ++de) {
    //   if (hPadvtbfor1[de]->GetEntries() < kMINENTRIES) {
    //     continue;
    //   }
    hPadvtbfor1->Draw("COLZ");
    hPadvtbfor1->Draw("TEXT90,SAME");

    c1->SaveAs("hPadvtbfor1.pdf");

}
}
