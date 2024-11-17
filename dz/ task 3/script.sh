#!/bin/bash

for i in {1..20}; do
    echo $i.txt > $i.txt
done 
find . -type f ! -name "*4*" ! -name "script.sh" -exec rm {} \;
echo "chto-nibud" > 14.txt
for each in $(find . -type f -name "*.txt"); do
    ln -sf $each .
done



