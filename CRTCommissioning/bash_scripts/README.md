# Bash Scripts for CRT commissioning

- RunCRTModAnaAndPlots.sh should be sourced with run numbers as arguments. And will attempt to ssh onto the gateway and daq machine and run BERNCRTAna and eventAna for all of the run numbers supplied and then copy them back over to the gpvms, then run some simple analyzer modules/plotting scripts of pedestals and max ADC distributions and create a pdf plot dump. 
  - Files will automatically be sent to /sbnd/data/users/$USER/crtModTests, you can set a global variable CRTDIR with your perfered path
- RunCRTPlotsOnly.sh is run the same way, but needs the files to already have been copied over from the daq machine
   
