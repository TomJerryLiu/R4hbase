THRIFT_DIR =/thrift-0.9.2/include
LIB_DIR =/thrift-0.9.2/lib 

GEN_SRC =  ./gen-cpp/hbase_types.cpp ./gen-cpp/hbase_constants.cpp ./gen-cpp/THBaseService.cpp

.PHONY: clean help

default:	HConnection.h HConnection.cpp
	g++ -Wall -fPIC -shared -o libhbase.so -I${THRIFT_DIR} -I${THRIFT_DIR}/thrift -L${LIB_DIR} HConnection.cpp ${GEN_SRC} -lthrift

clean:
		        rm -rf *.so

help:
				        $(warning "Makefile for C++ Hbase Thrift hbaseTools. Modify THRIFT_DIR and LIB_DIR in the \
									file to point to correct locations. See $${HBASE_ROOT}/hbase-examples/README.txt for \
									details.")
						        @:
