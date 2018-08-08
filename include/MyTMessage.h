#ifndef TMESSAGE_H
#define TMESSAGE_H

#include "TMessage.h"
//#include<stdio.h>
//#include<iostream>
//using namespace std;
class MyTMessage:public TMessage{
public:
	MyTMessage(void *buf, Int_t len);
};
#endif
