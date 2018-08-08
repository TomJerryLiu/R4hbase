/*
 * utils.h
 *
 *  Created on: Apr 10, 2018
 *      Author: richard
 */

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include <string>
#include "MyTMessage.h"
#include <stdlib.h>
#include <stdio.h>
#include "TROOT.h"
#include "TBufferFile.h"
#include <iostream>
#include <sstream>
using namespace std;
static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
long encode(const char *buf, const long size, char *base64Char);
char *decode(const char *base64Char, const long base64CharSize, char *originChar, long originCharSize);
std::string base64_encode(char * , unsigned int len);
std::string base64_decode(std::string & s);
char* serialize(TObject* tObject,UInt_t &length);
TObject * deserialize(char * buffer,UInt_t length);
template<typename T> string toString(const T& t){
	stringstream ss;  //创建一个格式化输出流
    ss<<t;             //把值传递如流中
    return ss.str();
}
//TObject* deserialize(string buffer,UInt_t length);
//string serialize(TObject * object,UInt_t& length);

#endif /* INCLUDE_UTILS_H_ */
