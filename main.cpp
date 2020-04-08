#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<map>
#include <set>
#include "grammar/grammar.h"
using namespace std;

string prefix = "txt/";



int main(){
	freopen("tmp.txt","w",stdout);
	Lexer::goLex(prefix+"test.txt");
	Lexer::print();
	Grammar::init(prefix+"generator.txt");
	Grammar::goGrammar(Lexer::LexRes);
	return 0;
}
