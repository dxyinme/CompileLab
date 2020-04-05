#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<map>
#include "lexer/lexer.h"
#include "parser/parser.h"
using namespace std;

string prefix = "txt/";



int main(){
	cout<<"小测试"<<endl;
	Lexer::goLex(prefix+"test.txt");
	bool lexerIsOK = Lexer::print();
	int Count = Lexer::LexRes.size();
	vector<pair<string , pair<string , string> > > o;
	o.clear();
	for(int i=0;i<Count;i++){
		o.push_back(Lexer::LexRes[i].toNormal());
	}
	Parser::Load(o);
	Parser::goParser();
	return 0;
}
