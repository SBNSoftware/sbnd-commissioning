Before running any scripts:
1. Source the usual larsoft setup scripts (https://cdcvs.fnal.gov/redmine/projects/larsoft/wiki/Quick-start_guide_to_using_and_developing_LArSoft_code)
1. Set up a python virtual environment in your working directory (https://docs.python.org/3/library/venv.html)
2. Create a directory of artroot files (raw CRT data put through an analysis fcl) called "runs" that the script calls from to read in the run files, then fill it with run root files to analyse.

----------------------------------------------------------------------------------------------------

How to run .py scripts:
python3 <script_name>.py <run number>
e.g. to run Hit_Flag_Distribution.py for runs/run5678.root:
python3 Hit_Flag_Distribution.py 5678

How to run Bash script (with no arguments):
source <script_name>.sh

How to run latex scripts:
pdflatex <script_name>.tex

----------------------------------------------------------------------------------------------------

Script breakdown:
Run_All_Scripts.sh - Bash script running each python script in the order outlined below. It has 1 argument which is the number of the run you are compiling.
To run a specific run, type the command 'source Run_All_Scripts.sh <run number>'

Hit_Flag_Distribution.py - Plots the number of hits by flag number for each FEB, the distribution of total hits per flag number and the number of hits per event fragment.

Timestamp_Hists.py - Plots the total TS0 and TS1 timestamps for flag 3 events for each FEB and the average TS0 and TS1 timestamps across all FEBs 

FEB_Clock_Drift.py - Plots flag 7 TS0 and Flag 11 TS1 timestamps as histograms for each FEB, subtracting the resective T0/T1 reset windows and plots the number of flag 7/11 hits missed by each FEB.

Strip_Channel_Plots.py - Plots the channel breakdown of flag 3 hits for each FEB, i.e. which of the 32 FEB channels had the highest adc value for each flag 3 hit.

Details.py - Prints the important details of the run such as total number of events/hits, number of events/hits by flag breakdown, number of FEBs, runtime, lostcpu/lostfpga/losthits and density of flag 7/11 hits successful. Also adds FEB IDs, flag numbers and the run number to the Latex file.

latex - Directory containing latex script TimingPlotDump.tex that take all plots produced by aforementioned scripts and dumps them into a latex presentation named TimingPlotDump<run_number>.pdf.

----------------------------------------------------------------------------------------------------

If anything doesn't make sense with these scripts then email me at hjscott1@sheffield.ac.uk and I will do my best to make sense of this absolute mess of python nonsense :)
