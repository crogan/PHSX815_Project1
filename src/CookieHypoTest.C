// C++ input/output includes
#include <iostream>
#include <fstream>

// ROOT includes (for histograms and plotting)
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMath.h"
#include "TLegend.h"

// our local includes
#include "MySort.hh"

using namespace std;

vector<double> GetLineValues(std::string& line);

// Exponential probability distribution
double ExpProb(double x, double rate);

int CanvasNumber = 0;
TCanvas* PlotHypotheses(vector<double>& array0, vector<double>& array1,
			string& title, double alpha = 0.);



// main function for compiled executable CookieHypoTest
int main(int argc, char* argv[]){
  bool printhelp = false;

  string InputFile[2];

  bool haveInput[2] = {false, false};

  double alpha = 0.;
  
  // parsing any command line options added by the user
  for(int i = 1; i < argc; i++){
    if(strncmp(argv[i],"--help", 6) == 0){
      printhelp = true;
      continue;
    }
    if(strncmp(argv[i],"-h", 2) == 0){
      printhelp = true;
      continue;
    }
    if(strncmp(argv[i],"-H0", 3) == 0){
      i++;
      InputFile[0] = string(argv[i]);
      haveInput[0] = true;
    }
    if(strncmp(argv[i],"-H1", 3) == 0){
      i++;
      InputFile[1] = string(argv[i]);
      haveInput[1] = true;
    }
    if(strncmp(argv[i],"-alpha", 6) == 0){
      i++;
      alpha = std::stod(argv[i]);
    }
  }

  // print the executable usage options if the user adds -h or --help or doesn't provide input
  if(printhelp || !haveInput[0] || !haveInput[1]){
    cout << "Usage: " << argv[0] << " [options] -H0 [input file 0] -H1 [input file 1]" << endl;
    cout << "  options:" << endl;
    cout << "   --help(-h)          print options" << endl;
    cout << "   -alpha [number]     significance of test" << endl;

    return 0;
  }

  string line;
  vector<double> lineVals;

  int Nmeas;

  double rate[2];
  vector<vector<double> > times[2];
  
  // read in experiment times
  for(int h = 0; h < 2; h++){
    std::ifstream ifile(InputFile[h].c_str());
    if(!ifile.is_open()){
      std::cout << "Unable to read input data file " << InputFile[h] << std::endl;
      return 0;
    }
  
    // first line is the rate parameter used for simulation
    getline(ifile, line);
    rate[h] = stod(line);

    int iexp = 0; // count number of experiments
    while(getline(ifile, line)){
      lineVals = GetLineValues(line);

      // number of measurements in this experiment
      Nmeas = lineVals.size();
      // add a new vector for this experiment
      times[h].push_back(vector<double>());
      for(int i = 0; i < Nmeas; i++){
	times[h][iexp].push_back(lineVals[i]);
      }
      iexp++;
    }
    ifile.close();
  }

  vector<double> LLR[2]; // Log Likelihood Ratios for each hypothesis, vector of experiments

  
  // construct vector of likelihood ratios for each experiment, for each hypothesis
  for(int h = 0; h < 2; h++){ // loop over hypotheses
    int Nexp = times[h].size();
    for(int e = 0; e < Nexp; e++){ // loop over experiments
      Nmeas = times[h][e].size();
      double LogLikeRatio = 0;
      for(int m = 0; m < Nmeas; m++){ // loop over times in an experiment
	LogLikeRatio += log( ExpProb(times[h][e][m], rate[1]) ); // Log-like for H1
	LogLikeRatio -= log( ExpProb(times[h][e][m], rate[0]) ); // Log-like for H0
      }
      LLR[h].push_back(LogLikeRatio);
    }
  }

  // sort arrays of Log Likelihood ratios
  MySort Sorter;
  Sorter.DefaultSort(LLR[0]);
  Sorter.DefaultSort(LLR[1]);
  
  string title = to_string(Nmeas)+" measurements / experiment with rates "+string(Form("%.1f, %.1f",rate[0], rate[1]))+" cookies / day";
  TCanvas* canvas = PlotHypotheses(LLR[0], LLR[1], title, alpha);
  canvas->SaveAs("Hypotheses.pdf");
  
}

// read a vector of doubles from a string (whitespace separated)
vector<double> GetLineValues(std::string& line){
  // remove leading whitespace
  while(line[0] == string(" ")[0])
    line.erase(0,1);

  vector<double> ret;
  string num;
  while(line.find(" ") != string::npos){
    size_t p = line.find(" ");
    num = line.substr(0,p);
    line.erase(0,p+1);
    while(line[0] == string(" ")[0])
      line.erase(0,1);

    ret.push_back(stod(num));
  }
  
  return ret;
}

// Exponential probability distribution
double ExpProb(double x, double rate){
  return rate*exp(-rate*x);
}

