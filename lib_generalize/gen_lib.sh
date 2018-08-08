#########################################################################
# File Name: gen_lib.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: Wed 08 Aug 2018 01:33:20 AM PDT
#########################################################################
#!/bin/bash
ROOT_HOME=/home/richard/root
THRIFT_HOME=/thrift-0.9.2/
cd src
rm RootEventData_rootcint* *.so
rm RootEventData/RootEventData_rootcint*
${ROOT_HOME}/bin/rootcint -f RootEventData_rootcint.cxx -c  RootEventData/*.h  ./LinkDef.h
cp RootEventData_rootcint.* RootEventData/
cp ./LinkDef.h ./RootEventData/
g++ -Wall -fPIC -shared -o libthis.so  *.cxx `${ROOT_HOME}/bin/root-config --cflags --glibs`
rm ./RootEventData/LinkDef.h
cd ..
mv src/libthis.so ./lib/
cd hbaseConnect/
make
cd ..
mv hbaseConnect/libhbase.so ./lib/
