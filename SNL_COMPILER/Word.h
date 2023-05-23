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

	//����������
	string reservedWord[21]={"program","procedure","type","var","if","then","else","fi",
		"while","do","endwh","begin","end","read","write","array",
		"of","record","return","integer","char"};

	ifstream file;
	HWND hwnd;
	string fileName;
	string tempString;//ȡ�õĵ���
	int curLine;//��ǰ��
	int offset;//�ļ�ƫ��
	TokenList tokenList;
	vector<string> error;

	wordScanner(TCHAR* fileName,HWND hwnd)
	{
	    note=0;
		curLine=1;
		offset=0;
		tempString="";
		this->fileName=fileName;

		file.open(fileName); //���ļ������������UI���һ��������ô�򿪵�
	}

	wordScanner()
	{
	    note=0;
		curLine=1;
		offset=0;
		tempString="";

		//file.open(fileName); //���ļ������������UI���һ��������ô�򿪵�
	}

	bool isReservedWord(string tempString);//�Ƿ�ʱ�����֣���ΪisID()�ӳ���
	int isID();//�Ƿ��Ǳ�ʶ��
	bool isChar();
	bool isInteger();
	bool isSingleBoundary();
	bool isDoubleBoundary();
	bool isNotes();
	int isArray();
	void start();
	void printResult();
	void createTokenFile();

	char getChar()//�ļ�ĩβ��Ҫһ��#Ϊ�Ǻ�
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

	void undoChar()//�ļ�ĩβ��Ҫһ��#Ϊ�Ǻ�
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
	{     //�ж�һ���ַ��Ƿ�Ϊ����
		if((ch>='0')&&(ch<='9'))
			return true;
		else
			return false;
	}

	bool isLetter(char ch)
	{        //�ж�һ���ַ��Ƿ�Ϊ��ĸ
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
		for(i=0;i<tokenList.List.size();i++) //�ͷſռ�
		{
			delete(&(tokenList.List[i]));
		}
*/
		file.close();
	}
};




#endif // WORD_H_INCLUDED
