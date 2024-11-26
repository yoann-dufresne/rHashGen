#!/usr/bin/bash

# set -e
# set -o pipefail

mkdir -p "results"
resdir="results/$(date -I)"
mkdir -p "$resdir"
printf "Data saved in $resdir\n"

printf "Start expe at $(date -Isecond)\n"
SECONDS=0 # Bash' env
s=0
while [[ 1 ]] ; do
    s=$((s+1))
    cmd="../../release/search @2024-11-26_expe__32bits_sample-test_HC-rand-init.status --seed=${s} --nb-tests=$(((2**32-1)/1000)) > ${resdir}/run_s${s}.yaml 2>${resdir}/run_s${s}.log"
    printf "Run $s..."
    echo "$cmd"
    $cmd
    if [[ $? > 0 ]]; then
        printf "Run $s... FAIL"
    else
        printf "Run $s...   OK"
    fi
    duration=$SECONDS
    printf " at $((duration/60)) minutes $((duration % 60)) seconds\n"
done