// Plot an arrays of doubles as a histograms (and return canvas)
TCanvas* PlotHypotheses(vector<double>& array0, vector<double>& array1,
			string& title, double alpha){
  int N0 = array0.size();
  int N1 = array1.size();
  double hmin = std::min(array0[0], array1[0]);
  double hmax = std::max(array0[N0-1], array1[N1-1]);

  // create histograms
  TH1D* hist = new TH1D(Form("hist0_%d", CanvasNumber),
			Form("hist0_%d", CanvasNumber),
			100, hmin, hmax);
  TH1D* hist1 = new TH1D(Form("hist1_%d", CanvasNumber),
			 Form("hist1_%d", CanvasNumber),
			 100, hmin, hmax);

  for(int i = 0; i < N0; i++)
    hist->Fill(array0[i]);
  for(int i = 0; i < N1; i++)
    hist1->Fill(array1[i]);

  // Normalize to unit area to make a probability distributions
  hist->Scale(1./hist->Integral());
  hist1->Scale(1./hist1->Integral());

  // some formating settings
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  TCanvas* canvas = (TCanvas*) new TCanvas(Form("canvas_%d", CanvasNumber),
					   Form("canvas_%d", CanvasNumber),
					   500.,400);
  double hlo = 0.15;
  double hhi = 0.04;
  double hbo = 0.15;
  double hto = 0.07;
  canvas->SetLeftMargin(hlo);
  canvas->SetRightMargin(hhi);
  canvas->SetBottomMargin(hbo);
  canvas->SetTopMargin(hto);
  canvas->SetGridy();
  canvas->SetLogy();
  canvas->Draw();
  canvas->cd();

  hist->SetLineColor(kBlue+2);
  hist->SetFillColor(kBlue+1);
  hist->SetFillStyle(3004);
  hist->Draw("hist");
  hist->GetXaxis()->CenterTitle();
  hist->GetXaxis()->SetTitleFont(42);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetXaxis()->SetTitleOffset(1.1);
  hist->GetXaxis()->SetLabelFont(42);
  hist->GetXaxis()->SetLabelSize(0.04);
  hist->GetXaxis()->SetTitle("#lambda = log [ #it{L}(H1) / #it{L}(H0) ]");
  hist->GetXaxis()->SetTickSize(0.);
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleFont(42);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleOffset(1.1);
  hist->GetYaxis()->SetLabelFont(42);
  hist->GetYaxis()->SetLabelSize(0.035);
  hist->GetYaxis()->SetTitle("Probability");
  hist->GetYaxis()->SetRangeUser(0.5*std::min(1./double(N0),1./double(N1)),
				 2*std::max(hist->GetMaximum(),hist1->GetMaximum()));

  hist1->SetLineColor(kGreen+2);
  hist1->SetFillColor(kGreen+1);
  hist1->SetFillStyle(3004);
  hist1->Draw("hist same");

  TLegend* leg = new TLegend(0.17,0.68,0.3946,0.8947);
  leg->SetTextFont(132);
  leg->SetTextSize(0.045);
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);
  leg->SetShadowColor(kWhite);
  leg->AddEntry(hist, "P(#lambda | H0)");
  leg->AddEntry(hist1, "P(#lambda | H1)");
  leg->Draw();
  
  // Draw some text on canvas
  TLatex l;
  l.SetTextFont(42);
  l.SetTextAlign(21);
  l.SetTextSize(0.04);
  l.SetNDC();

  l.DrawLatex((1.-hhi+hlo)/2., 1.-hto+0.012, title.c_str());
  
  if(alpha > 1./double(N0)){
    double lambda_crit = array0[std::min(int((1.-alpha)*N0),N0-1)];
    double beta = 0.;

    TH1D* histp = new TH1D(Form("histp0_%d", CanvasNumber),
			   Form("histp0_%d", CanvasNumber),
			   100, hmin, hmax);
    TH1D* histp1 = new TH1D(Form("histp1_%d", CanvasNumber),
			    Form("histp1_%d", CanvasNumber),
			    100, hmin, hmax);

    for(int i = 0; i < N0; i++)
      if(array0[i] > lambda_crit)
	histp->Fill(array0[i]);
    for(int i = 0; i < N1; i++)
      if(array1[i] < lambda_crit){
	histp1->Fill(array1[i]);
	beta++;
      }

    beta /= double(N1);

    histp->Scale(1./double(N0));
    histp1->Scale(1./double(N1));

    histp->SetLineColor(kBlue+2);
    histp->SetFillColor(kBlue+2);
    histp->SetFillStyle(3104);
    histp1->SetLineColor(kGreen+2);
    histp1->SetFillColor(kGreen+2);
    histp1->SetFillStyle(3104);
    histp->Draw("hist same");
    histp1->Draw("hist same");

    l.SetTextAlign(11);
    l.SetTextSize(0.045);
    l.SetTextColor(kBlue+3);
    if(alpha > 0.01)
      l.DrawLatex(0.19,0.63, Form("#alpha = %.2f", alpha));
    else
      l.DrawLatex(0.19,0.63, Form("#alpha = %.3f", alpha));
    l.SetTextColor(kGreen+3);
    if(beta > 0.001)
      l.DrawLatex(0.19,0.55, Form("#beta = %.3f", beta));
    else
      l.DrawLatex(0.19,0.55, Form("#beta = %.1e", beta));
    TLine* line = new TLine();
    line->SetLineWidth(2);

    double crit = hlo + (1.-hhi-hlo)*(lambda_crit-hmin)/(hmax-hmin);
    line->DrawLineNDC(crit, hbo, crit, 1-hto);
    l.SetTextSize(0.035);
    l.SetTextAlign(33);
    l.SetTextAngle(90);
    l.SetTextColor(kBlack);
    l.DrawLatex(crit+0.005, 1-hto-0.01, Form("#scale[1.4]{#lambda_{#alpha}} = %.3f", lambda_crit));
    
  }
  
  
  CanvasNumber++;
  
  return canvas;
}
