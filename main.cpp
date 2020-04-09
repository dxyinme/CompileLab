#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<map>
#include <set>
#include "lexer/lexer.h"
#include "parser/parser.h"
using namespace std;

string prefix = "txt/";



int main(){
	//freopen(prefix + "tmp.txt","w",stdout);
	Lexer::goLex(prefix+"test.txt");
	Lexer::print(prefix+"result/"+"lexer.txt");
	Parser::lineRec = Lexer::lineRec;
	Parser::endTran = Lexer::endTran;
	vector<Parser::result> newLexRes;
	for(auto x : Lexer::LexRes){
		newLexRes.push_back((Parser::result){x.type , x.name , x.value});
	} 
	Parser::init(prefix+"generator.txt");
	Parser::goParser(newLexRes);
	Parser::printFile(prefix+"result/"+"parser.txt");
	return 0;
}
