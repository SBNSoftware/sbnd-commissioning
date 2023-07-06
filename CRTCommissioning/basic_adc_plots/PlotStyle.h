#include "TGaxis.h"
//-- chi2 calculation                                                                     
bool chi2_use_overflow_err; ///< Do you want to consider correlations to under/overflow bins in chi2 calculation?                                                                        

//! do you want to draw histograms normalized to bin width                                
bool draw_normalized_to_bin_width;

//-- marker settings                                                                      
int data_marker;
int ratio_marker;
double data_marker_size;
double ratio_marker_size;

//-- line settings                                                                        
int data_line_width;
int data_line_style;
int mc_line_width;
int mc_line_style;
int ratio_line_width;

//-- cut arrow settings                                                                   
int arrow_line_width;
int arrow_line_style;
int arrow_line_color;

float arrow_size;
std::string arrow_type;

//-- color settings                                                                       
int palette_style;

int data_color;
int mc_color;
int mc_error_color;
int mc_error_style;
int ratio_color;

int mc_bkgd_color;
int mc_bkgd_width;
int mc_bkgd_line_color;
int mc_bkgd_style;

int data_bkgd_color;
int data_bkgd_style;
double data_bkgd_size;

//-- correlation/covariance                                                               
bool draw_corr_max1;
bool draw_corr_red_blue;
unsigned int n_color_contours;

//-- title settings                                                                       
int title_font;
double title_size;

//-- axis options                                                                         
bool   hist_min_zero;        ///< argument to SetHistMinimumZero()                        
bool   axis_draw_grid_x;     ///< Set to true to get grid drawn for X axis                
bool   axis_draw_grid_y;     ///< Set to true to get grid drawn for Y axis                
int    axis_max_digits;
int    axis_title_font_x;
int    axis_title_font_y;
int    axis_title_font_z;
double axis_title_offset_x;
double axis_title_offset_y;
double axis_title_offset_z;
double axis_title_size_x;
double axis_title_size_y;
double axis_title_size_z;
double axis_minimum;        ///< argument to SetMinimum() use -1111 for automatic scale   
double axis_maximum;        ///< argument to SetMinimum() use -1111 for automatic scale   
double axis_maximum_group;  ///< argument to SetMinimum() use -1111 for automatic scale   

//-- axis label options                                                                   
int    axis_label_font;
double axis_label_size;

//-- margins                                                                              
double extra_top_margin;
double extra_bottom_margin;
double extra_left_margin;
double extra_right_margin;

//-- layout                                                                               
double headroom;
double footroom; //because legroom made be think of room for a TLegend...                 

//-- legend                                                                               
double height_nspaces_per_hist;
double width_xspace_per_letter;
int    legend_text_font;
int    legend_fill_color;
int    legend_border_size;
double legend_text_size;
double legend_offset_x;
double legend_offset_y;
unsigned int legend_n_columns;


