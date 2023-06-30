#!/bin/bash

# Check if inputFile exists and has proper permissions
if [ ! -e "inputFile.txt" ] || [ ! -r "inputFile.txt" ]; then
  echo "Error: The inputFile does not exist or you don't have the necessary permissions."
  exit 1
fi

# Create the tallyResultsFile
tallyResultsFile="$1"
touch "$tallyResultsFile"

# Initialize arrays for vote counting and tracking voters
declare -a parties
declare -a voteCount
declare -a voters

# Read political parties from inputFile
mapfile -t parties < "political_parties.txt"
numParties=${#parties[@]}

# Initialize voteCount array with zeros for each party
for ((i = 0; i < numParties; i++)); do
  voteCount[$i]=0
done

# Read inputFile and count votes
while IFS= read -r line; do
  voterName=$(echo "$line" | awk '{print $1 " " $2}')
  partyName=$(echo "$line" | awk '{print $NF}')

  # Check for duplicate voters
  if [[ ! " ${voters[@]} " =~ " ${voterName} " ]]; then
    for ((i = 0; i < numParties; i++)); do
      if [[ "${parties[$i]}" == "$partyName" ]]; then
        ((voteCount[$i]++))
        break
      fi
    done
    voters+=("$voterName")
  fi
done < "inputFile.txt"

# Write the results to tallyResultsFile
for ((i = 0; i < numParties; i++)); do
  echo "${parties[$i]} ${voteCount[$i]}" >> "$tallyResultsFile"
done
