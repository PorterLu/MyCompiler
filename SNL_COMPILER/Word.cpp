#include "Word.h"
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
using namespace std;
bool wordScanner::isChar()
{
	Token* token;
	char ch;
	ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
        ch=getChar();
	if(ch!='\'')
    {
        undoChar();
		return false;
    }
	ch=getChar();          //������ĵ�һ���ַ�
    if(isNumber(ch)||isLetter(ch))
	{
        tempString+=ch;
        ch=getChar();             //����ĸ�����ֺ�ĵ�һ���ַ�
    }
    else
	{
        tempString='\'';
        undoChar();
		wordErrorState=true;
    }

	if(!wordErrorState)
	{
		if(ch=='\'')
		{
			token=new Token(tempString,"char",curLine);
			tokenList.push(*token);
			//cout<<tempString<<endl;
			tempString="";
			return true;
		}
		else
		{
			tempString='\'';
			undoChar();         //  �������Σ������������ '12 ������ʽ
			undoChar();
			wordErrorState=true;
		}
    }

	if(wordErrorState==true)
	{
	     string str;
	     str="�����";
	     str+=curLine+0x30;
	     str+="���д��󵥴ʣ� ";
	     str+=tempString;
	     tempString="";
	     error.push_back(str);
		 //cout<<tempString<<endl;
         return false;
    }
}

bool wordScanner::isInteger()
{
	Token* token;
	char ch;
	ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
        ch=getChar();
	if(!isNumber(ch))
    {
        undoChar();
		return false;
    }
	while(isNumber(ch))
	{
        tempString+=ch;
        ch=getChar();
    }
    undoChar();     //�����ļ��ַ�ָ��һλ
	token=new Token(tempString,"integer",curLine);
    tokenList.push(*token);
    //cout<<tempString<<endl;
	tempString="";
    return true;
}