void setStyle(){
  chi2_use_overflow_err = false;

  //-- do you want to draw histograms normalized to bin width                                 
  draw_normalized_to_bin_width = true;

  //-- rainbow                                                                                
  //palette_style = 1;
  //kBird=57, kCool=109, newer kRainBow=55
  palette_style=55;//57;

  //-- marker settings                                                                        
  data_marker = 20;
  ratio_marker = 20;
  data_marker_size = 1.0;
  ratio_marker_size = 1.0;

  //-- line settings                                                                          
  data_line_width = 1;
  data_line_style = 1;
  mc_line_width = 3;
  mc_line_style = 1;
  ratio_line_width = 3;

  //-- cut arrow settings                                                                     
  arrow_line_width = 4;
  arrow_line_style = 1;
  arrow_line_color = 1;
  arrow_size = 0.01;
  arrow_type = "|>";

  //-- color settings                                                                         
  data_color = 1;
  mc_color   = 2;
  mc_error_color = kRed-10;//45;                                                              
  mc_error_style = 1001;
  ratio_color = 1;

  mc_bkgd_color = 14;
  mc_bkgd_width = 1;
  mc_bkgd_line_color = 1;
  mc_bkgd_style = 3005;
  data_bkgd_color = 12;//gray                                                                 
  data_bkgd_style = 24;//circle                                                               
  data_bkgd_size   = 1.;

  //-- correlation                                                                            
  draw_corr_max1     = false;
  draw_corr_red_blue = true;

  //-- title settings                                                                         
  title_font    = 62;
  title_size = 0.06;

  //-- axis options                                                                           
  hist_min_zero     = true;
  axis_draw_grid_x = false;
  axis_draw_grid_y = false;
  axis_max_digits   = 3;
  axis_title_font_x = 62;
  axis_title_font_y = 62;
  axis_title_font_z = 62;
  axis_title_offset_x = 1.15;
  axis_title_offset_y = 1.2;
  axis_title_offset_z = .75;
  axis_title_size_x = 0.06;
  axis_title_size_y = 0.06;
  axis_title_size_z = 0.06;
  //axis_minimum      = MnvHist::AutoAxisLimit;
  //axis_maximum      = MnvHist::AutoAxisLimit;
  //axis_maximum_group= MnvHist::AutoAxisLimit; //0.5;                                          

  //-- axis label options                                                                     
  axis_label_font = 42;
  axis_label_size = 0.05;
  //-- margins                                                                                
  extra_bottom_margin = 0.;
  extra_left_margin   = 0.;
  extra_right_margin  = -0.50;

  //-- layout                                                                                 
  headroom = 1.5;  //old 1.5                                                                  
  footroom = 1.25;  //old 1                                                                   

  //-- legend                                                                                 
  height_nspaces_per_hist = 2.;
  width_xspace_per_letter = .5;
  legend_border_size      = 0;
  legend_fill_color       = -1;
  legend_text_size        = .035;
  legend_offset_x         = 0.;
  legend_offset_y         = 0.;
  legend_n_columns        = 1;
  legend_text_font        = 62;


  extra_top_margin = -.035; //go slightly closer to top of pad                                

  mc_bkgd_color = 46;
  mc_bkgd_line_color = 46;

  data_bkgd_color = 12;//gray                                                                 
  data_bkgd_style = 24;//circle                                                               

  //legend entries are closer                                                                 
  height_nspaces_per_hist = 1.2;
  width_xspace_per_letter = .4;
  legend_text_size        = .03;

  //========nukecc=========
  data_marker_size = ratio_marker_size = data_bkgd_size = 1.75;
  data_line_width = 5; //make these thicker now that everyone has 4k UHD monitors...          
  mc_line_width = 5; //make these thicker now that everyone has 4k UHD monitors...            
  mc_bkgd_style = 3002; //change the hatched histogram                                        
  data_bkgd_style = 22;//triangle                                                             
  //gStyle->SetEndErrorSize(0.0); //DO NOT WANT                                               
  gStyle->SetEndErrorSize(5);
  legend_text_size = .04;

  axis_title_offset_y = 1.;// less white space                                                
  axis_draw_grid_x = false;//true;                                                            
  axis_draw_grid_y = false;//true;                                                            
  legend_border_size      = 0;//1;                                                            
  legend_fill_color       = 10;
}

