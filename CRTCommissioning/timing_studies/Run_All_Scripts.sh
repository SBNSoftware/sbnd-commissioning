#RUNS EACH PYTHON SCRIPTS SEQUENTIALLY (SEE readme.txt FOR DETAILS)
python3 Hit_Flag_Distribution.py $1
python3 Timestamp_Hists.py $1
python3 FEB_Clock_Drift.py $1
python3 Strip_Channel_Plots.py $1
python3 Details.py $1

#RUNS LATEX PLOT DUMP COMPILING WITH PDFLATEX
pdflatex --jobname=TimingPlotDump$1 latex/TimingPlotDump.tex
rm *.aux *.log *.nav *.out *.snm *.toc #REMOVES RANDOM LATEX OUTPUT FULL (FEEL FREE TO EDIT IF YOU WANT TO KEEP THEM... FOR SOME REASON)
