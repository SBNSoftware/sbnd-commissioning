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
import os

#RUN NUMBER ENTERED AS FIRST ARGUMENT AFTER SCRIPT
runname = sys.argv[1]

#PATH TO ROOT FILE
fname = "runs/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/xycoinc/run" + str(runname) + ".root"
#fname = "Aframe_1_Collective_Runs/aframe1runs.root"
folder = "dumpall"
tname = "events"

#CREATE ROOT TREE USING UPROOT
tree = uproot.open(fname)[folder][tname]

#DEFINE BRANCHES OF ROOT TREE TO READ
branches = [
    "fEvent",
    "flags",
    "mac5",
    "ts0",
    "ts1",
]

#DEFINE DICTIONARY THAT STORES DATA FROM BRACHES
data = tree.arrays(branches, library="np")

#LOOP THROUGH DATA TO FILL ARRAY WITH FEB LIST
febs = []
for y in range(data['fEvent'].size):
    for z in range(data['mac5'][y].size):
        febs.append(data['mac5'][y][z])
feb_list = np.unique(febs) #LIST OF UNIQUE FEB IDs
febs.clear()

#CREATE DATA PARAMETERS
param_list = []
timestamp = ['ts0', 'ts1']
m = 0

#CREATE FLAG 3 HIT ARRAYS BASED ON TIMESTAMP AND FEB NUMBER (AND AVERAGE TIMESTAMP ARRAYS)
for n in range(len(timestamp)):
	for k in range(len(feb_list)):
		param_list.append("feb" + str(feb_list[k]) + "_3_" + str(timestamp[n]))
		globals()[param_list[m]] = []
		m += 1
	param_list.append("average" + str(timestamp[n]))
	globals()[param_list[m]] = []
	m += 1
print(param_list)

#FILL THE ARRAYS WITH FLAG 3 TS0 AND TS1 TIMESTAMP VALUES
for i in range(data['fEvent'].size):
	for j in range(data['flags'][i].size):
		if data['flags'][i][j]==3:
			for r in range(len(timestamp)):
				globals()["feb%d_3_%s" %(data['mac5'][i][j], timestamp[r])].append(data[str(timestamp[r])][i][j])
				globals()["average%s" %(timestamp[r])].append(data[str(timestamp[r])][i][j])

bigweights = [1/len(feb_list)] * len(averagets0) #WEIGHT THE AVERAGE HISTOGRAMS

#PLOT HISTOGRAMS OF FLAG 3 HITS FOR TS0 AND TS1 TIMESTAMPS FOR EACH FEB (ONE PLOT PER FEB PER TIMESTAMP)
for c in range(len(timestamp)):
	for a in range(len(feb_list)):
		fig = plt.figure()
		fig.suptitle('A-frame run ' + str(runname) + ' | FEB ' + str(feb_list[a]) + ' ' + str(timestamp[c]) + ' timestamp (ns) histogram | Flag = 3')
	#	fig.suptitle('A-frame 1 | FEB ' + str(feb_list[a]) + ' ' + str(timestamp[c]) + ' timestamp (ns) histogram (adc cut) | Flag = 3')
		plt.hist(globals()["feb%d_3_%s" %(feb_list[a], timestamp[c])], bins = 50, facecolor='dodgerblue', edgecolor='black')
		plt.ylabel('Frequency')
		plt.xlabel(str(timestamp[c]) + ' timestamp (ns)')
		if len(globals()["feb%d_3_%s" %(feb_list[a], timestamp[c])]) > 0:
			plt.xlim(xmin= min(globals()["feb%d_3_%s" %(feb_list[a], timestamp[c])]), xmax = max(globals()["feb%d_3_%s" %(feb_list[a], timestamp[c])]))
		plotpath = 'plots/run' + str(runname)
		if not os.path.exists(plotpath):
			os.makedirs(plotpath, 0o777)
		fig.savefig('plots/run' + str(runname) + '/FEB' + str(feb_list[a]) + '_flag3_' + str(timestamp[c]) + '_' + str(runname) + '.jpeg', dpi=200)
		#fig.savefig('Aframe_1_Collective_Runs/aframe1runs_FEB' + str(feb_list[a]) + '_flag3_' + str(timestamp[c]) + 'cut.jpeg', dpi=200)
		plt.close()
		print("FEB " + str(feb_list[a]) + " Flag 3 " + str(timestamp[c]) + " Histogram Done :)")
	
	#PLOT THE AVERAGE TS0 AND TS1 TIMESTAMP VALUES FOR FLAG 3 EVENTS
	fig2 = plt.figure()
	fig2.suptitle('A-frame run ' + str(runname) + ' | Average ' + str(timestamp[c]) + ' timestamp (ns) histogram | Flag = 3')
	#fig2.suptitle('A-frame 1 | Average ' + str(timestamp[c]) + ' timestamp (ns) histogram (cut adc peak < 300) | Flag = 3')
	plt.hist(globals()["average%s" %(timestamp[c])], bins = 50, weights = bigweights, facecolor='cornflowerblue', edgecolor='black')
	plt.ylabel('Average Frequency')
	plt.xlabel(str(timestamp[c]) + ' timestamp (ns)')
	plt.xlim(xmin = min(globals()["average%s" %(timestamp[c])]), xmax = max(globals()["average%s" %(timestamp[c])]))
	fig2.savefig('plots/run' + str(runname) + '/average_' + str(timestamp[c]) + '_flag3_' + str(runname) + '.jpeg', dpi = 200)
	#fig2.savefig('Aframe_1_Collective_Runs/aframe1runs_average_flag3_ts0_cut.jpeg', dpi=200)
	plt.close()
	print("Average " + str(timestamp[c]) + " Timestamp Histogram Done :)")
