#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<fstream>
#include<map>
#include "lexer/lexer.h"
using namespace std;




int main(){
	Lexer::goLex("test.txt");
	Lexer::print();
	return 0;
}