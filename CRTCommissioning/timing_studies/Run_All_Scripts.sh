python3 Hit_Flag_Distribution.py $1
python3 Timestamp_Hists.py $1
python3 FEB_Clock_Drift.py $1
python3 Strip_Channel_Plots.py $1
python3 Details.py $1
pdflatex --jobname=CRTPlotDump$1 latex/CRTPlotDump.tex
