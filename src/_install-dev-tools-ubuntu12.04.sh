echo "Installing packages needed for building BE editor on Ubuntu ..."

sudo apt-get update
echo "pausing ..."; read key

sudo apt-get install build-essential
echo "pausing ..."; read key

##sudo apt-get install autoconf
#sudo apt-get install automake
#echo "pausing ..."; read key

sudo apt-get install nkf
echo "pausing ..."; read key

sudo apt-get install gettext
echo "pausing ..."; read key

#sudo apt-get install console-data
#echo "pausing ..."; read key

sudo apt-get install libtool

#sudo apt-get install pandoc
