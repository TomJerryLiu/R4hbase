# R4hbase
1、配置文件说明./conf/htree.conf
配置从hbase中取数据的需要的一些参数
比如thrift_ip地址和端口号，日志文件的路径和最大日志文件的大小以及日志文件的最大数目
其中hbase表名和列簇名没有默认值，必须在配置文件或者在程序中设置，其他参数都有默认值
2、lib_generalize:产生libthis.so和libhbase.so两个库文件
需要先修改gen_lib.sh和hbaseConnect/Makefile中的thrift和root的安装路径才能正常执行
执行 ./gen_lib.sh 后在
