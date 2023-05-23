#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <windows.h>
#include<vector>
#include<string>
#include<cmath>
#include<iostream>
using namespace std;
#define UNTITLED TEXT ("(untitled)")
extern bool wordErrorState;
extern bool grammarErrorState;
struct Token
{
    string name;
	string type;
	int line;
	Token(string name,string type,int line)
	{
		this->name=name;
		this->type=type;
		this->line=line;
	}
};

//Token���е����ݽṹ
struct TokenList
{
	vector<Token> List;  //���ڴ洢token
	int num;			//token����
	int pos;			//��ǰ��ָ��ָ����һ��token

	void push(Token& token) //������ѹ��һ��token
	{
		List.push_back(token);
		num++;
	}

	Token get()   //ȡ��һ��token��ָ�����һλ
	{
		Token temp = List[pos];
		pos++;
		return temp;
	}

	void unget(){pos=pos>0?pos-1:0;} //ָ�����һλ

	TokenList(){num=0; pos=0;}
};

struct Node
{
    string name;
    vector<Node*> son;
    Node* father;
    int curSon;

    Node(string n){
        name=n;
        father=NULL;
        curSon=0;
        son.clear();
    }

    Node* getSon(){
        int tempNum=son.size()-1;
        if(curSon>tempNum)
            return NULL;
        Node* temp=son[curSon];
        curSon++;
        return temp;
    }

};


#endif // HEADER_H_INCLUDED
