#所有的目录做成变量，方便修改和移植 
BIN = ./bin
SRC = ./src
INC = ./include
THRIFT_HOME=/thrift-0.9.2
OBJ = ./obj
LIB = ./lib    
	#提前所有源文件(即：*.c文件)和所有中间文件(即：*.o)
SOURCE = $(wildcard ${SRC}/*.cxx)
OBJECT = $(patsubst %.cxx,${OBJ}/%.o,$(notdir ${SOURCE}))
	   
	   #设置最后目标文件
TARGET = R4hbase
BIN_TARGET = ${BIN}/${TARGET}
CC = g++ 
CFLAGS = -g -Wall -I${INC} -I${THRIFT_HOME}/include -I${THRIFT_HOME}/include/thrift -L${THRIFT_HOME}/lib -L${LIB} `root-config --cflags --glibs` -lthis -lhbase -llog4cpp -lpthread -lthrift
			 
#用所有中间文件生成目的文件，规则中可以用 $^替换掉 ${OBJECT}
${BIN_TARGET}:	${OBJECT}
	$(CC) -o $@ ${CFLAGS} ${OBJECT}
				    
					#生成各个中间文件
${OBJ}/%.o:	${SRC}/%.cxx
	$(CC) $(CFLAGS) -o $@ -c $< ${CFLAGS}
#	$(CC) $(CFLAGS) -o $@ -c $<  L${LIB}  `root-config --cflags --glibs` -lthis
.PHONY:	clean
clean:
	find $(OBJ) -name *.o -exec rm -rf 
	rm -rf $(BIN_TARGET)
