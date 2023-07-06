
// This is using ARRAY to store the ADC information. 
//void find_maximum_adc()
#include <TROOT.h>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
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

using namespace std; 

int main(int argc, char* argv[])
{
   if(argc==1){
     std::cout<<"Enter run number as arguments"<<std::endl;
     
   }
   int run=atoi(argv[1]);
   //gStyle->SetPalette(kRainBow);
   
   std::string topdir=getenv("CRTDIR");
   if(topdir=="") topdir="/sbnd/data/users/${USER}/crtModTests/";
   std::string plotdir = topdir+"/run"+std::to_string(run)+"/adcDists";
   if( 0 != system( Form( "test -d %s", plotdir.c_str() ) ) ) {
     int madedir = system( Form( "mkdir -m 777 -p %s", plotdir.c_str() ) );
     if( 0 != madedir )
       Error( "find_maximum_adc", Form("Could not make directory '%s'", plotdir.c_str() ) );  
   }

   gStyle->SetOptStat(0);
   TFile file(Form("%s/run%d.root", topdir.c_str(), run));
   TTreeReader reader("dumpall/events", &file);
   
   TTreeReaderValue<Int_t> Event_no(reader, "fEvent");
   TTreeReaderArray<Int_t> flags(reader, "flags"); 
   TTreeReaderArray<Int_t> mac5(reader, "mac5");
   TTreeReaderArray<unsigned int> ts0(reader, "ts0"); 
   TTreeReaderArray<unsigned int> ts1(reader, "ts1");  
   

   TTreeReaderArray<int> adc0(reader, "adc0");  
   TTreeReaderArray<int> adc1(reader, "adc1");  
   TTreeReaderArray<int> adc2(reader, "adc2");  
   TTreeReaderArray<int> adc3(reader, "adc3");  
   TTreeReaderArray<int> adc4(reader, "adc4");  
   TTreeReaderArray<int> adc5(reader, "adc5");  
   TTreeReaderArray<int> adc6(reader, "adc6");  
   TTreeReaderArray<int> adc7(reader, "adc7");  
   TTreeReaderArray<int> adc8(reader, "adc8");  
   TTreeReaderArray<int> adc9(reader, "adc9");  
   TTreeReaderArray<int> adc10(reader, "adc10");  
   TTreeReaderArray<int> adc11(reader, "adc11");  
   TTreeReaderArray<int> adc12(reader, "adc12");  
   TTreeReaderArray<int> adc13(reader, "adc13");  
   TTreeReaderArray<int> adc14(reader, "adc14");  
   TTreeReaderArray<int> adc15(reader, "adc15");  
   TTreeReaderArray<int> adc16(reader, "adc16");  
   TTreeReaderArray<int> adc17(reader, "adc17");  
   TTreeReaderArray<int> adc18(reader, "adc18");  
   TTreeReaderArray<int> adc19(reader, "adc19");  
   TTreeReaderArray<int> adc20(reader, "adc20");  
   TTreeReaderArray<int> adc21(reader, "adc21");  
   TTreeReaderArray<int> adc22(reader, "adc22");  
   TTreeReaderArray<int> adc23(reader, "adc23");  
   TTreeReaderArray<int> adc24(reader, "adc24");  
   TTreeReaderArray<int> adc25(reader, "adc25");  
   TTreeReaderArray<int> adc26(reader, "adc26");  
   TTreeReaderArray<int> adc27(reader, "adc27");  
   TTreeReaderArray<int> adc28(reader, "adc28");  
   TTreeReaderArray<int> adc29(reader, "adc29");  
   TTreeReaderArray<int> adc30(reader, "adc30");  
   TTreeReaderArray<int> adc31(reader, "adc31");  

   
   
   reader.SetEntry(0);
   int fhits= mac5.GetSize();
   std::vector<int>module_number;
   int nfebhits=25;
   for( int h=0; h<nfebhits; h++){
     int feb=mac5[ h* fhits/nfebhits];
     bool invec=false;
     for( int f: module_number){
       if( f==feb) invec=true;
       if(invec) break;
     }
     if(!invec) module_number.push_back(feb);
   }
   
   std::vector<int>addedfebs={};//can provide list of FEBs to force plots to be generated for, helpful if you have a module with way fewer hits
//{181, 71, 72, 75, 76, 153, 155, 156, 157, 158, 159, 182, 238};
//={134, 135, 136, 137, 149, 150, 151, 152, 71, 72, 75, 76};//={73,155};//
//
  for (int addedfeb : addedfebs){
    bool alreadyInVec=false;
    for(int f=0; f<module_number.size(); f++){
      if(module_number[f]==addedfeb ) alreadyInVec=true;
      if(alreadyInVec) break;
    }
    if(!alreadyInVec) module_number.push_back( addedfeb);
  }
   std::cout<<"Modules: ";
   for( int m: module_number) std::cout<<m<<", ";
   std::cout<<std::endl;
   const int module_numbers = module_number.size();
   //int module_number[module_numbers] = {73, 76, 72, 75, 71, 82};//{193, 87, 88, 85, 80, 81, 79, 89};
   std::vector<std::vector<std::vector<int>>> adc_info_of_modules_3d_vec(module_numbers); // depth: module, row: events, colums: (SiPM_no, adc, SiPM_no, adc)
   std::vector<std::vector<std::vector<int>>> adc_sum_of_modules_3d_vec(module_numbers);  // depth: module, row: events, colums: (SiPM_no, adc, SiPM_no, adc)
   //adc_inf_of_modules_3d_vec[feb][event][chan] ... I think
   int counter=0;
   while (reader.Next()) {
     
      for (int ihit=0; ihit<flags.GetSize(); ihit++){

         if (flags[ihit] != 3) continue; // flag == 3 means good run.

         int thisFeb=mac5[ihit];
         //int *p = std::find(std::begin(module_number), std::end(module_number), thisFeb);
         int index_in_module_number;
	 for(int m=0; m< module_numbers; m++) 
	   if(thisFeb==module_number[m]){
	     index_in_module_number=m;
	     break;
	   }
	 //std::distance(std::begin(module_number), p); // Find the module number. 
         
         if (index_in_module_number >= module_numbers) continue; // if couldn't find the targeted module, cut. 

         std::vector<int> this_adc_array;
         this_adc_array.push_back(adc0[ihit]); // can use std::format, but only c++20. 
         this_adc_array.push_back(adc1[ihit]);
         this_adc_array.push_back(adc2[ihit]);
         this_adc_array.push_back(adc3[ihit]);
         this_adc_array.push_back(adc4[ihit]);
         this_adc_array.push_back(adc5[ihit]);
         this_adc_array.push_back(adc6[ihit]);
         this_adc_array.push_back(adc7[ihit]);
         this_adc_array.push_back(adc8[ihit]);
         this_adc_array.push_back(adc9[ihit]);
         this_adc_array.push_back(adc10[ihit]);
         this_adc_array.push_back(adc11[ihit]);
         this_adc_array.push_back(adc12[ihit]);
         this_adc_array.push_back(adc13[ihit]);
         this_adc_array.push_back(adc14[ihit]);
         this_adc_array.push_back(adc15[ihit]);
         this_adc_array.push_back(adc16[ihit]);
         this_adc_array.push_back(adc17[ihit]);
         this_adc_array.push_back(adc18[ihit]);
         this_adc_array.push_back(adc19[ihit]);
         this_adc_array.push_back(adc20[ihit]);
         this_adc_array.push_back(adc21[ihit]);
         this_adc_array.push_back(adc22[ihit]);
         this_adc_array.push_back(adc23[ihit]);
         this_adc_array.push_back(adc24[ihit]);
         this_adc_array.push_back(adc25[ihit]);
         this_adc_array.push_back(adc26[ihit]);
         this_adc_array.push_back(adc27[ihit]);
         this_adc_array.push_back(adc28[ihit]);
         this_adc_array.push_back(adc29[ihit]);
         this_adc_array.push_back(adc30[ihit]);
         this_adc_array.push_back(adc31[ihit]);

         auto highest_adc_value = max_element(std::begin(this_adc_array), std::end(this_adc_array)); // find the maximum hits. 
         int adc_strip_no = std::distance(std::begin(this_adc_array), highest_adc_value);

         if (adc_strip_no%2 != 0){ // odd number
            // FEB number, adc_channel_no, adc_number, adc_channel_no, adc_number
            std::vector <int> this_event_trig_info{adc_strip_no-1, this_adc_array.at(adc_strip_no-1), adc_strip_no, *highest_adc_value};
            adc_info_of_modules_3d_vec.at(index_in_module_number).emplace_back(this_event_trig_info);
            
            // add two strips together.             
            std::vector <int> this_event_strip_info{ (adc_strip_no-1)/2, (this_adc_array.at(adc_strip_no-1)+ *highest_adc_value)};
            adc_sum_of_modules_3d_vec.at(index_in_module_number).emplace_back(this_event_strip_info);
         }else{ // even number
            std::vector <int> this_event_trig_info{adc_strip_no, *highest_adc_value, adc_strip_no+1, this_adc_array.at(adc_strip_no+1)};
            adc_info_of_modules_3d_vec.at(index_in_module_number).emplace_back(this_event_trig_info);

            // add two strips together.             
            std::vector <int> this_event_strip_info{ adc_strip_no/2, (this_adc_array.at(adc_strip_no+1)+ *highest_adc_value)};
            adc_sum_of_modules_3d_vec.at(index_in_module_number).emplace_back(this_event_strip_info);
         }

      }
      counter++;
      if(counter>10000){
	std::cout<<"Entry>10000" <<std::endl;
	break;
      }
   }// end of run. 
   
   TCanvas* c1=new TCanvas("c1", "c1", 1280, 840);
   c1->cd();
   gErrorIgnoreLevel=kWarning;
   //std::vector<TH2D*> adc2d;
   //for( int m: module_number){
   // TH2D* hadc2d=new TH2D( Form("adcs_feb%d", m), Form("FEB %d; ADC; Channel; Entries", m),  100, 0, 4200, 32, 0, 31);
      
   //}
   map<int, vector<double> > sum, sumsq;
   map<int, vector<int> > nhits;
   
   for(auto i = 0; i != adc_info_of_modules_3d_vec.size(); i++) // loop-through modules. 
   {  
      int feb=module_number[i];
      for(int c=0; c<32; c++){sum[feb].push_back(0.); sumsq[feb].push_back(0.); nhits[feb].push_back(0);  }
      TString out_file_name = Form("%s/Module_%d.root", plotdir.c_str(), module_number[i]);
      TFile *outfile = new TFile(out_file_name,"RECREATE");
      outfile->cd();

      std::vector<TH1F *> adc_1d_distributions;
      TH2D* hadc2d=new TH2D( Form("adcs_feb%d", module_number[i]), Form("FEB %d; ADC; Channel; Entries", module_number[i]),  100, 0, 4200, 32, 0, 32);
      for (int sipm_no = 0; sipm_no < 32; sipm_no ++){
         TString title_temp = Form("h_adc_sipm_%d", sipm_no);
         TH1F *h_temp = new TH1F(title_temp,"", 100, 0, 4200); 
         //TH1F *h_temp = new TH1F(title_temp,"", 80, 300, 700); 
         TString title= Form("Channel: %d", sipm_no);
         h_temp->SetTitle(title);  
         h_temp->GetXaxis()->SetTitle("ADC");h_temp->SetStats(0);
         adc_1d_distributions.push_back(h_temp);

      }

      for (auto j=0; j != adc_info_of_modules_3d_vec.at(i).size(); j++){ //loop through events.
         for (auto k=0; k != adc_info_of_modules_3d_vec.at(i).at(j).size(); k++){ // 1d vector was structed as SiPM_no, adc, SiPM_no, adc. 
            if (k%2 == 0) {
               int index = adc_info_of_modules_3d_vec.at(i).at(j).at(k);
               int adc_value = adc_info_of_modules_3d_vec.at(i).at(j).at(k+1);
               //if (adc_value>4080) continue;
               adc_1d_distributions.at(index)->Fill(adc_value);
	       int chan=adc_info_of_modules_3d_vec[i][j][k];//I think thats what shes saying?
	       hadc2d->Fill(adc_value, chan+.5);
	       if(adc_value<4000){
		 sum[feb][chan]+=adc_value;
		 sumsq[feb][chan]+=adc_value*adc_value;
		 nhits[feb][chan]++;
	       }
            }
         }
      }
      for( int c=0; c<32; c++){
	adc_1d_distributions[c]->Draw();
	c1->SaveAs(Form("%s/adcs_module%d_chan%d.png", plotdir.c_str(), module_number[i], c));
      }
      hadc2d->SetContour(51);
      int max=hadc2d->GetMaximum();
      //hadc2d->GetZaxis()->SetRangeUser(0., max*0.5);
      hadc2d->GetZaxis()->SetTitle("Entries");
      //hadc2d->SetPalette(kRainBow);
      hadc2d->Draw("colz");
      c1->SaveAs(Form("%s/h2d_adcs_module%d_woverflow.png", plotdir.c_str(), module_number[i]));
      hadc2d->Write();
      int mbin=hadc2d->GetXaxis()->FindBin(4100);
      for(int b=0; b<=hadc2d->GetNbinsY()+1; b++){
	hadc2d->SetBinContent(mbin, b, 0);
      }
      hadc2d->Draw("colz");
      c1->SaveAs(Form("%s/h2d_adcs_module%d.png", plotdir.c_str(), module_number[i]));      
      
      for(auto this_adc_distribution : adc_1d_distributions){
	this_adc_distribution->Write(); // write into files. 
	
      }
      outfile->Close();
   }
   std::ofstream txtfile, txtfile2;
   std::string dirname=topdir+"/run"+std::to_string(run);
   txtfile.open(Form("%s/febs.csv", dirname.c_str() ), std::ofstream::out);

   TString out_file_name = Form("%s/MeanandRMSAll%d.root", plotdir.c_str());
   TFile *outfile = new TFile(out_file_name,"RECREATE");
   outfile->cd();

   TH2D* mean2d=new TH2D("hmean_febvchan", Form("Run %d;FEB; Channel; Mean ADC", run), module_number.size(), 0 , module_number.size(), 32, 0, 32);
   TH2D* rms2d=new TH2D("hrms_febvchan", Form("Run %d;FEB; Channel; RMS (ADC)", run), module_number.size(), 0 , module_number.size(), 32, 0, 32);
   sort(module_number.begin(), module_number.end());
   
			 
   for( int feb: module_number) txtfile<<feb<<", ";
   txtfile2.open(Form("%s/meansrms.csv", dirname.c_str() ), std::ofstream::out);
   txtfile2<<" FEB, \t Chan, \t Mean, \t RMS;"<<endl;   
   //for( int feb: module_number){
   c1->SetRightMargin(0.16);
   mean2d->SetContour(99);
   rms2d->SetContour(99);
   for( int f=0; f<module_number.size();f++){
     int feb=module_number[f];
     mean2d->GetXaxis()->SetBinLabel(f+1, to_string(feb).c_str() );
     rms2d->GetXaxis()->SetBinLabel(f+1, to_string(feb).c_str() );
     for( int chan=0; chan<32; chan++){
       double mean=sum[feb][chan]/nhits[feb][chan];
       double rms2= nhits[feb][chan]* mean*mean - 2 * mean*sum[feb][chan] + sumsq[feb][chan];
       //cout<<"rms2: "<<rms2<<endl;
       double rms=sqrt(rms2/nhits[feb][chan]);
       mean2d->SetBinContent( f+1, chan+1, mean);
       mean2d->SetBinError( f+1, chan+1, rms);
       rms2d->SetBinContent( f+1, chan+1, rms);
       txtfile2<<feb<<", \t "<<chan<<", \t"<<mean<< ", \t"<< rms<<";"<<endl ;
     }
   }
   mean2d->GetZaxis()->SetRangeUser(0, 3500);
   rms2d->GetZaxis()->SetRangeUser(0, 2000);
   mean2d->Draw("colz");
   c1->SaveAs(Form("%s/meanADC_allfebs.png",plotdir.c_str()) );
   rms2d->Draw("colz");
   c1->SaveAs(Form("%s/rmsADC_allfebs.png",plotdir.c_str() ));
   outfile->Write();

   return 0;
}
