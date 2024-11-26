#!/usr/bin/bash

# set -e
# set -o pipefail

mkdir -p "results"
resdir="results/$(date -I)"
mkdir -p "$resdir"

s=0
while [[ 1 ]] ; do
    s=$((s+1))
    printf "Run $s..."
    ../../release/search @2024-11-26_expe__8bits_full-test_HC-rand-init.status --seed=${s} > ${resdir}/run_s${s}.yaml 2>${resdir}/run_s${s}.log
    if [[ $? > 0 ]]; then
        printf "Run $s... FAIL\n"
    else
        printf " OK\n"
    fi
done

