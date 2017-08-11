
#include <node.h>
#include <v8.h>

#include <map>
#include <list>
#include <string>

#include "pinyin.h"

using namespace v8;

void getPinyin(const v8::FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = Isolate::GetCurrent();

    // Check the number of arguments passed.
    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong number of arguments")));
      return;
    }

    // Check the argument types
    if (!args[0]->IsString() ) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong arguments")));
      return;
    }

    // Perform the operation
    String::Value inputString(args[0]);
	const uint16_t* pArrayStringUnicode = *inputString;
    Local<Array> arrayResult = Array::New(isolate);


    // convert hanzi to pinyin
    list<list<u16string>> listCharPinyin;
    Pinyin::getInstance()->convert(pArrayStringUnicode,inputString.length(), listCharPinyin);

    // organize output
    int charIndex = 0 ;
    for(list<list<u16string>>::const_iterator charIter  = listCharPinyin.begin(); charIter != listCharPinyin.end(); charIter++) {

        const list<u16string> & listSingleCharPinyin = *charIter;

        // organize pinyin array for one char
        Local<Array> arraySingleCharPinyin = Array::New(isolate);
        int pinyinIndex = 0 ;
        for(list<u16string>::const_iterator pinyinIter = listSingleCharPinyin.begin(); pinyinIter != listSingleCharPinyin.end(); pinyinIter++) {

            Local<String> singlePinyin = String::NewFromTwoByte(isolate,(uint16_t *)((*pinyinIter).c_str()));
            arraySingleCharPinyin->Set(pinyinIndex,singlePinyin);

            pinyinIndex++;
        }

        arrayResult->Set(charIndex, arraySingleCharPinyin);

        charIndex++;
    }
    args.GetReturnValue().Set(arrayResult);
}

void Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  exports->Set(String::NewFromUtf8(isolate, "getPinyin"), FunctionTemplate::New(isolate, getPinyin)->GetFunction());
}

NODE_MODULE(hello, Init)
