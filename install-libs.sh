#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
#echo $SCRIPT_DIR
eval "arduino-cli lib install ""$(sed 's/"/\\"/g' $SCRIPT_DIR/library-list.txt | xargs )"
