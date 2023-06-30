echo "Installing packages needed for building BE editor on Ubuntu ..."

sudo apt-get update
echo "pausing ..."; read key

sudo apt-get install build-essential
echo "pausing ..."; read key

#sudo apt-get install autoconf
sudo apt-get install automake
echo "pausing ..."; read key

sudo apt-get install libncursesw5
sudo apt-get install libncursesw5-dev
echo "pausing ..."; read key

sudo apt-get install libpcre3-dev
echo "pausing ..."; read key

sudo apt-get install nkf
echo "pausing ..."; read key

#sudo apt-get install console-data
#echo "pausing ..."; read key

sudo apt-get install libtool
