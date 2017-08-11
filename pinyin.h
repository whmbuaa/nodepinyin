
#ifndef PINYIN_H
#define PINYIN_H

#include <map>
#include <string>
#include <list>

using namespace std;

class Pinyin
{
	protected :
	    map<int,const char*> &m_mapDict;
        static Pinyin* m_pInstance;

        Pinyin(map<int,const char*> & dict);
        static unsigned unicodeToUtf8(const uint32_t unic, char *pOutput,  const unsigned outSize);
        static u16string & replaceChar(u16string& stringSrc);
        static list<u16string> & divideString(list<u16string> & listOutput,u16string strSrc, char16_t delimit);
	public :
	    static const Pinyin* getInstance();
	    void convert (const uint16_t  inputString[] , const unsigned len, list<list<u16string>> & output) const;

};

#endif

