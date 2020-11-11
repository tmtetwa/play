
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

void CheckDigits(std::string digifile = "trddigits.root",
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

  TH2F* hPadvRow = new TH2F("hPadvRow", "hPadvRow",144, 0, 143, 16, 0, 15);
  TH2F* hPadvRow1 = new TH2F("hPadvRow1", "hPadvRow1 det == 50;pad;row",144, 0, 143, 16, 0, 15);

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
        if (adc == (ADC_t)TRDSimParam::Instance()->GetADCoutRange()) {
        // LOG(INFO) << "Out of range ADC " << adc;
           continue;
        }
        hPadvRow->Fill(pad,row,adc);
        hPadvRow1->Fill(pad,row,adc);
      }

    } // digit loop

    TCanvas* c5 = new TCanvas("c5", "hPadvRow1 ", 600, 600);
    hPadvRow1->Draw("COLZ");
    hPadvRow1->Draw("TEXT90,SAME");
    c5->SaveAs("hPadvRow1.pdf");

  }
 }
