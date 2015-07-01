export fn=./cmake_install.sh
wget -O $fn http://www.cmake.org/files/v2.8/cmake-2.8.12-Linux-i386.sh 1>&2
echo "Press Q and then say yes to all. Press Enter to continue." && read
bash ${fn}
ln -f -s ./cmake-*/bin/cmake mycmake
ln -f -s ./cmake-*/bin/ccmake myccmake
rm -R ./cmake_install.sh
echo "Cmake installed in current directory. A link has been created as well to use './mycmake' command right away."
