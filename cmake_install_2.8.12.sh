export fn=/tmp/cmake.sh && ls $fn
(echo "use previous $fn? Enter for yes, ctrl+d for no." && read) || (wget -O $fn http://www.cmake.org/files/v2.8/cmake-2.8.12-Linux-i386.sh 1>&2)
cd /tmp
sudo bash ${fn}
#echo sudo ln -f -s /tmp/cmake*/bin/cmake /usr/local/bin/cmake
cd -
