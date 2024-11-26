#!/usr/bin/bash

# set -e
# set -o pipefail

mkdir -p "results"
resdir="results/$(date -I)"
mkdir -p "$resdir"
printf "Data saved in $resdir"

printf "Start expe at $(date -Isecond)\n"
SECONDS=0 # Bash' env
s=0
while [[ 1 ]] ; do
    s=$((s+1))
    cmd="../../release/search @2024-11-26_expe__8bits_full-test_HC-rand-init.status --seed=${s}"
    printf "Run $s...\n"
    echo "$cmd"
    $cmd > ${resdir}/run_s${s}.yaml 2>${resdir}/run_s${s}.log
    if [[ $? > 0 ]]; then
        printf "Run $s... FAIL"
    else
        printf "Run $s...   OK"
    fi
    duration=$SECONDS
    printf " at $((duration/60)) minutes $((duration % 60)) seconds\n"
done

