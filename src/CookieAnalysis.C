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

// our local includes
#include "MySort.hh"

using namespace std;

vector<double> GetLineValues(std::string& line);

int CanvasNumber = 0;
TCanvas* PlotVector(vector<double>& array, string& title, string& xaxis, bool do_quantiles = false);

// main function for compiled executable CookieAnalysis
int main(int argc, char* argv[]){
  bool printhelp = false;

  string InputFile;

  bool haveInput = false;
  string InputFile1;
  
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
   
    InputFile = string(argv[i]);
    haveInput = true;
  }

  // print the executable usage options if the user adds -h or --help or doesn't provide input
  if(printhelp || !haveInput){
    cout << "Usage: " << argv[0] << " [options] [input file]" << endl;
    cout << "  options:" << endl;
    cout << "   --help(-h)          print options" << endl;

    return 0;
  }

  string line;
  vector<double> lineVals;

  int Nmeas;
  
  vector<double> times;
  vector<double> times_avg;
  
  // read input file
  std::ifstream ifile(InputFile.c_str());
  if(!ifile.is_open()){
    std::cout << "Unable to read input data file " << InputFile << std::endl;
    return 0;
  }
  
  // first line is the rate parameter used for simulation
  getline(ifile, line);
  double rate = stod(line);
 
  while(getline(ifile, line)){
    lineVals = GetLineValues(line);

    // number of measurements in this experiment
    Nmeas = lineVals.size();
    double t_avg = 0;
    for(int i = 0; i < Nmeas; i++){
      t_avg += lineVals[i];
      times.push_back(lineVals[i]);
    }
    t_avg /= double(Nmeas);
    times_avg.push_back(t_avg);
  }
  ifile.close();

  // sort arrays of times and average times
  MySort Sorter;
  Sorter.DefaultSort(times);
  Sorter.DefaultSort(times_avg);
  // try some other methods! see how long they take
  // Sorter.BubbleSort(times_avg);
  // Sorter.InsertionSort(times_avg);
  // Sorter.QuickSort(times_avg);
  
  string title0 = to_string(Nmeas)+" measurements / experiment with rate "+string(Form("%.2f",rate))+" cookies / day";
  string xaxis0 = "Average time between missing cookies [days]";
  TCanvas* canvas0 = PlotVector(times_avg, title0, xaxis0, true);
  canvas0->SaveAs("times_avg.pdf");

  string title1 = "rate of "+string(Form("%.2f",rate))+" cookies / day";
  string xaxis1 = "Time between missing cookies [days]";
  TCanvas* canvas1 = PlotVector(times, title1, xaxis1);
  canvas1->SaveAs("times.pdf");
  
  
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

// Plot an array of doubles as a histogram (and return canvas)
TCanvas* PlotVector(vector<double>& array, string& title, string& xaxis, bool do_quantiles){
  int N = array.size();
  double hmax = array[N-1];

  // create histogram object
  TH1D* hist = new TH1D(Form("hist_%d", CanvasNumber),
			Form("hist_%d", CanvasNumber),
			100, 0., hmax);

  for(int i = 0; i < N; i++)
    hist->Fill(array[i]);

  // Normalize to unit area to make a probability distribution
  hist->Scale(1./hist->Integral());

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
  hist->GetXaxis()->SetTitle(xaxis.c_str());
  hist->GetXaxis()->SetTickSize(0.);
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleFont(42);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleOffset(1.1);
  hist->GetYaxis()->SetLabelFont(42);
  hist->GetYaxis()->SetLabelSize(0.035);
  hist->GetYaxis()->SetTitle("Probability");

  // Draw some text on canvas
  TLatex l;
  l.SetTextFont(42);
  l.SetTextAlign(21);
  l.SetTextSize(0.04);
  l.SetNDC();

  l.DrawLatex((1.-hhi+hlo)/2., 1.-hto+0.012, title.c_str());
  
  if(do_quantiles){
    TLine* line = new TLine();
    line->SetLineWidth(2);

    double median = array[int(0.5*N)];
    double xmedian = hlo + (1.-hhi-hlo)*median/hmax;
    line->DrawLineNDC(xmedian, hbo, xmedian, 1-hto);
    l.SetTextSize(0.03);
    l.SetTextAlign(33);
    l.SetTextAngle(90);
    l.DrawLatex(xmedian+0.005, 1-hto-0.01, Form("median = %.3f", median));

    for(int sigma = 1; sigma <= 3; sigma++){
      // convert sigma to fraction of prob.;
      double P = TMath::Erf(sigma/sqrt(2.))/2.;
      double low  = array[int((0.5-P)*N)];
      double high = array[int((0.5+P)*N)];
      double xlow  = hlo + (1.-hhi-hlo)*low/hmax;
      double xhigh = hlo + (1.-hhi-hlo)*high/hmax;
      line->SetLineColor(kGreen+5-sigma);
      line->DrawLineNDC(xlow, hbo, xlow, 1-hto);
      line->DrawLineNDC(xhigh, hbo, xhigh, 1-hto);
      l.SetTextColor(kGreen+5-sigma);
      l.DrawLatex(xlow+0.005, 1-hto-0.01, Form("-%d #sigma = %.3f", sigma, low));
      l.DrawLatex(xhigh+0.005, 1-hto-0.01, Form("+%d #sigma = %.3f", sigma, high));
    }
  }

  // // line->DrawLineNDC(hlo, hbo-0.024*lmax, 1-hhi, hbo-0.0235*lmax);
 
  // l.SetTextSize(0.025);
  // l.SetTextFont(42);
  // l.SetTextAlign(23);
  // line->SetLineWidth(1);
  // double lo = hlo;
  // double hi = hlo;
  // double yline = hbo-0.024*lmax;
  // int ib = 0;
  // for(int r = 0; r < NR; r++){
  //   int NM = bin[r].NBins();
  //   lo = hi;
  //   hi = double(NM)/double(NB)*(1.-hhi-hlo) + lo;
    
  //   line->SetLineStyle(1);
  //   line->DrawLineNDC(lo + eps, yline,
  // 		      lo + eps, yline + 6*eps);
  //   line->DrawLineNDC(hi - eps, yline,
  // 		      hi - eps, yline + 6*eps);
  //   line->DrawLineNDC(lo + eps, yline,
  // 		      hi - eps, yline);
  //   line->SetLineStyle(5);
  //   line->DrawLineNDC(hi, hbo, hi, 1.-hto);
  //   line->SetLineStyle(3);
  //   for(int b = 0; b < NM; b++){
  //     if(ib%2 == 1)
  // 	line->DrawLineNDC(lo + (hi-lo)*(b+0.5)/double(NM), hbo,
  // 			  lo + (hi-lo)*(b+0.5)/double(NM), (hbo+yline)/2.+eps);
  //     ib++;
  //   }
    
  //   l.DrawLatex((hi+lo)/2., yline - 8*eps, blabels[r].c_str());
  // }
  
  CanvasNumber++;
  
  return canvas;
}
