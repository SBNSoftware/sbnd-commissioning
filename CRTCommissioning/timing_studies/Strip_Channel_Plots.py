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

#READ IN ROOT FILE
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
]
for d in range(32):
	branches.append("adc" + str(d))

#DEFINE DICTIONARY THAT STORES DATA FROM BRANCHES
data = tree.arrays(branches, library="np")

#LOOP THROUGH DATA TO FILL ARRAY WITH UNIQUE FEB IDs
febs = []
for y in range(data['fEvent'].size):
	for z in range(data['mac5'][y].size):
		febs.append(data['mac5'][y][z])
feb_list = np.unique(febs)
feb_channel_hits = [] #STORES THE DATA OBJECTS
m = 0

#CREATE THE INDIVIDUAL ARRAYS OF FLAG 3 HITS PER 32 CHANNELS
for e in range(len(feb_list)):
	feb_channel_hits.append("feb" + str(feb_list[e]) + "_channel_hits")
	globals()[feb_channel_hits[m]] = [0] * 32
	m += 1

#FILL THE ARRAYS WITH THE SiPM CHANNEL ID HIT IN EACH EVENT FOR EACH FEB
for a in range(data['fEvent'].size):
	for b in range(data['flags'][a].size):
		if data['flags'][a][b] == 3:
			#FIND THE CHANNEL WITH THE HIGHEST ADC VALUE, DEFINING WHICH CHANNEL IS 'HIT'
			maxindex = 0
			for c in range(31):
				if (data['adc%d' %(c+1)][a][b] > data['adc%d' %(maxindex)][a][b]):
					maxindex = c+1
			globals()["feb%d_channel_hits" %(data['mac5'][a][b])][maxindex] += 1

#HELPFUL PRINTS :)
print("Events: " + str(data['fEvent'].size))
print("Parameters: " + str(feb_channel_hits))
for g in range(len(feb_list)):
	print(globals()["feb%d_channel_hits" %(feb_list[g])])

#DEFINE/CREATE PLOT PATH DIRECTORY
plotpath = 'plots/run' + str(runname)
if not os.path.exists(plotpath):
    os.makedirs(plotpath, 0o777)

#PLOT THE DISTRIBUTION OF CHANNEL HITS FOR EACH FEB
for f in range(len(feb_list)):
	fig = plt.figure()
	fig.suptitle('A-frame run ' + str(runname) + ' | FEB ' + str(feb_list[f]) + ' hits per channel | Flag = 3')
	#fig.suptitle('A-frame 2 | FEB ' + str(feb_list[f]) + ' hits per channel | Flag = 3')
	plt.bar(range(32), globals()["feb%d_channel_hits" %(feb_list[f])], align = 'edge', width = 1, facecolor = 'teal', edgecolor = 'black')
	plt.ylabel('Number of hits')
	plt.xlabel('Channel number')
	plt.xlim([0, 32])
	fig.savefig('plots/run' + str(runname) + '/FEB' + str(feb_list[f]) + '_channel_hits_' + str(runname) + '.jpeg', dpi=200)
	#fig.savefig('Aframe_2_Collective_Runs/aframe2runs_FEB' + str(feb_list[f]) + '_channel_hits.jpeg', dpi=200)
	plt.close()
	print("FEB " + str(feb_list[f]) + " channel hits done :)")
