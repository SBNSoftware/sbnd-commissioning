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

#READ IN ROOT FILE
fname = "runs/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/run" + str(runname) + ".root"
#fname = "/sbnd/data/users/afilkins/crtModTests/xycoinc/run" + str(runname) + ".root"
#fname = "aframe2runs.root"
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
	"lostcpu",
	"lostfpga",
	"lost_hits",
]

#DEFINE DICTIONARY THAT STORES DATA FROM BRANCHES
data = tree.arrays(branches, library="np")

#CREATE THE ARRAYS OF FEBS SELECTING CERTAIN FLAGS
time = []
timerange = 0
lostcpu = 0
lostfpga = 0
losthits = 0
totalhits = 0
febs = []
flags = []
flag_hits = [0, 0, 0]
for y in range(data['fEvent'].size):
	for z in range(data['mac5'][y].size):
		time.append(data['timestamp'][y][z]) #UNIX TIMESTAMP
		febs.append(data['mac5'][y][z]) #FEB ID
		flags.append(data['flags'][y][z]) #FLAG NUMBER
		lostcpu += data['lostcpu'][y][z] #WHETHER AN EVENT IS LOST BEFORE THE CPU STAGE
		lostfpga += data['lostfpga'][y][z] #WHETHER AN EVENT IS LOST BEFORE THE FPGA STAGE
		losthits += data['lost_hits'][y][z] #ESTIMATION OF LOST TOTAL HITS CORRECTING FOR DEADTIME
		totalhits +=1 #TOTAL NUMBER OF HITS
		
		if data['flags'][y][z] == 3:
			flag_hits[0] += 1 #TOTAL NUMBER OF FLAG 3 HITS
		elif data['flags'][y][z] == 7:
			flag_hits[1] += 1 #TOTAL NUMBER OF FLAG 7 HITS
		elif data['flags'][y][z] == 11:
			flag_hits[2] += 1 #TOTAL NUMBER OF FLAG 11 HITS

timerange = max(time) - min(time) #TOTAL TIME TAKEN OF RUN IN ns
flag_list = np.unique(flags) #LIST OF UNIQUE FLAG NUMBERS
feb_list = np.unique(febs) #LIST OF UNIQUE FEB IDs
flag_list.sort()
feb_list.sort()

#PRINT THEM ALL
print("Events: " + str(data['fEvent'].size))
print("Hits: " + str(totalhits))
print("FEBs: " + str(feb_list))
print("Flags: " + str(flag_list))
print("Flag 3 hits: " + str(flag_hits[0]))
print("Flag 7 hits: " + str(flag_hits[1]))
flag7density = (flag_hits[1]*1000000000)/(T0_reset_rate*len(feb_list)*timerange) #FRACTION OF SUCCESSFUL T0 RESET EVENTS WITH RESPECT TO THE TOTAL NUMBER OF 1s INCREMENTS OF THE RUN
print("Flag 7 Density: " + str(flag7density))
print("Flag 11 hits: " + str(flag_hits[2]))
flag11density = (flag_hits[2]*1000000000)/(T1_reset_rate*len(feb_list)*timerange) #FRACTION OF SUCCESSFUL T1 RESET EVENTS WITH RESPECT TO THE TOTAL NUMBER OF T1 RESET INCREMENTS OF THE RUN
print("Flag 11 Density: " + str(flag11density))
hours = timerange/(1e9*3600) #TOTAL RUNTIME IN HOURS
minutes = 60*(hours - int(hours)) #EXCESS MINUTES AFTER ROUNDED-DOWN HOURS
seconds = 60*(minutes - int(minutes)) #EXCESS SECONDS AFTER ROUNDED-DOWN MINUTES
print("Runtime: " + str(int(hours)) + "h " + str(int(minutes)) + "m " + str(int(seconds)) + "s")
print("lostcpu: " + str(lostcpu))
print("lostfpga: " + str(lostfpga))
print("lost_hits: " + str(losthits))

flag_list_str = ''.join(str(a) + "," for a in flag_list) #CONVERTS THE LIST OF FLAG NUMBERS INTO A STRING WITH COMMAS IN BETWEEN (FORMATTING FOR LATEX FILE)
flag_list_str = flag_list_str[:-1] #REMOVES THE FINAL COMMA BECAUSE I'M BAD AT CODING
feb_list_str = ''.join(str(b) + "," for b in feb_list) #SAME BUT FOR FEB NUMBERS
feb_list_str = feb_list_str[:-1] #SAME SAME

#WRITES THE LIST OF FLAGS, FEBS AND RUN NUMBERS IN THE NewRunsAndFEBIDs.tex FILE TO BE READ INTO CRTPlotDump.tex SO PARAMETERS ARE AUTOMATICALLY UPDATED WITH EACH RUN
RunParameters = {'flaglisthere':flag_list_str, 'feblisthere':feb_list_str, 'runnumberhere':runname}
with open('latex/RunsAndFEBIDs.tex', 'r') as myfile:
	text = myfile.read()
	for key, value in RunParameters.items():
		text = text.replace(key, str(value))
	with open('latex/NewRunsAndFEBIDs.tex', 'w') as output:
		output.write(text)

myfile.close()
output.close()
