#!/usr/bin/env python
# coding: utf-8
# Author: Harry Scott; hjscott1@sheffield.ac.uk

#ROOT IMPORTS
import uproot
from ROOT import *

#PYTHON IMPORTS
import sys
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import numpy as np
import math
import datetime as dt
import os

from importlib import reload

#RUN NUMBER ENTERED AS FIRST ARGUMENT AFTER SCRIPT
runname = sys.argv[1]
T0_reset_rate = 1.0 #Hz
T1_reset_rate = 4.727865 #Hz

#PATH TO ROOT FILE
fname = "runs/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/xycoinc/run" + str(runname) + ".root"
#fname = "Aframe_2_Collective_Runs/aframe2runs.root"
folder = "dumpall"
tname = "events"

#CREATE ROOT TREE USING UPROOT
tree = uproot.open(fname)[folder][tname]

#DEFINE BRANCHES AND CREATE DATA TREE
branches = [
    "fEvent",
    "flags",
    "mac5",
    "ts0",
    "ts1",
    "timestamp",
]

#DEFINE DICTIONARY THAT STORES DATA FROM BRANCHES
data = tree.arrays(branches, library="np")

#LOOP THROUGH DATA TO FILL ARRAY WITH UNIQUE FEB IDs AND FIND TIMESTAMP VALUES
time = []
timerange = 0
febs = []
for y in range(data['fEvent'].size):
    for z in range(data['mac5'][y].size):
        time.append(data['timestamp'][y][z])
        febs.append(data['mac5'][y][z])
timerange = max(time) - min(time) #TIME OF RUN IN ns
feb_list = np.unique(febs) #LIST OF UNIQUE FEB IDs

#CREATE DATA PARAMETERS
flag_list = [7, 11] #SELECT ONLY FLAG 7 AND 11 HITS
flag_hits = [0, 0] #STORES THE TOTAL NUMBER OF HITS FOR EACH FLAG
param_list = [] #STORES THE DATA OBJECTS
m = 0

#CREATE THE INDIVIDUAL ARRAYS OF FLAG 7 TS0 AND FLAG 11 TS1 TIMESTAMP ARRAYS
for k in range(len(feb_list)):
    param_list.append("feb" + str(feb_list[k]) + "_7_TS0")
    globals()[param_list[m]] = []
    m += 1
    param_list.append("feb" + str(feb_list[k]) + "_11_TS1")
    globals()[param_list[m]] = []
    m += 1

#CREATE THE PARAMETERS USED TO FIND THE NUMBER OF MISSED FLAG 7 AND 11 HITS
meanF7TS1 = []
meanF11TS0 = []
meanF7TS1array = []
meanF11TS0array = []
currentFEBs = []
numFebsWithinRangeFlag7 = []
numFebsWithinRangeFlag11 = []
deviation = 20 #ns

