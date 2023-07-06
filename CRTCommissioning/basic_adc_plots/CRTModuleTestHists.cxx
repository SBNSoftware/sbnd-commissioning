#include <TROOT.h>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TCanvas.h"
#include <vector>
#include <iostream>
#include <TF1.h>
#include <TChain.h>
#include <TStyle.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <experimental/filesystem>
#include "PlotStyle.h"

using namespace std;

//void plot(){
int main(int argc, char* argv[]) {
  int run;
  bool makeADCplots=true;//false;
  bool doGainLabels=true;
  if(argc==1){
    cout<<"Using Run 4394 as default. Run with argument for other runs. Expects berncrtana file to have name of form bercrt_hist_run[run#].root"<<endl;
    return 0;
    
  }
  else{
    run=atoi(argv[1]);
  }
  SetRootEnv();
  cout<<"starting script"<<endl;
  vector<int> runs;
  string runstr=to_string(run);
  for(int i=1; i<argc; i++){
    cout<<"irun: "<< i<<endl;
    int runtmp=atoi(argv[i]);
    runs.push_back( runtmp );
    if(i!=1) runstr+="_"+runtmp;
    cout<<runs[i-1]<<endl;
  }
    //fnames.push_back( argv[i]);
  std::ifstream finfo("runinfo.csv");
  string gainstr;
  map<int, int> gains, volts;
  if(doGainLabels){
    getline(finfo, gainstr);
    while( getline(finfo , gainstr)){
      if (gainstr.size()==0) continue;
      int indx0=gainstr.find(":") ;
      int indx1=gainstr.find(",") ;
      //string runstr2=gainstr.substr(0, indx0)   ;
      //string voltstr=gainstr.substr( indx0+1 ,  /*len=*/indx1-indx0-1   ) ;
      //string gainstr2=gainstr.substr( indx1+1, -1) ;
      //cout<< "run: "<<runstr2.c_str()<< "\t voltstr: "<<voltstr.c_str()<<"\t gainstr: "<< gainstr2.c_str()<<endl;
      int run=stoi( gainstr.substr(0, indx0)   );
      volts[run]=stoi( gainstr.substr( indx0+1 ,  /*len=*/indx1-indx0-1   ) );
      gains[run]=stoi( gainstr.substr( indx1+1) );
    }
  }

    //TFile* f= new TFile(fname);
  //cout<<"Opened: /sbnd/data/users/afilkins/crts/run4394_thesh300_bernana.root"<<endl;
  TChain* thits=new TChain("berncrtana/hits");//= (TChain*) f->Get("berncrtana/hits");
  //if(run==2100) thits->Add("/sbnd/data/users/afilkins/crts/berncrt_hist_run21*.root");
  //else thits->Add(fname);
  //cout<<"fname: "<< fname<<endl;
  string topdir=getenv("CRTDIR");
  if(topdir=="") topdir="/sbnd/data/users/${USER}/crtModTests";
  for(int i=0; i<runs.size(); i++){
    TString fname=Form("%s/berncrt_hist_run%d.root", topdir.c_str() , runs[i]);
    thits->Add(fname );
    cout<<"Added "<<fname.Data()<<" to TChain";
  }
  cout<<"loaded tree"<<endl;
  int nchan=32;
  unsigned char mac5;
  unsigned int t0;
  unsigned long long timestamp, run_start_time;

  unsigned short adc[nchan];
  bool dotimecut=false;
  if( run==4394) dotimecut=true;
  if( run>2100 && run<2200) dotimecut=true;
  if(run==4492) dotimecut=true;
  thits->SetBranchAddress("mac5", &mac5);
  thits->SetBranchAddress("ts0", &t0);
  thits->SetBranchAddress("adc", &adc);
  //unsigned short flags1;
  unsigned short flags;
  //thits->SetBranchAddress("flags", &flags1);
  thits->SetBranchAddress("flags", &flags);
  thits->SetBranchAddress("timestamp", &timestamp);
  thits->SetBranchAddress("run_start_time", &run_start_time);

  TCanvas* c1= new TCanvas;
  cout<<"hard coding 1st test aframe febs"<<endl;
  
  vector<int> febs;//={153, 154, 155, 156, 157, 158, 159, 181, 182};//{92, 73,75,76,72,71,82};//={87, 193, 88, 85, 81, 80, 79, 89};//{193, 
  map<int, vector<TH1D*>> hadcs, hadcs_flag3;
  map<int, TH1D*> meanpeds;
  cout<<"Getting entries"<<endl;
  int nentries=thits->GetEntries();
  //if(nentries>10000) nentries=10000;
  thits->GetEntry(0);
  
  //cout<<"l39: flag0= "<<flags1<<endl;
  string dirname=topdir+"/run"+runstr;

  if( 0 != system( Form( "test -d %s", dirname.c_str() ) ) )
    {
      int madedir = system( Form( "mkdir -m 777 -p %s", dirname.c_str() ) );
      if( 0 != madedir )
	Error( "CRTModuleTestHists", Form("Could not make directory '%s'", dirname.c_str() ) );

    }
  string adcplotdir=dirname+"/ped_adc_plots";
  if( 0 != system( Form( "test -d %s", adcplotdir.c_str() ) ) )
    {
      int madedir = system( Form( "mkdir -m 777 -p %s", adcplotdir.c_str() ) );
      if( 0 != madedir )
	Error( "CRTModuleTestHists", Form("Could not make directory '%s'", adcplotdir.c_str() ) );

    }

  //  if(!fs::is_directory( dirname.c_str() ) || !fs::exists( dirname.c_str() ) )
  //  fs::create_directory( dirname.c_str() );
  TFile *fout=new TFile(Form("%s/pedestals.root", dirname.c_str() ), "RECREATE");
  std::ofstream txtfile; 
  txtfile.open(Form("%s/run%s_pedestals.csv", dirname.c_str(), runstr.c_str()), std::ofstream::out);


  fout->cd();
  cout<<"Initializing histograms"<<endl;
  map<int, vector<double> > sum, sqrs;//, avg, rms;
  //map<int, vector< vector<double>>> evAdcs;
  map<int, vector<int> > nhits;
  
  if( 0 == system( Form( "test -f %s/febs.csv", dirname.c_str()  ) ) ){
    string febcsvname=dirname+"/febs.csv";
    cout<<"Looking at "<<febcsvname.c_str()<<endl;
    std::ifstream infile(febcsvname.c_str() );
    string line;
    getline(infile, line);
    int indx=0;
    //cout<<"line legnth"<<line.length()<<endl;
    while(indx<line.length()){
      int indx2=line.find(',',indx+1);
      string febstr= (indx==0)? line.substr(indx, indx2-indx) : line.substr(indx+2, indx2-indx -2);//2 extra chars for space and comma    
      indx=indx2;
      //cout<<"FEB: "<< febstr.c_str()<< "\t size: "<<febstr.length()<<endl;
      if(febstr=="") break;
      try{ febs.push_back( stoi(febstr) );  }
      catch(std::exception e){ cout<<"Input CSV file doesn't have expected format...Found FEB# "<<febstr.c_str(); break;}
    }
    //cover febs that dont get reconstructed in the other tree
    int m5Entries=(nentries<100)? nentries: 300;
    for(int iEntry=0; iEntry<m5Entries; iEntry++){
      int step=m5Entries/nentries;
      thits->GetEntry(iEntry* step );
      //try and find all the febs... lets hope thats enough entries
      if(febs.size()==0) febs.push_back( int(mac5) );
      else{
	bool alreadyInVec=false;
	for(int f=0; f<febs.size(); f++){
	  if(febs[f]==int(mac5) ) alreadyInVec=true;
	  if(alreadyInVec) break;
	}
	if(!alreadyInVec) febs.push_back( int(mac5));
      }
    }
  }
  else{
    int m5Entries=(nentries<100)? nentries: 200;
    for(int iEntry=0; iEntry<m5Entries; iEntry++){
      int step=m5Entries/nentries;
      thits->GetEntry(iEntry* step );
      //try and find all the febs... lets hope thats enough entries
      if(febs.size()==0) febs.push_back( int(mac5) );
      else{
	bool alreadyInVec=false;
	for(int f=0; f<febs.size(); f++){
	  if(febs[f]==int(mac5) ) alreadyInVec=true;
	  if(alreadyInVec) break;
	}
	if(!alreadyInVec) febs.push_back( int(mac5));
      }//end else for vec with at least one element
    }//end initial entry loop
  }  

  ///I keep missing febs let fix that
  //hard coded febs
  vector<int>addedfebs= {71, 72, 75, 76, 153, 155, 156,  157,  158,  159, 181, 182, 238};//={181, 71, 72, 73, 75, 76, 157};
  for (int addedfeb : addedfebs){
    bool alreadyInVec=false;
    for(int f=0; f<febs.size(); f++){
      if(febs[f]==addedfeb ) alreadyInVec=true;
      if(alreadyInVec) break;
    }
    if(!alreadyInVec) febs.push_back( addedfeb);
  }

  sort(febs.begin(), febs.end() );
  cout<<"Filled the feb vector; size: "<<febs.size()<<endl;
  cout<<"FEBS: ";
  TH2D* hped_febvchan=new TH2D("hped_febvchan", Form("Run %s;FEB; Channel;Pedestal (ADC)", runstr.c_str() ), febs.size(), 0, febs.size(), nchan, 0, nchan); 
  TH2D* hped_febvchanRMS=new TH2D("hped_febvchanRMS", Form("Run %s;FEB; Channel; Pedestal RMS (ADC)",runstr.c_str() ), febs.size(), 0, febs.size(), nchan, 0, nchan); 

  for( int f=0; f<febs.size(); f++){
    //hadcs[feb]= vector<TH1D*>;
    int feb=febs[f];
    cout<<feb<<"\t";
    TString name=Form("pedestal_v_channel_feb%d", febs[f]);
    TH1D* meanped=new TH1D(name, Form("FEB %d;Channel;Pedestal (ADC)",febs[f]), nchan, 0, nchan); 
    meanpeds[feb]=meanped; 
    for( int chan=0; chan<nchan; chan++){
      name=Form("adc_feb_%d_channel_%d",febs[f], chan);
      TH1D* hadc=new TH1D("pedestal_"+name, "pedestal_"+name, 100, 0, 500);
      TH1D* hadc_flag3=new TH1D(name+"_flag3", name, 500, 0, 5000);
      hadcs[feb].push_back(hadc);
      hadcs_flag3[feb].push_back(hadc_flag3);
      sum[feb].push_back(0.);
      sqrs[feb].push_back(0.);
      nhits[feb].push_back(0);
    }
  }  
  cout<<""<<endl;
  cout<<"Starting event loop"<<endl;
  //nentries=250;
  if(nentries>1e6) nentries=1e6;
  cout<<"Total Number of Entries: "<<nentries<<endl;
  double totRunTime=0;

  for(int iEntry=0; iEntry<nentries; iEntry++){
    if(iEntry%100000==0) cout<<iEntry/1000<<"k \t";
    thits->GetEntry(iEntry);
    //int flags=flag;
    //cout<<"flags l52: "<<flags<<endl;
    int i=-1; 
    double rtime= (timestamp-run_start_time)*1e-9/3600;
    if(rtime>totRunTime) totRunTime=rtime;
    for( int f=0; f<febs.size(); f++){
      int feb=febs[f];
      for( int chan=0; chan<nchan; chan++){
	i++;
	if (int(mac5)!= febs[f]) continue;
	if (rtime > 3.0 && dotimecut) continue;
	if( flags==3) hadcs_flag3[feb][chan]->Fill(adc[chan]);
	if (!(flags==7 || flags==11)) continue;
	hadcs[feb][chan]->Fill(adc[chan]);
	//if( adc[chan]>1000) continue;// Set some threshold indicating that theres other activity happening to exclude from the average and rms
	sum[feb][chan]+=adc[chan];
	sqrs[feb][chan]+=adc[chan]*adc[chan];
	nhits[feb][chan]++;
	//evAdcs[feb][chan].push_back(adc[chan]);
      }
    }
  }
  //thits->Draw("flags");
  //c1->SaveAs("/sbnd/data/users/afilkins/crts/flags2.png");
  
  
  //Actual plotting part....should probably be a different script.

  //TCanvas* c1= new TCanvas;
  if(doGainLabels && gains[run]!=0 )txtfile<<"Gain "<<gains[run]<<"-- Bias "<<volts[run]<<" -- ";
  txtfile<<"FEB, \t"<<"Channel, \t"<<"Mean, \t"<<"RMS;"<<endl;
  gErrorIgnoreLevel=kWarning;
  cout<<"Making and writing plots"<<endl;
  for( int f=0; f<febs.size(); f++){
    int feb=febs[f];
    hped_febvchan->GetXaxis()->SetBinLabel(f+1, to_string(feb).c_str() );
    hped_febvchanRMS->GetXaxis()->SetBinLabel(f+1, to_string(feb).c_str() );
    for( int chan=0; chan<nchan; chan++){
      double avg=sum[feb][chan]/nhits[feb][chan];
      //double rms2=0.0;
      //for(int i=0; i<evAdcs[feb][chan].size(); i++){
      //	rms2+= pow(avg - evAdcs[feb][chan][i] , 2);
      //}
      //double rms=sqrt(rms2/nhits[feb][chan]);
      double rms2= nhits[feb][chan]*avg*avg - 2 * avg* sum[feb][chan] + sqrs[feb][chan] ;
      double rms=sqrt(rms2/nhits[feb][chan]);
      txtfile<<feb<<", \t"<<chan<<", \t"<<avg<<", \t"<<rms<<";"<<endl;
      meanpeds[feb]->SetBinContent(chan+1, avg);
      meanpeds[feb]->SetBinError(chan+1, rms);

      hped_febvchan->SetBinContent(f+1, chan+1, avg);
      hped_febvchan->SetBinError(f+1, chan+1, rms);
      hped_febvchanRMS->SetBinContent(f+1, chan+1, rms);

      hadcs[feb][chan]->GetXaxis()->SetTitle("Pedestal ADC");
      hadcs[feb][chan]->GetYaxis()->SetTitle("N Events");
      hadcs[feb][chan]->SetTitle(Form("FEB %d, Channel %d, Run %d", feb, chan, run));
      //hadcs[feb][chan]->Scale(1./totRunTime);


      string bname=Form("%s/adc_feb%d_chan%d", adcplotdir.c_str(), feb, chan);
      if(makeADCplots){
	hadcs[feb][chan]->Draw();
	c1->SaveAs( (bname+"_pedestals.png").c_str() );
	c1->SaveAs( (bname+"_pedestals.C").c_str() );
      }
      /*      hadcs_flag3[feb][chan]->GetXaxis()->SetTitle("ADC (flag=3)");
      hadcs_flag3[feb][chan]->GetYaxis()->SetTitle("N Events");
      hadcs_flag3[feb][chan]->SetTitle(Form("FEB %d, Channel %d, Run %d", feb, chan, run));
      hadcs_flag3[feb][chan]->Scale(1./totRunTime);
      if(makeADCplots){
	hadcs_flag3[feb][chan]->Draw();
	c1->SaveAs( (bname+"_flag3.png").c_str() );
	c1->SaveAs( (bname+"_flag3.C").c_str() );
	}*/
    }
  }
  gStyle->SetOptStat(0);

 
  
  TCanvas* c2=new TCanvas("c2", "c2", 1240, 800);
  c2->SetRightMargin(0.2);
  string bname=Form("%s/pedestal_FEBvChan", dirname.c_str());
  hped_febvchan->SetContour(100);
  //hped_febvchan->GetZaxis()->SetTitleOffset(1.1);
  hped_febvchan->GetZaxis()->SetRangeUser(0,500);
  //cout<<"l324"<<endl;
  Apply2DStyle(hped_febvchan);
  //cout<<"l326"<<endl;
  gStyle->SetPalette(kRainBow);//Thermometer);
  hped_febvchan->Draw("colz");
  c2->SaveAs( (bname+".png").c_str() );
  c2->SaveAs( (bname+".C").c_str() );
 
  hped_febvchanRMS->SetContour(100);
  //hped_febvchanRMS->GetZaxis()->SetTitleOffset(1.1);
  //cout<<"l334"<<endl;
  hped_febvchanRMS->GetZaxis()->SetRangeUser(0,50);
  //cout<<"l336"<<endl;
  Apply2DStyle(hped_febvchanRMS);
cout<<"l338"<<endl;
  //hped_febvchanRMS->GetZaxis()->SetTitleOffset(1.5);
  hped_febvchanRMS->Draw("colz");
  bname=Form("%s/rms_pedestal_FEBvChan", dirname.c_str());
  c2->SaveAs( (bname+".png").c_str() );
  c2->SaveAs( (bname+".C").c_str() );

  fout->Write();
  return 0;
}
