#!/bin/bash


case $SLURM_LOCALID in
0)
/home/lmoffatt/macro_dr/macro_dr/macro_dr ../macro_dr/models/scheme_1_inact.txt ../macro_dr/scripts/experiment.txt  ../macro_dr/scripts/simulation.txt "runIdNamePre= \"MH_Li_inact\"" ../macro_dr/scripts/evidence_data.txt
;;

1)
/home/lmoffatt/macro_dr/macro_dr/macro_dr ../macro_dr/models/scheme_2_inact.txt ../macro_dr/scripts/experiment.txt  ../macro_dr/scripts/simulation.txt "runIdNamePre= \"MH_Li_inact\"" ../macro_dr/scripts/evidence_data.txt
;;

2)
/home/lmoffatt/macro_dr/macro_dr/macro_dr ../macro_dr/models/scheme_3_inact.txt ../macro_dr/scripts/experiment.txt  ../macro_dr/scripts/simulation.txt "runIdNamePre= \"MH_Li_inact\"" ../macro_dr/scripts/evidence_data.txt
;;

3)
/home/lmoffatt/macro_dr/macro_dr/macro_dr ../macro_dr/models/scheme_4_inact.txt ../macro_dr/scripts/experiment.txt  ../macro_dr/scripts/simulation.txt "runIdNamePre= \"MH_Li_inact\"" ../macro_dr/scripts/evidence_data.txt
;;

esac
