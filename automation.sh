#!/bin/bash
limit=3
interval=60
echo "" > raw_data.txt
while [ $limit -gt 0 ]; do
    ./cew
    echo "API Call Made At $(date)"
    limit=$((limit - 1))
    
    if [ $limit -gt 0 ]; then
        sleep "$interval"
    fi
done
./proccess_data
echo "Saved The Proccessed Data To proccessed_data.txt File"
./email
