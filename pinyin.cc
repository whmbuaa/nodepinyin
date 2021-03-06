#include "pinyin.h"
#include "dict_zi.h"

#include <set>

Pinyin* Pinyin::m_pInstance = NULL;

Pinyin::Pinyin(map<int,const char*> & dict):m_mapDict(dict){

}

const Pinyin* Pinyin::getInstance(){
    if(m_pInstance == NULL) {
        m_pInstance = new Pinyin(getDict());
    }
    return m_pInstance;
}

void Pinyin::convert (const uint16_t inputString[], const unsigned len, list<list<u16string>> & output) const {

    u16string strNonChinese;
    for(unsigned charIndex  = 0 ; charIndex < len; charIndex++) {

        if(m_mapDict.count(inputString[charIndex]) == 0 ){
            strNonChinese.append((char16_t *)&inputString[charIndex],1);
        } else {
            if(strNonChinese.length() > 0) {
                output.push_back(list<u16string>(1, strNonChinese));
                strNonChinese.clear();
            }
            list<u16string> listOfSingleCharPinyin;

            u16string strPinyinOfSingleChar((char16_t *)m_mapDict[inputString[charIndex]]);
            // replace phonetic
            replaceChar(strPinyinOfSingleChar);
            // divide pinyin string and remove duplication
            divideString(listOfSingleCharPinyin,strPinyinOfSingleChar, u',');

            output.push_back(listOfSingleCharPinyin);
        }

    }

    //in case no chinese character at the end
    if(strNonChinese.length() > 0) {
         output.push_back(list<u16string>(1, strNonChinese));
    }

}

u16string & Pinyin::replaceChar(u16string& strSrc){

    map<char16_t, char16_t> mapPhonetic = getPhoneticMap();
    for(map<char16_t, char16_t>::const_iterator iterPhoneticMap = mapPhonetic.begin(); iterPhoneticMap != mapPhonetic.end(); iterPhoneticMap++ ) {

        char16_t charToReplace = iterPhoneticMap->first;
        char16_t charReplaceWith = iterPhoneticMap->second;

        size_t startPos = 0;
        size_t foundPos = 0 ;
        while( ( foundPos = strSrc.find(charToReplace, startPos)) != string::npos) {
            strSrc.replace(foundPos,1,1,charReplaceWith);
            startPos = foundPos+1;
        }
    }
    return strSrc;
}
list<u16string> & Pinyin::divideString(list<u16string> & listOutput,u16string strSrc, char16_t delimit){

    size_t startPos = 0 ;
    size_t foundPos = 0 ;
    set<u16string> strSet;  // to remove the duplicated pinyin
     while( ( foundPos = strSrc.find(delimit, startPos)) != string::npos) {
        strSet.insert(strSrc.substr(startPos, foundPos-startPos));
        startPos = foundPos+1;
    }

    if(startPos < strSrc.length()){
        strSet.insert(strSrc.substr(startPos, strSrc.length()-startPos));
    }

    for(set<u16string>::const_iterator iterStrSet = strSet.begin(); iterStrSet != strSet.end(); iterStrSet++) {
        listOutput.push_back(*iterStrSet);
    }
    return listOutput;
}
/*****************************************************************************
 * 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码.
 *
 * 参数:
 *    unic     字符的Unicode编码值
 *    pOutput  指向输出的用于存储UTF8编码值的缓冲区的指针
 *    outsize  pOutput缓冲的大小
 *
 * 返回值:
 *    返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 .
 *
 * 注意:
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求;
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种;
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
 *     2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小!
 ****************************************************************************/
unsigned  Pinyin::unicodeToUtf8(const uint32_t unic, char *pOutput,  const unsigned outSize)
{

    if ( unic <= 0x0000007F )
    {
        // * U-00000000 - U-0000007F:  0xxxxxxx
        *pOutput     = (unic & 0x7F);
        return 1;
    }
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )
    {
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        *(pOutput+1) = (unic & 0x3F) | 0x80;
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    }
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
    {
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        *(pOutput+2) = (unic & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
    {
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+3) = (unic & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    }
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
    {
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+4) = (unic & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    }
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
    {
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+5) = (unic & 0x3F) | 0x80;
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}