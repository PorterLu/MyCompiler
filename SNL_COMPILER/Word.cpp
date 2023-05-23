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
	ch=getChar();          //读’后的第一个字符
    if(isNumber(ch)||isLetter(ch))
	{
        tempString+=ch;
        ch=getChar();             //读字母或数字后的第一个字符
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
			undoChar();         //  回退两次，解决出现类似 '12 这种形式
			undoChar();
			wordErrorState=true;
		}
    }

	if(wordErrorState==true)
	{
	     string str;
	     str="程序第";
	     str+=curLine+0x30;
	     str+="行有错误单词： ";
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
    undoChar();     //回退文件字符指针一位
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
        tempString+=ch;     //双分界
		token=new Token(tempString,"doubleBoundary",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
        return true;
    }
    else
	{
        undoChar();
        str="程序第"+curLine<+"行有错误单词： "+tempString;
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
	     str="程序第";
	     str+=curLine+0x30;
	     str+="行有错误单词： ";
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
    if(ch=='.'){     //数组分界
        tempString+=ch;
		token=new Token(tempString,"arrayBound",curLine);
		tokenList.push(*token);
		//cout<<tempString<<endl;
		tempString="";
        return true;
    }
    else{            //程序结束,直接返回一个点为一个词
        undoChar();     //回退文件字符指针一位
        return 2;
    }
}

/*总共有20个保留字，已在header.h中定义，遍历保留字数组，
与当前标识符比较，进行判断，输入的参数为可能是保留字的标识符*/
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

/*判断是否是标识符的函数，返回值为0不是标识符，1时是标识符，2时是保留字，这里将保留字看作特殊的标识符*/
int wordScanner::isID()
{
	char curChar;  //当前字符
	Token* token;
	bool isOver=false,isID=false; //isOver表示是否可以推出自动机，isID表示是否是标识符
	int transTable[2][2]={{-1,1}, {1,1}};//转换表，自动机的状态转换数组，-1表示进入错误状态
	int state=0; //自动初始状态
	curChar = getChar(); //获取一个字符
	//开始前先除掉空白字符，空格，制表符，回车
    while(curChar=='\n'||curChar=='\r'||curChar=='\t'||curChar==' ')
        curChar=getChar();
	//进入标识符判断的自动机，字母开头，数字和字母组成的字符串
	while(((curChar>=48&&curChar<=57)||(curChar>=65&&curChar<=90)||(curChar>=97&&curChar<=122))&&!isOver)//确定是否是在表示符的字符集合里
	{
		switch(state)
		{
			case 0:
				if(curChar>=48&&curChar<=57)  //状态0是初始状态，输入数字，意味着数字开头，不符合定义，退出自动机
					isOver=true;
				else
				{
					state=transTable[state][1]; //转换
                    tempString+=curChar; //生成字符串
					curChar=getChar(); //去下一个字符
					isID=true;		//字符串，一号状态时终止状态
				}
				break;
			case 1:
				if(curChar>=48&&curChar<=57) //数字
				{
					state=transTable[state][0];
                    tempString+=curChar;
					curChar=getChar();
					isID=true;
				}
				else  //字母
				{
					state=transTable[state][1];
                    tempString+=curChar;
					curChar=getChar();
					isID=true;
				}
		}
	}
	undoChar(); //回退一个字符，因为最新的字符一定不是标识符的部分
	if(isID&&isReservedWord(tempString))//是否是保留字
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
		cout<<token->name<<" "<<token->type<<" 第"<<token->line<<"行"<<endl;
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
	int ans; //判断符
	char ch;
	cout<<"----------------------词法分析---------------------"<<endl;
	while(!file.eof())//文件未结束
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
             str="未知错误 "+ch;
             str+=" 第";
             str+=curLine+0x30;
             str+="行";
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