bool wordScanner::isSingleBoundary()
{
	Token* token;
	char ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
		ch=getChar();
	if((ch=='+')||(ch=='-')||(ch=='*')||(ch=='/')||(ch=='<')||(ch=='=')||(ch=='(')||(ch==')')||(ch=='[')||(ch==']')||(ch==';')||(ch==','))
	{
		tempString+=ch;
		token=new Token(tempString,"singleBoundary",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
		return true;
	}
	else
	{
		undoChar();
		return false;
	}
}

bool wordScanner::isDoubleBoundary()
{
	Token* token;
	string str;
	char ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
        ch=getChar();
	if(ch!=':')
	{
		undoChar();
		return false;
	}
	tempString+=ch;
    ch=getChar();
    if(ch=='='){
        tempString+=ch;     //˫�ֽ�
		token=new Token(tempString,"doubleBoundary",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
        return true;
    }
    else
	{
        undoChar();
        str="�����"+curLine<+"���д��󵥴ʣ� "+tempString;
		error.push_back(str);
		wordErrorState=true;
		tempString="";
		return false;
    }
}

bool wordScanner::isNotes()
{
	char ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
        ch=getChar();
	if(ch!='{')
	{
		undoChar();
		return false;
	}
	do
	{
           ch=getChar();
		   tempString+=ch;
	}while((!file.eof()&&ch!='}'));

	if(ch!='}')
	{
		 string str;
	     str="�����";
	     str+=curLine+0x30;
	     str+="���д��󵥴ʣ� ";
	     str+=tempString;
	     tempString="";
	     error.push_back(str);
	     wordErrorState=true;
	}
	else
	{
	    //cout<<tempString<<endl;
		tempString="";
		return true;
	}
}

int wordScanner::isArray()
{
	Token* token;
	char ch=getChar();
    while(ch=='\n'||ch=='\r'||ch=='\t'||ch==' ')
        ch=getChar();
	if(ch!='.')
	{
		undoChar();
		return false;
	}
	tempString+=ch;
    ch=getChar();
    if(ch=='.'){     //����ֽ�
        tempString+=ch;
		token=new Token(tempString,"arrayBound",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
        return true;
    }
    else{            //�������,ֱ�ӷ���һ����Ϊһ����
        undoChar();     //�����ļ��ַ�ָ��һλ
        return 2;
    }
}

/*�ܹ���20�������֣�����header.h�ж��壬�������������飬
�뵱ǰ��ʶ���Ƚϣ������жϣ�����Ĳ���Ϊ�����Ǳ����ֵı�ʶ��*/
bool wordScanner::isReservedWord(string tempString)
{
	int i;
	for(i=0;i<=20;i++)
	{
		if(tempString==reservedWord[i])
			return true;
	}
	return false;
}

/*�ж��Ƿ��Ǳ�ʶ���ĺ���������ֵΪ0���Ǳ�ʶ����1ʱ�Ǳ�ʶ����2ʱ�Ǳ����֣����ｫ�����ֿ�������ı�ʶ��*/
int wordScanner::isID()
{
	char curChar;  //��ǰ�ַ�
	Token* token;
	bool isOver=false,isID=false; //isOver��ʾ�Ƿ�����Ƴ��Զ�����isID��ʾ�Ƿ��Ǳ�ʶ��
	int transTable[2][2]={{-1,1}, {1,1}};//ת�������Զ�����״̬ת�����飬-1��ʾ�������״̬
	int state=0; //�Զ���ʼ״̬
	curChar = getChar(); //��ȡһ���ַ�
	//��ʼǰ�ȳ����հ��ַ����ո��Ʊ������س�
    while(curChar=='\n'||curChar=='\r'||curChar=='\t'||curChar==' ')
        curChar=getChar();
	//�����ʶ���жϵ��Զ�������ĸ��ͷ�����ֺ���ĸ��ɵ��ַ���
	while(((curChar>=48&&curChar<=57)||(curChar>=65&&curChar<=90)||(curChar>=97&&curChar<=122))&&!isOver)//ȷ���Ƿ����ڱ�ʾ�����ַ�������
	{
		switch(state)
		{
			case 0:
				if(curChar>=48&&curChar<=57)  //״̬0�ǳ�ʼ״̬���������֣���ζ�����ֿ�ͷ�������϶��壬�˳��Զ���
					isOver=true;
				else
				{
					state=transTable[state][1]; //ת��
                    tempString+=curChar; //�����ַ���
					curChar=getChar(); //ȥ��һ���ַ�
					isID=true;		//�ַ�����һ��״̬ʱ��ֹ״̬
				}
				break;
			case 1:
				if(curChar>=48&&curChar<=57) //����
				{
					state=transTable[state][0];
                    tempString+=curChar;
					curChar=getChar();
					isID=true;
				}
				else  //��ĸ
				{
					state=transTable[state][1];
                    tempString+=curChar;
					curChar=getChar();
					isID=true;
				}
		}
	}
	undoChar(); //����һ���ַ�����Ϊ���µ��ַ�һ�����Ǳ�ʶ���Ĳ���
	if(isID&&isReservedWord(tempString))//�Ƿ��Ǳ�����
	{
		token=new Token(tempString,"reservedWord",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
		return 2;
	}
	else if(isID)
	{
		token=new Token(tempString,"id",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
		return 1;
	}
	else
		return 0;
}

void wordScanner::printResult()
{
	int i=0;
	Token* token;
	for(i=0;i<tokenList.num;i++)
	{
		token=&tokenList.List[i];
		cout<<token->name<<" "<<token->type<<" ��"<<token->line<<"��"<<endl;
	}
}

void wordScanner::createTokenFile()
{
    int i;
    Token* token;
    string str=fileName.substr(0,fileName.length()-3)+"token";
    FILE* tempFile=fopen(str.c_str(),"w");
	for(i=0;i<tokenList.num;i++)
	{
	    token=&(tokenList.List[i]);
        fprintf(tempFile,"%s %s %d\n",(token->name).c_str(),(token->type).c_str(),token->line);
	}
	fclose(tempFile);
}

void wordScanner::start()
{
    //file.open(fileName);
    cout<<"word start"<<endl;
	int ans; //�жϷ�
	char ch;
	cout<<"----------------------�ʷ�����---------------------"<<endl;
	while(!file.eof())//�ļ�δ����
	{

		if(ans=isID()){}
		else if(ans=isChar()){}
		else if(ans=isInteger()){}
		else if(ans=isSingleBoundary()){}
		else if(ans=isDoubleBoundary()){}
		else if(ans=isNotes()){}
		else if(ans=isArray()){if(ans==2) break;}
		else
		{
			 ch=getChar();
			 string str;
             str="δ֪���� "+ch;
             str+=" ��";
             str+=curLine+0x30;
             str+="��";
             tempString="";
             error.push_back(str);
		}
	}

	if(file.eof())
        wordErrorState=false;

	if(wordErrorState==false)
    {
        createTokenFile();
        printResult();
    }

    cout<<"word finish"<<endl;
	//system("pause");
}