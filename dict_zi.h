#ifndef DICT_ZI_H
#define DICT_ZI_H

#include <map>

using namespace std ;

map<int, const char*>& getDict();
map<char16_t, char16_t>& getPhoneticMap();
#endif