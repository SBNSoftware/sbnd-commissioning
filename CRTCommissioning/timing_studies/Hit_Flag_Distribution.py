#!/usr/bin/env python
# coding: utf-8
# Author: Harry Scott; hjscott1@sheffield.ac.uk

#ROOT IMPORTS
import uproot
from ROOT import *
from array import array

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

#PATH TO ROOT FILE
fname = "runs/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/xycoinc/run" + str(runname) + ".root"
#fname = "aframe2runs.root"
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
feb_list.sort()
febs.clear()

#CREATE DATA PARAMETERS
flag_list = [] #LIST OF UNIQUE FLAGS
feb_hits = [] #ARRAY OF PARAMETERS FOR EACH FEB AND FLAG
totalhits = 0 #INT OF TOTAL NUMBER OF HITS IN RUN
hits_per_event = [0] * (max(data['fEvent']) + 1) #ARRAY CONTAINING NUMBER OF HITS PER EVENT FRAGMENT
m = 0
l = 0

#CREATE THE HIT ARRAYS BASED ON THE FLAG AND FEB NUMBERS
for i in range(data['fEvent'].size):
	for j in range(data['flags'][i].size):
		if data['flags'][i][j] not in flag_list:
			flag_list.append(data['flags'][i][j])
			for k in range(len(feb_list)):
				feb_hits.append("feb" + str(feb_list[k]) + "_" + str(flag_list[l]) + "_hits")
				globals()[feb_hits[m]] = 0
				m += 1
			l += 1
flag_list.sort()

#FILL THE ARRAYS WITH NUMBER OF HITS BASED ON FLAG AND FEB NUMBER
for a in range(data['fEvent'].size):
	hits_per_event[data['fEvent'][a]] = data['flags'][a].size
	for b in range(data['flags'][a].size):
		totalhits +=1
		for c in range(len(feb_list)):
			for d in range(len(flag_list)):
				if data['mac5'][a][b]==feb_list[c] and data['flags'][a][b]==flag_list[d]:
					globals()["feb%d_%d_hits" %(data['mac5'][a][b], data['flags'][a][b])] += 1
    
print("Events: " + str(data['fEvent'].size))
print("Hits: " + str(totalhits))
print("FEBs: " + str(feb_list))
print("Flags: " + str(flag_list))

#CREATE THE PATH DIRECTORY WHERE PLOTS ARE SAVED
plotpath = 'plots/run' + str(runname)
if not os.path.exists(plotpath):
    os.makedirs(plotpath, 0o777)

#PLOT THE DISTRIBUTION OF HITS PER BOARD FOR EACH FLAG (ONE PLOT PER FLAG)
feb_names = []
fig1 = plt.figure()
for f in range(len(flag_list)):	
	hit_objects = []
	feb_names = []
	fig1.suptitle('A-frame run ' + str(runname) + ' | FEB hit distribution | Flag = ' + str(flag_list[f]))
	#fig1.suptitle('A-frame 2 | FEB hit distribution | Flag = '  + str(flag_list[f]))
	for e in range(len(feb_list)):
		hit_objects.append(globals()["feb%d_%d_hits" %(feb_list[e], flag_list[f])])
		feb_names.append(str(feb_list[e]))
	plt.xlabel('FEB number')
	plt.ylabel('Number of hits')
	plt.bar(feb_names, hit_objects, align='center', facecolor='mediumblue', edgecolor='black')
	plt.savefig('plots/run' + str(runname) + '/hit_distribution_flag' + str(flag_list[f]) + '_' + str(runname) + '.jpeg', dpi=200)
	#plt.savefig('aframe2runs_hit_distribution_flag' + str(flag_list[f]) + '.jpeg', dpi=200)
	plt.clf()
	print("Flag " + str(flag_list[f]) + " Hit Distribution Done :)")

#PLOT THE NUMBER OF TOTAL HITS FOR EACH FLAG
flag_hits = [0] * (len(flag_list))
flag_names = []
for b in range(len(flag_list)):
    flag_names.append(str(flag_list[b]))
for w in range(len(flag_list)):    
    for f in range(len(feb_list)):
        flag_hits[w] += globals()["feb%d_%d_hits" %(feb_list[f], flag_list[w])]
    print("Flag " + str(flag_list[w]) + ": " + str(flag_hits[w]))
fig2 = plt.figure(2)
fig2.suptitle('A-frame run ' + str(runname) + ' | Flag distribution')
#fig2.suptitle('A-frame 2 | Flag distribution')
plt.xlabel('Flag Number')
plt.ylabel('Number of hits')
plt.bar(flag_names, flag_hits, align='center', facecolor='deepskyblue', edgecolor='black')
fig2.savefig('plots/run' + str(runname) + '/flag_distribution_' + str(runname) + '.jpeg', dpi=200)
#fig2.savefig('aframe2runs_flag_distribution.jpeg', dpi=200)
print("Flag Distribution Done :)")

#PLOT THE NUMBER OF HITS PER EVENT FRAGMENT
fig3 = plt.figure(3)
fig3.suptitle('A-frame run ' + str(runname) + ' | Hits per event')
plt.xlabel('Event number')
plt.ylabel('Number of hits')
plt.plot(hits_per_event, color='midnightblue')
plt.xlim([0, len(hits_per_event)])
plt.ylim(bottom = 1)
fig3.savefig('plots/run' + str(runname) + '/hits_per_event_' + str(runname) + '.jpeg', dpi=200)
print("Hits Per Event Done :)")

#NUMBER OF HITS PER EVENT ON LOGARITHMIC SCALE
#fig4 = plt.figure(4)
#fig4.suptitle('A-frame run ' + str(runname) + ' | Hits per event (log scale)')
#plt.xlabel('Event number')
#plt.ylabel('Number of hits')
#plt.plot(hits_per_event, color='midnightblue')
#plt.xlim([0, len(hits_per_event)])
#plt.yscale("log")
#plt.ylim(bottom = 1)
#fig4.savefig('plots/run' + str(runname) + '/hits_per_event_log_' + str(runname) + '.jpeg', dpi=200)
#print("Hits Per Event Log Scale Done :)")
