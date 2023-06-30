#!/bin/bash

# Check if poll-log file exists and has proper permissions
if [ ! -e "pollLog.txt" ] || [ ! -r "pollLog.txt" ]; then
  echo "Error: The poll-log file does not exist or you don't have the necessary permissions."
  exit 1
fi

# Create the pollerResultsFile
pollerResultsFile="pollerResultsFile.txt"
touch "$pollerResultsFile"

# Create arrays for vote and voters
declare -A voteCount
declare -A voters

# Read the poll-log file and count votes
while IFS= read -r line; do
  voterName=$(echo "$line" | awk '{print $1 " " $2}')
  partyName=$(echo "$line" | awk '{print $NF}')

  # Check for duplicate voters
  if [[ -z ${voters[$voterName]} ]]; then
    ((voteCount[$partyName]++))
    voters[$voterName]=1
  fi
done < "pollLog.txt"

# Write the results to pollerResultsFile
for party in "${!voteCount[@]}"; do
  echo "$party ${voteCount[$party]}" >> "$pollerResultsFile"
done
