#include <TROOT.h>
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TCanvas.h"
#include <vector>
#include <iostream>
#include <TF1.h>
#include <string>
#include "TStyle.h"
#include "localColor.h"
#include "TLegend.h"
#include <fstream>
#include <experimental/filesystem>

using namespace std;

//void plot(){
int main(int argc, char* argv[]) {
  if(argc==1){
    cout<<"Input run numbers you wish to run over as arguments"<<endl;
    return 0;
  }
  vector<int> runs;//= {4747, 4740, 4739, 4738, 4741, 4749, 4746, 4748, 4729, 4750};
  bool doGainLabels=true;
  map<int, int> gains, volts;
  //4729, 4738, 4739, 4740, 4741, 4746, 4747, 4748, 4749, 4750};
  //vector<int> thresh= {360, 340, 320, 300, 280, 260, 240, 230, 220, 215};
//220, 300, 320, 340, 280, 240, 360, 230, 260, 215 };
  //for(int i=1; i<argc; i++){
  //  runs.push_back(atoi(argv[i]));
  //}
  std::ifstream finfo("runinfo.csv");
  string infostr;
  gStyle->SetEndErrorSize(5);
  if(doGainLabels){
    getline(finfo, infostr);
    while( getline(finfo , infostr)){
      if( infostr.size()==0 ) continue;
      int indx0=infostr.find(":") ;
      int indx1=infostr.find(",") ;
      //string runstr2=infostr.substr(0, indx0);
      //string voltstr=infostr.substr( indx0+1 ,  /*len=*/indx1-indx0-1   ) ;
      //string gainstr2=infostr.substr( indx1+1 ) ;
      //cout<< "run: "<<runstr2.c_str()<< "\t voltstr: "<<voltstr.c_str()<<"\t gainstr: "<< gainstr2.c_str()<<endl;

      int run=stoi( infostr.substr(0, indx0)   );
      volts[run]=stoi( infostr.substr( indx0+1 ,  /*len=*/indx1-indx0-1   ) );
      gains[run]=stoi( infostr.substr( indx1+1) );
    }
  }
  string runstr=to_string( atoi(argv[1]) );
  for(int i=1; i<argc; i++){
    cout<<"irun: "<< i<<endl;
    int runtmp=atoi(argv[i]);
    runs.push_back( runtmp );
    if(i!=1) runstr+="_"+to_string(runtmp);
    cout<<runs[i-1]<<endl;

  }
  int nruns=runs.size(); 
  string topdir=getenv("CRTDIR");
  if(topdir=="") topdir="/sbnd/data/users/${USER}/crtModTests";
  string csvname=topdir+"/run"+to_string(runs[0])+"/run"+ to_string(runs[0]) +"_pedestals.csv";
  cout<<"looking at file "<<csvname.c_str() <<endl;

  std::ifstream infile(csvname.c_str() );
  string line;
  int l=0;//counter, starts with header line
  vector<int>febs;
  while( getline(infile, line) ){
    if( (l-1)%32==0 ){
      string febstr=line.substr(0,line.find(','));
      try{ febs.push_back( stoi(febstr) );  }
      catch(std::exception e){ cout<<"Input CSV file doesn't have expected format...Found FEB# "<<febstr.c_str()<<"trying to iterate one more line down the file to look for an FEB number"<<endl; l++;}
    }
    l++;
  }
  cout<<"FEBS: ";
  for(int feb: febs) cout<<feb<<"\t";
  cout<<endl;
  //vector<int> febs={87, 193, 88, 85, 81, 80, 79, 89};
  map< int, vector<TH1D*>> hpeds, hrms;
  map<int, vector<vector<TH1D*>>> hadc;
  for(int r=0; r<nruns; r++){
    int run=runs[r];
    TString fname=Form("%s/run%d/pedestals.root",topdir.c_str(), run);
    TFile* fin= new TFile(fname);//'
    assert(fin);
    if(!fin){ 
      cout<<"ERROR: File "<<fname.Data() <<" not found."<<endl;
      return 1;
    }

    int nfebs=febs.size();
    for(int f=0; f<nfebs; f++){
      int feb=febs[f];
      TH1D* hp= (TH1D*) fin->Get(Form("pedestal_v_channel_feb%d", feb));
      if(!hp){ 
	cout<<"ERROR: Histogram for run "<<run<<", FEB "<<feb<<" not found."<<endl;
	if( feb==71 || feb ==72 || feb==75 || feb==76){//ignore if its a verical
	  cout<<"FEB"<<feb<< " disconected for some runs, removing from the feb array"<<endl;
	  febs.erase( remove( febs.begin(), febs.end(), feb) , febs.end() );
	  nfebs--; f--;
	}
	else return 1;
      }
      hpeds[feb].push_back(hp);
      hadc[feb].push_back({});
      for(int c=0; c<32; c++){
	TH1D* adc=(TH1D*)fin->Get(Form("adc_feb_%d_channel_%d_flag3", feb, c));
	hadc[feb][r].push_back(adc);
	//cout<<"feb,r,c"<<feb<<r<<c<<endl;
	//hadc[feb][r][c]->Print();
      }
    }
  }

  TCanvas* c1= new TCanvas("c1", "c1", 1280,800);
  c1->SetLeftMargin(0.15);
  c1->SetBottomMargin(0.15);
  gStyle->SetOptStat(0);
  c1->SetRightMargin(0.1);
  vector<int> colors=getColors(2);
  for( int f=0; f<febs.size(); f++){
    int feb=febs[f];
    
    TLegend* leg;
    if(runs.size()<8){ 
      leg=new TLegend(0.5, 0.7, 0.9,0.9);
      leg->SetNColumns(2);
    }
    else{
      leg=new TLegend(0.2, 0.7, 0.9,0.9);
      leg->SetNColumns(4);
    }
    leg->SetFillStyle(0);
    leg->SetLineWidth(0);

    double ymax=1;
    double ymaxerr=1;
    for(int r=1; r<nruns; r++){
      double maxtmp=hpeds[feb][r]->GetMaximum();
      if(maxtmp>ymax){
	ymax=maxtmp;
	ymaxerr= hpeds[feb][r]->GetBinError( hpeds[feb][r]->GetMaximumBin() );
      }
    }
    ymax=400;
    hpeds[feb][0]->GetYaxis()->SetTitle("Average ADC");
    hpeds[feb][0]->SetLineColor(colors[1]);
    hpeds[feb][0]->SetMarkerColor(colors[1]);
    hpeds[feb][0]->SetMarkerStyle(1);
    hpeds[feb][0]->SetLineWidth(2.5);
    TString lab= Form("Run %d", runs[0]);
    //if(gains[ runs[0] ] !=0 && doGainLabels) lab= Form("Gain %d, Bias %d", gains[ runs[0] ],  volts[ runs[0] ]);
    //leg->AddEntry(hpeds[feb][0], lab, "lep");
    //leg->AddEntry(hpeds[feb][0], Form("Threshold %d", thresh[0]), "lep");

    for(int r=0; r<nruns; r++){
      int run=runs[r];
      hpeds[feb][r]->SetLineColor(colors[r+1]);
      hpeds[feb][r]->SetMarkerColor(colors[r+1]);
      hpeds[feb][r]->SetMarkerStyle(1);     
      hpeds[feb][r]->SetLineWidth(2.5);

      if(nruns>8){
	int cmod=0;
	if(volts[run]==114) cmod=-7;
	if(volts[run]==134) cmod=-5;
	if(volts[run]==154) cmod=-2;
	if(volts[run]==174) cmod=0;
	if(volts[run]==204) cmod=+2;
	if(volts[run]==225) cmod=+4;


	if(gains[run]==35) hpeds[feb][r]->SetLineColor(kBlue+cmod);
	if(gains[run]==41) hpeds[feb][r]->SetLineColor(kCyan+cmod);
	if(gains[run]==45) hpeds[feb][r]->SetLineColor(kGreen+cmod);
	if(gains[run]==49) hpeds[feb][r]->SetLineColor(kOrange+cmod);
	if(gains[run]==49) hpeds[feb][r]->SetLineColor(kRed+cmod);

	if(gains[run]==35) hpeds[feb][r]->SetMarkerColor(kBlue+cmod);
	if(gains[run]==41) hpeds[feb][r]->SetMarkerColor(kCyan+cmod);
	if(gains[run]==45) hpeds[feb][r]->SetMarkerColor(kGreen+cmod);
	if(gains[run]==49) hpeds[feb][r]->SetMarkerColor(kOrange+cmod);
	if(gains[run]==49) hpeds[feb][r]->SetMarkerColor(kRed+cmod);
      } 

      lab= Form("Run %d", run);
      if(gains[ run ] !=0 && doGainLabels) lab= Form("Gain %d, Bias %d", gains[ run ],  volts[ run ]);
      leg->AddEntry(hpeds[feb][r], lab, "lep");

      //leg->AddEntry(hpeds[feb][r], Form("Threshold %d", thresh[r]), "lep");
      if(r==0){
	hpeds[feb][0]->GetYaxis()->SetRangeUser(0, ymax);//(ymax+ymaxerr)*1.1 );
	hpeds[feb][0]->Draw("e1");
      }

      hpeds[feb][r]->Draw("same e1");
    }
    leg->Draw("same");
    //cout<<"Run string: "<<runstr.c_str()<<endl;
    string dirname=topdir+"/Averages_Runs_"+runstr;
    cout<<dirname.c_str()<<endl;
    if( 0 != system( Form( "test -d %s", dirname.c_str() ) ) )
      {
	int madedir = system( Form( "mkdir -m 777 -p %s", dirname.c_str() ) );
	if( 0 != madedir )
	  Error( "PedestalRunPlots", Form("Could not make directory '%s'", dirname.c_str() ) );
      }

    string dirname2=dirname+"/ChanelByChanelADCDists";
    if( 0 != system( Form( "test -d %s", dirname2.c_str() ) ) )
      {
	int madedir = system( Form( "mkdir -m 777 -p %s", dirname2.c_str() ) );
	if( 0 != madedir )
	  Error( "PedestalRunPlots", Form("Could not make directory '%s'", dirname2.c_str() ) );
      }

    gErrorIgnoreLevel=kWarning;
    
    string bname=Form("%s/Averages_Runs_%s/pedestals_feb%d", topdir.c_str(), runstr.c_str(), feb);
    c1->SaveAs( (bname+".png").c_str() );
    c1->SaveAs( (bname+".eps").c_str() );
    c1->SaveAs( (bname+".C").c_str() );


    TLegend* leg2=new TLegend(0.2, 0.7, 0.9,0.9);
    leg2->SetFillStyle(0);
    leg2->SetLineWidth(0);
    leg2->SetNColumns(4);

    for(int r=0; r<nruns; r++){
      int run=runs[r];
      TH1D* hsdv=(TH1D*) hpeds[feb][r]->Clone();
      for(int b=1; b<=hsdv->GetNbinsX(); b++){
	hsdv->SetBinContent( b, hsdv->GetBinError(b) );
      }
      hrms[feb].push_back(hsdv);
      hrms[feb][r]->GetYaxis()->SetRangeUser(0, 60);
      hrms[feb][r]->GetYaxis()->SetTitle("Pedestal RMS (ADC)");
      int cmod=0;
      if(volts[run]==114) cmod=-7;
      if(volts[run]==134) cmod=-5;
      if(volts[run]==154) cmod=-2;
      if(volts[run]==174) cmod=0;
      if(volts[run]==204) cmod=+2;
      if(volts[run]==225) cmod=+4;


      if(gains[run]==35) hrms[feb][r]->SetLineColor(kBlue+cmod);
      if(gains[run]==41) hrms[feb][r]->SetLineColor(kCyan+cmod);
      if(gains[run]==45) hrms[feb][r]->SetLineColor(kGreen+cmod);
      if(gains[run]==49) hrms[feb][r]->SetLineColor(kOrange+cmod);
      if(gains[run]==49) hrms[feb][r]->SetLineColor(kRed+cmod);
      //hrms[feb][r]->SetFillColor(colors[r+1]);
      //if(r<10) hrms[feb][r]->SetFillStyle( 3305 +10*r);
      //if(r>=10) hrms[feb][r]->SetFillStyle( 3351 + r%10);
      if(r==0) hrms[feb][r]->Draw("hist");
      else hrms[feb][r]->Draw("histsame");
      
      lab= Form("Run %d", runs[r]);
      if(gains[ run ] !=0 && doGainLabels) lab= Form("Gain %d, Bias %d", gains[ run ],  volts[ run ]);
      leg2->AddEntry(hrms[feb][r], lab, "l");
    }
    leg2->Draw("same");
    bname=Form("%s/Averages_Runs_%s/pedestalrms_feb%d", topdir.c_str(), runstr.c_str(), feb);
    c1->SaveAs( (bname+".png").c_str() );
    c1->SaveAs( (bname+".C").c_str() );
    

    for(int c=0; c<32; c++){
      hadc[feb][0][c]->SetLineColor(colors[1]);
      hadc[feb][0][c]->SetMarkerColor(colors[1]);
      hadc[feb][0][c]->SetLineWidth(2.5);
      hadc[feb][0][c]->SetTitle(Form("FEB %d, Channel %d", feb, c));
      hadc[feb][0][c]->GetYaxis()->SetRangeUser(0, 170);//400);
      hadc[feb][0][c]->GetXaxis()->SetRangeUser(0,400);
      hadc[feb][0][c]->GetYaxis()->SetTitle("N Events/hour");
      hadc[feb][0][c]->Draw("hist");

      for(int r=1; r<nruns; r++){
	int run=runs[r];
	hadc[feb][r][c]->SetLineColor(colors[r+1]);
	hadc[feb][r][c]->SetMarkerColor(colors[r+1]);
	hadc[feb][r][c]->SetLineWidth(2.5);
	hadc[feb][r][c]->Draw("same hist");
      }
      leg->Draw("same");
      string bname=Form("%s/pedestalDists_feb%d_chan%d", dirname2.c_str(), feb, c);
      //bname=Form("/sbnd/data/users/afilkins/crts/thresholds/feb%d_chan%d", feb, c);
      c1->SaveAs( (bname+".png").c_str() );
      c1->SaveAs( (bname+".C").c_str() );
    
    }
  } 
  string texfname=topdir+"/tex/RunsAndFEBIDs.tex";
  std::ofstream texfile;
  texfile.open(texfname);
  texfile<<"\\newcommand\\febs{";
  for(int feb: febs){
      texfile<<feb;
      if(feb!=febs[febs.size()-1]) texfile<<", ";
    }  
  texfile<<"}"<<endl;
  texfile<<"\\newcommand\\runstr{"<<runstr.c_str()<<"}"<<endl;
  texfile<<"\\newcommand\\runs{";
  for(int run: runs){
      texfile<<run;
      if(run!=runs[runs.size()-1]) texfile<<", ";
  }  
  texfile<<"}"<<endl;
  return 0;
}
