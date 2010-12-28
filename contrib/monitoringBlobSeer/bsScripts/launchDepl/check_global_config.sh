#!/bin/bash

if [ -e "../globalEnv/env.sh" ]
then
    FILE="../globalEnv/env.sh"
elif [ -e "../../globalEnv/env.sh" ]
then
    FILE="../../globalEnv/env.sh"
elif [ -e "globalEnv/env.sh" ]
then
    FILE="globalEnv/env.sh"
elif [ -e "bsScripts/globalEnv/env.sh" ]
then
    FILE="bsScripts/globalEnv/env.sh"
else
    echo "No Global configuration file found. Please run this script from the scripts directory"
    exit 1
fi

echo $FILE
