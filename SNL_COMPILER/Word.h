#ifndef WORD_H_INCLUDED
#define WORD_H_INCLUDED

#include<string.h>
#include<fstream>
#include<cstdlib>
#include<iostream>
#include"header.h"
using namespace std;

struct wordScanner
{
    int note;

	//保留字数组
	string reservedWord[21]={"program","procedure","type","var","if","then","else","fi",
		"while","do","endwh","begin","end","read","write","array",
		"of","record","return","integer","char"};

	ifstream file;
	HWND hwnd;
	string fileName;
	string tempString;//取得的单词
	int curLine;//当前行
	int offset;//文件偏移
	TokenList tokenList;
	vector<string> error;

	wordScanner(TCHAR* fileName,HWND hwnd)
	{
	    note=0;
		curLine=1;
		offset=0;
		tempString="";
		this->fileName=fileName;

		file.open(fileName); //打开文件，后期如果有UI设计一定不是这么打开的
	}

	wordScanner()
	{
	    note=0;
		curLine=1;
		offset=0;
		tempString="";

		//file.open(fileName); //打开文件，后期如果有UI设计一定不是这么打开的
	}

	bool isReservedWord(string tempString);//是否时保留字，作为isID()子程序
	int isID();//是否是标识符
	bool isChar();
	bool isInteger();
	bool isSingleBoundary();
	bool isDoubleBoundary();
	bool isNotes();
	int isArray();
	void start();
	void printResult();
	void createTokenFile();

	char getChar()//文件末尾需要一个#为记号
	{
		char ch1,ch2,ch;

		if(offset>=3)
        {
            file.seekg(offset-1);
            ch1=file.get();
            file.seekg(offset-2);
            ch2=file.get();
            if(ch1=='\n'&&ch2=='\n')
                curLine++;
        }
		file.seekg(offset);
		ch=file.get();
		offset++;
		return ch;
	}

	void undoChar()//文件末尾需要一个#为记号
	{
		char ch1,ch2,ch;
		if(offset>=3)
        {
            file.seekg(offset-2);
            ch1=file.get();
            file.seekg(offset-3);
            ch2=file.get();
            if(ch1== '\n'&&ch2=='\n')
                curLine--;
        }
        offset--;
	}

	bool isNumber(char ch)
	{     //判断一个字符是否为数字
		if((ch>='0')&&(ch<='9'))
			return true;
		else
			return false;
	}

	bool isLetter(char ch)
	{        //判断一个字符是否为字母
		if ((ch>='a')&&(ch<='z'))
			return true;
		if ((ch>='A')&&(ch<='Z'))
			return true;
		return false;
	}

	~wordScanner()
	{
	    int i;
	    Token* token;
/*
		for(i=0;i<tokenList.List.size();i++) //释放空间
		{
			delete(&(tokenList.List[i]));
		}
*/
		file.close();
	}
};




#endif // WORD_H_INCLUDED