void SetRootEnv()
{
  setStyle();
  gStyle->SetPalette(palette_style);

  // Canvas Styles                                                                            
  gStyle->SetCanvasDefW(900);
  gStyle->SetCanvasDefH(750);
  gStyle->SetOptStat(0000);
  gStyle->SetOptFit(0000);
  //gStyle->SetOptTitle(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetHistLineWidth(2);

  // Axis Styles      
  gStyle->SetTitleFontSize(title_size);
  gStyle->SetHistMinimumZero( hist_min_zero );
  gStyle->SetTitleOffset( axis_title_offset_x, "X" );
  gStyle->SetTitleSize( axis_title_size_x, "X" );
  gStyle->SetTitleFont( axis_title_font_x, "X" );
  gStyle->SetTitleOffset( axis_title_offset_y, "Y" );
  gStyle->SetTitleSize( axis_title_size_y, "Y" );
  gStyle->SetTitleFont( axis_title_font_y, "Y" );
  gStyle->SetTitleOffset( 1.5,"z");//axis_title_offset_z, "Z" );
  gStyle->SetTitleSize( axis_title_size_z, "Z" );
  gStyle->SetTitleFont( axis_title_font_z, "Z" );
  gStyle->SetLabelFont( axis_label_font, "XYZ" );
  gStyle->SetLabelSize( axis_label_size, "XYZ" );
  TGaxis::SetMaxDigits(axis_max_digits);
  gStyle->SetPadGridX( axis_draw_grid_x );
  gStyle->SetPadGridY( axis_draw_grid_y );

  // Marker Styles                                                                            
  gStyle->SetMarkerStyle(data_marker);
  gStyle->SetMarkerSize(data_marker_size);
  gStyle->SetMarkerColor(data_color);

  //gStyle->SetEndErrorSize(2);                                                               
  gStyle->SetErrorX(0.5);
}

void ApplyDataStyle(TH1* tmpData){
  SetRootEnv();
  tmpData->SetMarkerStyle(data_marker);
  tmpData->SetMarkerSize(data_marker_size);
  tmpData->SetMarkerColor(data_color);
  tmpData->SetLineWidth(data_line_width);
  tmpData->SetLineStyle(data_line_style);
  tmpData->SetLineColor(data_color);
}

void ApplyMCStyle(TH1* tmpMC){
  SetRootEnv();
  tmpMC->GetXaxis()->SetTitleFont(axis_title_font_x);
  tmpMC->GetYaxis()->SetTitleFont(axis_title_font_y);
  tmpMC->GetXaxis()->SetTitleSize(axis_title_size_x);
  tmpMC->GetYaxis()->SetTitleSize(axis_title_size_y);
  tmpMC->GetXaxis()->SetLabelFont(axis_label_font);
  tmpMC->GetYaxis()->SetLabelFont(axis_label_font);
  tmpMC->GetXaxis()->SetLabelSize(axis_label_size);
  tmpMC->GetYaxis()->SetLabelSize(axis_label_size);
  tmpMC->GetXaxis()->CenterTitle(kTRUE);

  tmpMC->SetLineColor(mc_color);
  tmpMC->SetLineWidth(mc_line_width);
  tmpMC->SetLineStyle(mc_line_style);

}

void Apply2DStyle(TH2* hist){
  SetRootEnv();
  gStyle->SetPadRightMargin(0.2);
  hist->GetXaxis()->SetTitleFont(axis_title_font_x);
  hist->GetYaxis()->SetTitleFont(axis_title_font_y);
  hist->GetZaxis()->SetTitleFont(axis_title_font_z);
  std::cout<<"PlotStyle:l309"<<std::endl;
  hist->GetXaxis()->SetTitleSize(axis_title_size_x);
  hist->GetYaxis()->SetTitleSize(axis_title_size_y);
  hist->GetZaxis()->SetTitleSize(axis_title_size_z);
  std::cout<<"PlotStyle:l312"<<std::endl;
  hist->GetXaxis()->SetLabelFont(axis_label_font);
  hist->GetYaxis()->SetLabelFont(axis_label_font);
  hist->GetZaxis()->SetLabelFont(axis_label_font);
  hist->GetXaxis()->SetLabelSize(axis_label_size);
  hist->GetYaxis()->SetLabelSize(axis_label_size);
  hist->GetZaxis()->SetLabelSize(axis_label_size);
  hist->GetXaxis()->CenterTitle(kTRUE);
  hist->GetYaxis()->CenterTitle(kTRUE);
  hist->GetZaxis()->CenterTitle(kTRUE);



  hist->SetContour(31);
  hist->GetZaxis()->SetTitleOffset(1.1);
  
}
