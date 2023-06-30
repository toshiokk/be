echo "Installing packages needed for building BE editor on Ubuntu ..."

sudo apt update
echo "pausing ..."; read key

sudo apt install make automake g++ nkf cppcheck
echo "pausing ..."; read key
