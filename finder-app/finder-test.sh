#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

set -e
set -u

NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data
username=$(cat conf/username.txt)

# Clean any previous build artifacts
#make clean

# Compile your writer application using native compilation
#make

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

rm -rf "${WRITEDIR}"

# Create $WRITEDIR if not assignment1
assignment=$(cat conf/assignment.txt)

if [ "$assignment" != 'assignment1' ]; then
    mkdir -p "$WRITEDIR"
fi

# Loop to use your "writer" utility instead of "writer.sh" script
for i in $(seq 1 $NUMFILES); do
    ./writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
done

OUTPUTSTRING=$(./finder.sh "$WRITEDIR" "$WRITESTR")

# Remove temporary directories
rm -rf /tmp/aeld-data

set +e
echo ${OUTPUTSTRING} | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
    echo "success"
    exit 0
else
    echo "failed: expected  ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
    exit 1
fi