#FILL THE ARRAYS WITH DATA
for i in range(data['fEvent'].size):
    flag7index = []
    flag11index = []
    for j in range(data['flags'][i].size):
        if data['flags'][i][j]==7:
            flag_hits[0] += 1 #APPEND TOTAL NUMBER OF FLAG 7 HITS
            globals()["feb%d_%d_%s" %(data['mac5'][i][j], data['flags'][i][j], "TS0")].append(data['ts0'][i][j]) #APPENDS SPECIFIC FEB'S FLAG 7 TS0 HIT
            meanF7TS1.append(data['ts1'][i][j])
            flag7index.append(j)
        elif data['flags'][i][j]==11:
            flag_hits[1] += 1
            globals()["feb%d_%d_%s" %(data['mac5'][i][j], data['flags'][i][j], "TS1")].append(data['ts1'][i][j])
            meanF11TS0.append(data['ts0'][i][j])
            flag11index.append(j)
	#CALCULATES THE MEAN TS1 VALUE FOR FLAG 7 HITS AND FINDS THE NUMBER OF FEB'S TS1 TIMESTAMP DEVIATING BY 20ns FROM THE MEAN
    if len(meanF7TS1) > 0:
        meanF7TS1value = sum(meanF7TS1)/len(meanF7TS1)
        meanF7TS1array.append(meanF7TS1value)
        numFebsWithinRangeFlag7.append(0)
        for s in range(len(flag7index)):
            if ((data['ts1'][i][flag7index[s]] - meanF7TS1value) < deviation):
                numFebsWithinRangeFlag7[len(meanF7TS1array)-1] += 1
        meanF7TS1 = []

	#CALCULATES THE MEAN TS0 VALUE FOR FLAG 11 HITS AND FINDS THE NUMBER OF FEB'S TS0 TIMESTAMP DEVIATING BY 20ns FROM THE MEAN (i know this looks gross, just email me at hjscott1@sheffield.ac.uk if you want a breakdown)
    if len(meanF11TS0) > 0:
        meanF11TS0.sort()
        currentF11TS0mean = []
        currentF11Batch = 0
        for u in range(len(meanF11TS0)):
            if (((meanF11TS0[currentF11Batch] + 1e9/(100*T1_reset_rate)) < meanF11TS0[u]) or (u == len(meanF11TS0) - 1)) and (len(currentF11TS0mean) > 0):
                if (u == len(meanF11TS0) - 1) and meanF11TS0[u] < (meanF11TS0[currentF11Batch] + 1e9/(100*T1_reset_rate)):
                    currentF11TS0mean.append(meanF11TS0[u])
                currentF11Batch = u
                meanF11TS0value = sum(currentF11TS0mean)/len(currentF11TS0mean)
                meanF11TS0array.append(meanF11TS0value)
                numFebsWithinRangeFlag11.append(0)
                for t in range(len(flag11index)):
                    if (abs(data['ts0'][i][flag11index[t]] - meanF11TS0value) < deviation):
                        numFebsWithinRangeFlag11[-1] += 1
                currentF11TS0mean = []
                currentF11TS0mean.append(meanF11TS0[u])
            else:
                currentF11TS0mean.append(meanF11TS0[u])
        meanF11TS0 = []

#SUBTRACT THE T0/T1 RESET WINDOW FROM THE ARRAYS SO THEY ARE CENTRED AROUND 0
flag7_TS0_range = []
flag11_TS1_range = []
for p in range(len(feb_list)):
    globals()["feb%d_7_TS0" %(feb_list[p])] = [x - int(1e9/T0_reset_rate) for x in globals()["feb%d_7_TS0" %(feb_list[p])]]
    globals()["feb%d_11_TS1" %(feb_list[p])] = [y - int(1e9/T1_reset_rate) for y in globals()["feb%d_11_TS1" %(feb_list[p])]]    

#HELPFUL PRINTS :)
print(feb_list)
print(param_list)
print("Flag 7: " + str(flag_hits[0]))
flag7density = (flag_hits[0]*1000000000)/(T0_reset_rate*len(feb_list)*timerange)
print("Density: " + str(flag7density))
print("Flag 11: " + str(flag_hits[1]))
flag11density = (flag_hits[1]*1000000000)/(T1_reset_rate*len(feb_list)*timerange)
print("Density: " + str(flag11density))

#DEFINE/CREATE PLOT PATH DIRECTORY
plotpath = 'plots/run' + str(runname)
if not os.path.exists(plotpath):
    os.makedirs(plotpath, 0o777)

#PLOT THE NUMBER OF FEBS SUCCESSFULLY OUTPUTTING A FLAG 7 EVENT IN THE T0 RESET WINDOW
fig = plt.figure(0)
fig.suptitle('A-frame run ' + str(runname) + ' | FEB occupancy | Flag = 7')
#fig.suptitle('A-frame 2 | FEB occupancy | Flag = 7')
febOccupancyF7 = plt.hist(numFebsWithinRangeFlag7, bins = range(1, len(feb_list)+2), facecolor='cadetblue', edgecolor='black')
plt.xlabel('Number of FEBs within ' + str(deviation) + ' ns of average TS1')
plt.ylabel('Frequency')
#fig.savefig('aframe2runs_FEB_Occupancy_flag7.jpeg', dpi=200)
fig.savefig('plots/run' + str(runname) + '/FEB_Occupancy_flag7_TS1_' + str(runname) + '.jpeg', dpi=200)
print("Flag 7 Occupancy Done :)")

