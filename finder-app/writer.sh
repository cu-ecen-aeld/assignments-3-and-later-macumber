#!/bin/bash
# Script for writing text to files
# Author: Dan Macumber

set -e
set -u

WRITEFILE=""
WRITESTR=""

if [ $# -lt 2 ];
then
	echo "Usage: writer.sh writefile writestr"
	exit 1
else
	WRITEFILE=$1
	WRITESTR=$2
fi

WRITEDIR=$(dirname "$WRITEFILE")

mkdir -p "$WRITEDIR"
echo "$WRITESTR" > "$WRITEFILE"
