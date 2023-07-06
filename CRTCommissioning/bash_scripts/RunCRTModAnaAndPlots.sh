
#=========================================================================
#Make plots for a single set of FEBs, if you have multple sets of FEBs run multiple times.
#source RunCRTModPlots.sh run1 run2 run3 run4 "run5 run6" will make individual plots and pedestal measurements of runs 1-4, and a single set of histograms for runs 5 and 6 combined. All 5 of these pedestals will be included in the summary plots.  

#PDF will open when finished running, or can be found in /sbnd/data/users/afilkins/crtModTests/
#=========================================================================
export CRTDIR=${CRTDIR:-"/sbnd/data/users/$USER/crtModTests/"}
if [ ! -d $CRTDIR ]; then
    mkdir $CRTDIR
    mkdir $CRTDIR/tex
fi
if [ ! -f ${CRTDIR}/tex/CRTPlots.tex ]; then
    cp ../tex/CRTPlots.tex ${CRTDIR}/tex/CRTPlots.tex
fi

if [ $# -eq 0 ]
then
    echo "Source this script with run numbers as arguments"
else 
    source /cvmfs/sbnd.opensciencegrid.org/products/sbnd/setup_sbnd.sh
    setup sbndcode v09_60_01 -q e20:prof
    PWDstart=$PWD
    cd ../basic_adc_plots;
    runs=""
    #default locations of files on daq machines when running run_BERNCRTAna.sh
    daqfiledirbern=/home/nfs/sbnd/Aframe/runs/BERN/
    daqfiledirana=/home/nfs/sbnd/Aframe/runs/
    for run in "$@"
    do 
	echo "Working on run $run"
	if [ ! -f "${CRTDIR}/berncrt_hist_run$run.root" ] || [ ! -f "${CRTDIR}/run$run.root" ]; then
	    echo "Analyzer files not on gpvm. Getting them."
#=========================================
#Uncomment this section to run analyzer modules only on files that are missing from the daq machines
#	    ssh $USER@sbnd-gateway01.fnal.gov "
#	    if [ ! -f $daqfiledirana/run$run.root ] || [ ! -f $daqfiledirbern/berncrt_hist_run$run.root ]; then 
#                echo 'Running analyzer modules run ${run}';
#		ssh ${USER}@sbnd-evb04.fnal.gov '                                              
#                source /home/nfs/afilkins/run_BernCRTAna.sh ${run};'
#	    fi"
#================
#Uncomment this section to run analyzer modules for all given runs, even if a local copy already exists on the daq machine
	    echo 'Running analyzer modules run ${run}';
	    ssh $USER@sbnd-gateway01.fnal.gov "
                ssh ${USER}@sbnd-evb04.fnal.gov '
		source /home/nfs/afilkins/run_BernCRTAna.sh ${run};'"
	fi
#============
	if [ ! -f "${CRTDIR}/berncrt_hist_run$run.root" ]; then
	    scp $USER@sbnd-gateway01.fnal.gov:$daqfiledirbern/berncrt_hist_run$run.root $CRTDIR
	    chmod 777 ${CRTDIR}/berncrt_hist_run$run.root
	fi
	if [ ! -f "${CRTDIR}/run$run.root" ]; then
	    scp $USER@sbnd-gateway01.fnal.gov:$daqfiledirana/run$run.root ${CRTDIR}/
	    chmod 777 ${CRTDIR}/run$run.root
	fi

	echo $PWD
	./find_maximum_adc $run
	./CRTModuleTestHists $run
	chmod -R 777 ${CRTDIR}/run$run
	runs=$runs"_"$run
    done 
    ./PedestalRunPlots "$@"
    cd ${CRTDIR}/tex/
    pdflatex -interaction=nonstopmode CRTPlots.tex
    #echo $runs
    mv CRTPlots.pdf ../CRTPlots_run$runs.pdf
    chmod 777 ../CRTPlots_run$runs.pdf
    echo "Wrote ${CRTDIR}/CRTPlots_run$runs.pdf"
    #okular ../CRTPlots_run$runs.pdf &
    cd $PWDstart
fi