#PLOT THE NUMBER OF FEBS SUCCESSFULLY OUTPUTTING A FLAG 11 EVENT IN THE T1 RESET WINDOW
fig1 = plt.figure(1)
fig1.suptitle('A-frame run ' + str(runname) + ' | FEB occupancy | Flag = 11')
#fig1.suptitle('A-frame 2 | FEB occupancy | Flag = 11')
febOccupancyF11 = plt.hist(numFebsWithinRangeFlag11, bins = range(1, len(feb_list)+2), facecolor='cadetblue', edgecolor='black')
plt.xlabel('Number of FEBs within ' + str(deviation) + ' ns of average TS0')
plt.ylabel('Frequency')
#fig1.savefig('aframe2runs_FEB_Occupancy_flag11.jpeg', dpi=200)
fig1.savefig('plots/run' + str(runname) + '/FEB_Occupancy_flag11_TS0_' + str(runname) + '.jpeg', dpi=200)

print("Flag 11 Occupancy Done :)")

#PLOT THE FLAG 7 TS0 AND FLAG 11 TS1 HISTOGRAMS AFTER SUBTRACTING THE RESET WINDOWS
binwidth = 1
for a in range(len(feb_list)):
	#CALCULATE MEAN AND STANDARD DEVIATION FOR FLAG 7 T0 RESET TIMESTAMP HISTOGRAMS:
	print("FEB " + str(feb_list[a]))
	f7entries = len(globals()["feb%d_7_TS0" %(feb_list[a])])
	flag7densitycurrent = (f7entries*1000000000)/(T0_reset_rate*timerange) #DENSITY OF SUCCESSFUL FLAG 7 EVENTS (# OF FLAG 7s / INCREMENTS OF 1s ACROSS RUN)
	if (f7entries > 0):
		mean = np.average(globals()["feb%d_7_TS0" %(feb_list[a])]) #MEAN FLAG 7 TS0 VALUE
		sigma = np.std(globals()["feb%d_7_TS0" %(feb_list[a])]) #STANDARD DEVIATION OF DISTRIBUTION
		
		#HELPFUL PRINTS :)
		print("Flag 7 hits: " + str(f7entries))
		print("Flag 7 hit density: " + str(flag7densitycurrent))
		print("TS0 Mean: " + str(mean))
		print("TS0 s.d: " + str(sigma))
		
		#CREATE LEGEND FOR HISTOGRAM
		textstr = '\n'.join((
			'Entries = %d' %(f7entries), 
			'Mean = %f' %(mean),
			'Std = %f' %(sigma)))

		#DEFINE NUMBER OF BINS AS 1ns WIDTH
		if (max(globals()["feb%d_7_TS0" %(feb_list[a])]) - min(globals()["feb%d_7_TS0" %(feb_list[a])]) < 50):
			n7bins = range(min(globals()["feb%d_7_TS0" %(feb_list[a])]), max(globals()["feb%d_7_TS0" %(feb_list[a])]) + binwidth, binwidth)
		else:
			n7bins = 50
		
		#PLOTTING FLAG 7 TS0 VALUES IN A HISTOGRAM
		fig2 = plt.figure(2)
		fig2.suptitle('Run ' + str(runname) + ' FEB ' + str(feb_list[a]) + ' | TS0 - ' + str(int(1e9/T0_reset_rate)) + ' ns histogram | Flag = 7')
		#fig2.suptitle('A-frame 2 FEB ' + str(feb_list[a]) + ' | TS0 - ' + str(int(1e9/T0_reset_rate)) + ' ns histogram | Flag = 7')
		flag7ts0hist = plt.hist(globals()["feb%d_7_TS0" %(feb_list[a])], bins = n7bins, facecolor='b', edgecolor='black')
		props = dict(boxstyle='square', facecolor='white', alpha=0.5)
		fig2.text(0.67, 0.86, textstr, fontsize=10, verticalalignment='top', bbox=props)
		plt.xlabel("TS0 - " + str(int(1e9/T0_reset_rate)) + " ns")
		plt.ylabel("Frequency")
		fig2.savefig('plots/run' + str(runname) + '/FEB' + str(feb_list[a]) + '_flag7_TS0_' + str(runname) + '.jpeg', dpi=200)
		#fig2.savefig('aframe2runs_FEB' + str(feb_list[a]) + '_flag7_TS0.jpeg', dpi = 200)
		plt.clf()
		print("FEB " + str(feb_list[a]) + " Flag 7 Histogram Done :)")
	else:
		print("FEB " + str(feb_list[a]) + " flag 7 is empty :(")

	#CALCULATE MEAN AND STANDARD DEVIATION FOR FLAG 11 T1 RESET TIMESTAMP HISTOGRAM:
	f11entries = len(globals()["feb%d_11_TS1" %(feb_list[a])])
	flag11densitycurrent = (f11entries*1000000000)/(T1_reset_rate*timerange) #DENSITY OF SUCCESSFUL FLAG 7 EVENTS (# OF FLAG 11s / TOTAL INCREMENTS OF T1 RESET WINDOW ACROSS RUN)
	if (f11entries > 0):
		mean2 = np.average(globals()["feb%d_11_TS1" %(feb_list[a])]) #MEAN FLAG 11 TS1 VALUE
		sigma2 = np.std(globals()["feb%d_11_TS1" %(feb_list[a])]) #STANDARD DEVIATION OF DISTRIBUTION

		#HELPFUL PRINTS :)
		print("Flag 11 hits: " + str(f11entries))
		print("Flag 11 hit density: " + str(flag11densitycurrent))
		print("TS1 Mean: " + str(mean2))
		print("TS1 s.d: " + str(sigma2))
		
		#CREATE LEGEND FOR HISTOGRAM
		textstr = '\n'.join((
			'Entries = %d' %(f11entries), 
			'Mean = %f' %(mean2),
			'Std = %f' %(sigma2)))
		
		#DEFINE NUMBER OF BINS AS 1ns WIDTH
		if (max(globals()["feb%d_11_TS1" %(feb_list[a])]) - min(globals()["feb%d_11_TS1" %(feb_list[a])]) < 50):
			n11bins = range(min(globals()["feb%d_11_TS1" %(feb_list[a])]), max(globals()["feb%d_11_TS1" %(feb_list[a])]) + binwidth, binwidth)
		else:
			n11bins = 50

		#PLOTTING FLAG 11 TS1 VALUES IN A HISTOGRAM
		fig3 = plt.figure(3)
		#fig3.suptitle('A-frame 2 FEB ' + str(feb_list[a]) + ' | TS1 - ' + str(int(1e9/T1_reset_rate)) + ' ns histogram | Flag = 11')
		fig3.suptitle('Run ' + str(runname) + ' FEB ' + str(feb_list[a]) + ' | TS1 - ' + str(int(1e9/T1_reset_rate)) + ' ns histogram | Flag = 11')
		flag11ts1hist = plt.hist(globals()["feb%d_11_TS1" %(feb_list[a])], bins = n11bins, facecolor='b', edgecolor='black')
		props = dict(boxstyle='square', facecolor='white', alpha=0.5)
		fig3.text(0.67, 0.86, textstr, fontsize=10, verticalalignment='top', bbox=props)
		plt.xlabel("TS1 - " + str(int(1e9/T1_reset_rate)) + " ns")
		plt.ylabel("Frequency")

		fig3.savefig('plots/run' + str(runname) + '/FEB' + str(feb_list[a]) + '_flag11_TS1_' + str(runname) + '.jpeg', dpi=200)
		#fig3.savefig('aframe2runs_FEB' + str(feb_list[a]) + '_flag11_TS1.jpeg', dpi = 200)
		plt.clf()

		print("FEB " + str(feb_list[a]) + " Flag 11 Histogram Done :)")
	else:
		print("FEB " + str(feb_list[a]) + " flag 11 is empty :(")
