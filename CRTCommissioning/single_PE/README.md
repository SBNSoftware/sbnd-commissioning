# Measurement of the Silicon PhotonMultiplier (SiPM) responses for Photon Electrons (PEs)

## Introduction
The general idea and the methodology of this analysis can be found in [docdb-31811](https://sbn-docdb.fnal.gov/cgi-bin/sso/ShowDocument?docid=31811). 

As a brief recap, we are utilising Silicon Photomultipliers (SiPMs) as our photon detectors within the CRT module. In our measurement approach, we use rediogenics to generate the so-called "finger" plots, where each peak corresponds to an increasing number of Photon Electrons (PEs). By fitting the ADC value of each PE peak against the number of peaks, we can extract the single PE response/effective gain for a specific bias setting. Through scanning different bias settings, we gain insights into how SiPMs respond to PEs as a function of bias, and the same logical process can be applied with different pre-amplifier settings.

This repository contains a folder named as `data_taken` where saving all files for the data taking and also another folder names as `data_analysis` where containing all files we are using to analyse the data. 

## Folder `data_taking`
This folder includes a subfolder that stores all files needed for data taking. The folder of `aframe_singlePE_voltage_scanning` should be set up under `/home/nfs/sbnd/DAQ_DevAreas/DAQ_23Sep2022CRTNoise/srcs/sbndaq/sbn-nd/DAQInterface/configs/standard`, the bash script `run_aframe_singlePE_voltage_scan.sh` should be put in 
`/home/nfs/sbnd/DAQ_DevAreas/DAQ_23Sep2022CRTNoise/srcs/sbndaq/sbn-nd/DAQInterface/`. The part `/DAQ_23Sep2022CRTNoise/srcs/sbndaq/sbn-nd/DAQInterface/` might be changing depending on which DAQ area you are using for the A-frame testing. 
### Configuration files
To understand the configuration fikes, let's take the file `aframe_singlePE_voltage_scanning/crt01_aframe_singlePE_140.fcl` as an example. This file contains the configuration settings for all the channels of each Front-End Board (FEB), as shown below:

```
FEBConfiguration_singlePEVoltage140.channel_configuration:
[
     [ 0, 0, 1, 140, 1, 52, 47, 0, 0, 1], # Channel 0
     ...
]
```
So the key configurations that we changed here are:
- the bias setting, which is the $4^{th}$ parameter with value of `140` 
- the pre amplifer, which the $5^{th}$ parameter with value of `52`

Side note: During the last A-frame testing for the CRT north wall, we only focused on performing bias setting scanning. This means that we collected data using different bias settings in order to comprehend how the SiPMs responded to varying bias settings. But of course, we can do a similar measurement with different pre amplifier settings, please talk to Michelle S. and Chris H. before moving forward. 

### Running fhicls
The set of fhicls names as `crt01_aframe_singlePE_*.fcl` in the floder `aframe_singlePE_voltage_scanning ` are running fhicls for the data taking. You need to make sure that they are up-to-date, as we will be changing FEBs all the time, and with different Aframes, the number of modules are also changing. 

Here is a list about which lines you need to double-check before starting the DAQ: 

- `daq.fragment_receiver.fragment_ids: [0xB799, 0xB7EE, 0xB79B, 0xB79C, 0xB79D, 0xB79E, 0xB79F, 0xB7B5, 0xB7B6]` these are hexadecimals for the FEB number. One useful [website](https://www.rapidtables.com/convert/number/decimal-to-hex.html) that I found for the conversion between decimal and hexadecimals. 
- `daq.fragment_receiver.ethernet_port: "enp23s0f0"` you will need to double-check with Michelle S./Erin Y. about whether this number for ethernet port is correct. 
- `daq.fragment_receiver.TurnOnHV : [ true, true, true, true, true, true, true, true, true ]` the number of `true` you put here should match with the number of FEBs you are taking data with. 
- `daq.fragment_receiver.PPS_offset_ns : [  0, 0, 0, 0, 0, 0, 0, 0, 0 ]` same as above. 
- you need to make sure all FEB you are taking data with have a line like `daq.fragment_receiver.FEBConfigurationMAC153`

All other copied fhicls named with `.copy` are just fhicls I saved in this folder are just a copy of fhicls that were using before the single PE data taking. 

### The bash script to take the data
The script `run_aframe_singlePE_voltage_scan.sh` are used to automatically take data for this measurement using the run-sleep-stop stragey. The bash script is also saving the run log information in the `runlog_bias_scan.log`. This bash script should be well-designed, but please double-check before running. 

## Folder `data_analysis`
This folder contains all the necessary files to execute the analysis box for the collected data from this measurement. The primary bash script, aframe_singlePE_analyse_go.sh, is the only file you need to run or focus on. However, if you encounter any unexpected or unusual results and suspect a potential issue with the code, I kindly request that you reach out to me before taking any further action. <Jiaoyang Li/李 娇瑒(jiaoyang.li@ed.ac.uk)>

### The bash script `aframe_singlePE_analyse_go.sh`
The thing you must change: 
- `Run_numbers=()` you need to provide the run number of the data you want to analyse. 

Things you might consider changeing:
- `TARGET_bias_setting`, which is used to calculate the effective gain if bias setting provided. The effective gain is used to converting the raw ADC value to the PEs. 
- `TARGET_eff_gain`, which is used to calculate the bias setting for the commissioning of each channel if the effctive gain is targeted. 
- `Voltages=()` if you decide to scan through different bias settings. 
- `n` which is the number of bias settings, the change should be considered if `Voltages` is changed. 
- `Data_dir` this is the directory where we store the data, doubld-check if this is true in your case. 


======================================================
**From this upon, I am going to use the dir `home/nfs/sbnd/Aframe/single_PE_search` as an example, I would suggest you to start a fresh new directory for your analysis.**
## How to take data 
- Launch into the sbnd daq machine. 
- Create a `tmux` session by (you can also use `screen` or any equvilant techiniques): 
    - `tmux new -s <name of your session>`
    - the reaon for this is due to if your gpvm session dies due to the poor internet connection, you data taking will also be stopped. 
- Source the bash script to set up the env to launch DAQ by: 
    - `source crt_Aframe_launchdaq.sh`
- Launch the DAQ:
    - `source run_aframe_singlePE_voltage_scan.sh`
- Detach from the `tmux` session by: `control b d`

## How to run the analyse box: 
- Launch into the sbnd daq machine. 
- Create a tmux session by:
    - `tmux new -s <name of your session>`
- Go to the folder `/home/nfs/sbnd/Aframe/single_PE_search`
    - Source the bash script to set up the env to launch DAQ by: `source launch_single_PE_search_aframe.sh`
        - (This setting up script is setup to a different source direction than the X-Y coincidence! For now)
- Modify the run numbers in `aframe_singlePE_analyse_go.sh`
Run the black box: `./aframe_singlePE_analyse_go.sh`
- Detach from the tmux session by: `control b d` 

## How to dump all plots in a PDF: 
Huge thanks to Amy F. for this script! 
- Launch into the sbnd daq machine. 
- From a DAQ machine : 
    - `cd /home/nfs/sbnd/Aframe/single_PE_search/tex/`
- Edit `RunsAndFEBIDs.tex` to use your FEB id numbers, run numbers, and starting run number
- Run `pdflatex SinglePEPlots.tex`
    - You sometimes have to run pdflatex twice for the table of contents to update
- Open your pdf or copy it to your local machine. Location:`/home/nfs/sbnd/Aframe/single_PE_search/tex/SinglePEPlots.tex`
