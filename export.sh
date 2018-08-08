#########################################################################
# File Name: export.sh
# Author: ma6174
# mail: ma6174@163.com
# Created Time: Sun 01 Apr 2018 01:37:24 PM PDT
#########################################################################
#!/bin/bash
LIB=./lib
THRIFT_LIB=/thrift-0.9.2
LOG4CPP_LIB=/usr/local/lib
export LD_LIBRARY_PATH=${LOG4CPP_LIB}:${LIB}:${THRIFT_LIB}:${LD_LIBRARY_PATH}

