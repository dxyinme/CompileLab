#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<set>
#include<fstream>

using namespace std;

namespace Lexer{
	
	const int BUFFSZ = 1e3;
	
	int cntDFA ;
	
	void ERROR(){
		cerr << "error!!!" << endl;
	}
	
	struct result{
		string type;
		string name;
		string value;
	};
	
	struct DFA{
		DFA(){}
		int ID;
		int sz;
		int start;
		vector<int> isEnd;
		vector<map<int,int> > mp;
		void build(string DFAFileName){
			mp.clear();
			isEnd.clear();
			ID = cntDFA++;
			ifstream in;
			in.open(DFAFileName.c_str());
			int m;
			in >> sz >> m;
			for(int i=0;i<sz;i++){
				map<int,int> o;
				mp.push_back(o);
				isEnd.push_back(0);
			}
			in >> start ;
			int x , cnt;
			in >> cnt ;
			for(int i=0;i<cnt;i++){
				in >> x;
				isEnd[x] = 1;
			}
			for(int i=1;i<=m;i++){
				int u,v,w;in>>u>>v>>w;
				mp[u][w] = v;
			}
			in.close();
		}
	};
	
	vector<result> LexRes;
	vector<DFA> dfaList;
	map<string , int> DFAnum;
	vector<string> context;
	map<string , int> endNum;
	int nowCol , nowRow;
	
	void addDFA(string FileName){
		DFA now;
		now.build(FileName);
		dfaList.push_back(now);
		string name = "";
		for(int i=0;i + 7 < FileName.size();i++){
			name.push_back(FileName[i]);
		}
		cout << name << " " << FileName << endl;
		DFAnum[name] = cntDFA-1;
	}
	
	
	
	void init(){
		cntDFA = 0;
		LexRes.clear();
		dfaList.clear();
		DFAnum.clear();
		context.clear();
		nowRow = 0;
		nowCol = -1;
	}
	
	
	int nextchar(){
		++nowCol ;
		if(nowCol >= context[nowRow].size()){
			nowRow ++;
			nowCol = 0;
		}
		if(nowRow >= context.size()) return -1;
		return context[nowRow][nowCol];
	}
	
	int prechar(){
		--nowCol ;
		if(nowCol < 0){
			nowRow --;
			if(nowRow < 0) return -1;
			nowCol = (int)context[nowRow].size() - 1;
		}
		return context[nowRow][nowCol];
	}
	
	void goNumber(){
		DFA dfa = dfaList[DFAnum["Integer"]];
		int now = dfa.start;
		string name = "";
		cerr << " ? " << endl;
		for(;;){
			int nc = nextchar();
			if(!dfa.mp[now].count(nc)){
				if(dfa.isEnd[now]){
					LexRes.push_back((result){"CONST" , name , name});
					prechar();
					return ;
				}
				else{
					ERROR();
				}
			}
			else{
				now = dfa.mp[now][nc];
				name.push_back((char)nc);
			}
		}
	}
	
	void goEnd(){
		//todo
	}
	
	/* 词法分析 */
	int goLex( string FileName ){
		ifstream in;
		in.open("reend.txt");
		int num; in >> num;
		for(int i=1;i<=num;i++){
			string End ;
			int Num;
			in >> End >> Num;
			endNum[End] = Num;
		}
		in.close();
		init();
		
		addDFA("IntegerDFA.txt");
		string now = "";
		in.open(FileName.c_str());
		while(getline(in , now)){
			context.push_back(now); now.push_back('\n');
			//cout<< now << " "<< context.size() << endl;
		}
		in.close();
		int nc;
		while((nc = nextchar()) != -1){
			if(isdigit(nc)){
				prechar();
				goNumber();
				goEnd();
			}
			else{
				continue;
			}
		}
		
		return 0;
	}
	
	/*输出结果*/
	void print(){
		for(int i=0;i<LexRes.size();i++){
			cout << LexRes[i].name << " <" << LexRes[i].type << "," << LexRes[i].value << ">" << "\n";
		}
	}
}


