#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<set>
#include<stack>
#include<map>
#include<fstream>

using namespace std;

namespace Parser{
	
	struct Token{
		string name , type , value ;
	};
	
	struct result{
		bool isError;
		string subname;
		vector<int> sons;
	};
	
	void ERROR(string msg = "?"){
		cerr << "error:" << msg << endl;
	}


	int root; // 语法分析树的根节点
	int nowToken;
	Token now;

	/**
	 * 语法分析树
	 */
	vector<result> ParseRes;

	/**
	 * token序列
	 */
	vector<Token> tokenList;

	/**
	 * stack
	 */
	stack<Token> Stack;

	/**
	 * 预测分析表
	 */
	map<string , map<string , string> > mp;

	void loadMap(){
		string prefix = "txt/";
		ifstream in;
		in.open(prefix+"map.txt");

		in.close();
	}

	void load(vector<pair<string , pair<string , string > > > lex){
		tokenList.clear();
		for(auto p : lex){
			tokenList.push_back((Token){p.first , p.second.first , p.second.second});
		}
		cout << tokenList.size() << endl;
	}
	
	
	void nextToken(){
		++nowToken;
		if(nowToken >= tokenList.size()){
			nowToken = tokenList.size();
			now = ((Token){ "" , "$" , ""}) ;
		}
		else now = tokenList[nowToken];
	}
	

	bool isThis(Token token , string temp){
		return (token.type == temp);
	}

	void init(){
		nowToken = -1;
		loadMap();
	}
	
	
	
	// 输出语法树
	void print(string s,int x){
		cout << s << ParseRes[x].subname << endl;
		for(auto to : ParseRes[x].sons){
			print(s + " " , to);
		}
	}

	void goParser(){
		init();
		
	}
	
}