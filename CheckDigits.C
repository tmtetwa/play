
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

void CheckDigits(std::string digifile = "/scratch/alice/trddigits.root",
                 std::string hitfile = "/scratch/alice/o2sim_HitsTRD.root",
                 std::string inputGeom = "",
                 std::string paramfile = "/scratch/alice/o2sim_par.root")
{
  TFile* fin = TFile::Open(digifile.data());
  TTree* digitTree = (TTree*)fin->Get("o2sim");
  std::vector<Digit>* digitCont = nullptr;
  //std::vector<Digit>* digits = nullptr;
  digitTree->SetBranchAddress("TRDDigit", &digitCont);
  //digitTree->SetBranchAddress("TRDDigit", &digits);
  int nev = digitTree->GetEntries();

  // TH2F* hPadvRow = new TH2F("hPadvRow", "hPadvRow",144, 0, 143, 16, 0, 15);
  TH2F* hPadvRowfor1 = new TH2F("hPadvRowfor1", "hPadvRowfor1 det 3;pad;row",144, 0, 143,16, 0, 15);
  TH2F* hPadvRow1[540];
  for (int d = 0; d < 540; ++d) {
    hPadvRow1[d] = new TH2F(Form("hPadvRow1_%d", d), Form("hPadvRow1 Detector = %d;pad;row", d), 144, 0, 143, 16, 0, 15);
  }

  LOG(INFO) << nev << " entries found";
  for (int iev = 0; iev < nev; ++iev) {
    digitTree->GetEvent(iev);
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
        int padsum[144][16];
        padsum[pad][row] += adc;
        //hPadvRow->Fill(pad,row,adc);
        hPadvRow1[det]->Fill(pad,row,adc);

        if(det == 9)
        {
          //cout << "ADC: " << adc << "DET "<< det << "Pad: " << pad << "R: "
          //<< row<<endl;
          if(pad>49 && pad<54)
          {
            if(row>=0 && row<=3)
            {
              
                  cout << row << "r  "  << pad << "  p"<< adc << " adc "<< endl;


            }
          }
          hPadvRowfor1->Fill(padsum[pad][row],adc);
        }

      }

    } // digit loop

    TCanvas* c5 = new TCanvas("c5", "hPadvRow1 ", 600, 600);

    //std::vector<int> dete;
    for (int d = 1; d < 10; ++d) {
      if (hPadvRow1[d]->GetEntries() < kMINENTRIES) {
        continue;
      }
      hPadvRow1[d]->Draw("COLZ");
      hPadvRow1[d]->Draw("TEXT90,SAME");

    }
    c5->SaveAs("hPadvRow1.pdf");

    TCanvas* c1 = new TCanvas("c1", "hPadvRowfor1 ", 600, 600);

    hPadvRowfor1->Draw("COLZ");
    hPadvRowfor1->Draw("TEXT90,SAME");

    c1->SaveAs("hPadvRowfor1.pdf");

  }
 }
