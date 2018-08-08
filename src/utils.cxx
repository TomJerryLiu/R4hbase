/*
 * utils.c
 *
 *  Created on: Apr 10, 2018
 *      Author: richard
 */
#include "utils.h"
/*string serialize(TObject* object,UInt_t& length){
	stringstream ss;
	TMessage* message = new TMessage(kMESS_OBJECT);
   	message->WriteObject(object);
	char * buffer = message->Buffer();
	length= message->Length();
	for(UInt_t i = 0;i<length;i++)
		ss<<buffer[i];
	string serStr = ss.str();
	delete message;
	return serStr;
}*/
char* serialize(TObject* object,UInt_t& length){
	TMessage* message = new TMessage(kMESS_OBJECT);
   	message->WriteObject(object);
	char * buffer = message->Buffer();
	length= message->Length();
	char *result = new char[length];
	for(UInt_t i = 0;i<length;i++)
		result[i] = buffer[i];
	delete message;
	return result;
}



/*TObject* deserialize(string buffer,UInt_t length){
	char * data = (char*)malloc((length+1)*sizeof(char));
	buffer.copy(data,length,0);
	MyTMessage *message_demo = new MyTMessage(data,length);
	TObject* object = message_demo->ReadObject(message_demo->GetClass());
	return object;
}*/
TObject* deserialize(char* buffer,UInt_t length){
	MyTMessage *message_demo = new MyTMessage(buffer,length);
	TObject* object = message_demo->ReadObject(message_demo->GetClass());
	return object;
}


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

long encode(const char *buf, const long size, char *base64Char) {
    long a = 0;
    int i = 0;
    while (i < size) {
        char b0 = buf[i++];
        char b1 = (i < size) ? buf[i++] : 0;
        char b2 = (i < size) ? buf[i++] : 0;

        int int63 = 0x3F; //  00111111
        int int255 = 0xFF; // 11111111
        base64Char[a++] = base64_chars[(b0 >> 2) & int63];
        base64Char[a++] = base64_chars[((b0 << 4) | ((b1 & int255) >> 4)) & int63];
        base64Char[a++] = base64_chars[((b1 << 2) | ((b2 & int255) >> 6)) & int63];
        base64Char[a++] = base64_chars[b2 & int63];
    }
    switch (size % 3) {
    	case 0:
    		base64Char[a]='\0';break;
    	case 1:
            base64Char[--a] = '=';
    	case 2:
    		base64Char[--a] = '=';
            base64Char[++a] = '\0';
    }
    return a;
}

char *decode(const char *base64Char, const long base64CharSize, char *originChar, long originCharSize) {
    int toInt[128] = {-1};
    for (int i = 0; i < 64; i++) {
        toInt[base64_chars[i]] = i;
    }
    int int255 = 0xFF;
    int index = 0;
    for (int i = 0; i < base64CharSize; i += 4) {
        int c0 = toInt[base64Char[i]];
        int c1 = toInt[base64Char[i + 1]];
        originChar[index++] = (((c0 << 2) | (c1 >> 4)) & int255);
        if (index >= originCharSize) {
            return originChar;
        }
        int c2 = toInt[base64Char[i + 2]];
        originChar[index++] = (((c1 << 4) | (c2 >> 2)) & int255);
        if (index >= originCharSize) {
            return originChar;
        }
        int c3 = toInt[base64Char[i + 3]];
        originChar[index++] = (((c2 << 6) | c3) & int255);
    }
    return originChar;
}

std::string base64_encode(unsigned char *bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64_decode(std::string & encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = 0; j < i; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}
