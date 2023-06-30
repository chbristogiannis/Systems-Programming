#!/bin/bash

# Check the number of arguments
if [ "$#" -ne 2 ]; then
  echo "Whrong number of arguments"
  exit 1
fi

# Checks if the file exists
if [ ! -f "$1" ]; then
  echo "File $1 does not exists"
  exit 1
fi

# Checks if the number is interger
if ! [[ "$2" =~ ^[0-9]+$ ]]; then
  echo "Second argument must be a interger"
  exit 1
fi

# Save political parties from politicalParties.txt
mapfile -t parties < "$1"
numParties=${#parties[@]}

# Create input file
inputFile="inputFile.txt"
rm -f "$inputFile"  # Delete previous

# Random name creation
for (( i=0; i<$2; i++ )); do
  firstName=$(cat /dev/urandom | tr -dc 'a-z' | fold -w $(( RANDOM % 10 + 3 )) | head -n 1 | sed 's/.*/\u&/')
  lastName=$(cat /dev/urandom | tr -dc 'a-z' | fold -w $(( RANDOM % 10 + 3 )) | head -n 1 | sed 's/.*/\u&/')
  partyIndex=$(( RANDOM % numParties ))
  party=${parties[$partyIndex]}
  echo "$firstName $lastName $party" >> "$inputFile"
done

echo "File $inputFile is created with $2 lines"





#cat /dev/urandom: Αυτή η εντολή διαβάζει τυχαία δεδομένα από τη συσκευή /dev/urandom.
#tr -dc 'a-z': Αυτή η εντολή χρησιμοποιεί το πρόγραμμα tr για να απομακρύνει όλους τους μη αλφαβητικούς χαρακτήρες, κρατώντας μόνο τους πεζούς λατινικούς χαρακτήρες από το 'a' έως το 'z'.
#fold -w $(( RANDOM % 10 + 3 )): Αυτό ουσιαστικά προσθέτει τυχαίο μήκος από 3 έως 12 χαρακτήρες σε κάθε γραμμή.
#head -n 1: Αυτή η εντολή επιλέγει μόνο την πρώτη γραμμή από την εισαγόμενη προηγούμενη εντολή.
#sed 's/.*/\u&/': Αυτή η εντολή χρησιμοποιεί το πρόγραμμα sed για να αλλάξει την πρώτη γραμμή κειμένου έτσι ώστε να καταλήγει σε κεφαλαίο γράμμα.

#mapfile -t parties < "$1": Αυτή η εντολή χρησιμοποιεί το πρόγραμμα mapfile για να διαβάσει τις γραμμές από το αρχείο "$1" και να τις αποθηκεύσει σε έναν πίνακα με όνομα parties. Το -t δηλώνει ότι οι γραμμές θα αφαιρεθούν από τα τελικά κενά χαρακτήρες.
#numParties=${#parties[@]}: Αυτή η εντολή υπολογίζει τον αριθμό των στοιχείων στον πίνακα parties και τον αποθηκεύει στη μεταβλητή numParties. Το ${#parties[@]} επιστρέφει τον αριθμό των στοιχείων στον πίνακα parties.
