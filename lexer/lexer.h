#include<cstring>
#include<cstdio>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<set>
#include<map>
#include<fstream>

using namespace std;

namespace Lexer{
	
	const int BUFFSZ = 1e3;
	
	int cntDFA ;
	
	void ERROR(string v = ""){
		cerr << v <<" error!!!" << endl;
		exit(1);
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
	
	/* 查看nc是否是某终结符的开头 */
	bool inEnd(int nc){
		string name = "";
		name.push_back(nc);
		if(!endNum.count(name))return false;
		else return true;
	}
	
	
	/* 识别终结符 */
	void goEnd(){
		string name = "";
		int nc = nextchar();
		bool flag = 0;
		while(((char)nc) == ' ' || ((char)nc) == '\n' || ((char)nc) == '\r'){
			flag = 1;
			nc = nextchar();
		}
		if(flag){
			prechar(); 
			return ;
		}
		if(nc == -1) return ;
		name.push_back((char)nc);
		nc = nextchar();
		string name2 = name;
		name2.push_back((char)nc);
		if(endNum.count(name2)){
			LexRes.push_back((result){name2 , name2 , " "});
			return ;
		}
		prechar();
		if(endNum.count(name)){
			LexRes.push_back((result){name , name , " "});
			return ;
		}
		cout << name << " " << name2 <<endl;
		ERROR("end");
		return ;
	}
	
	
	/* 把解析结果存下来 */
	void putRes(string name , string type){
		if(type == "Integer"){
			LexRes.push_back((result){type , name , name});
		}
		if(type == "VariableOrKeyword"){
			if(!endNum.count(name)){
				LexRes.push_back((result){"Variable" , name , name});
			}
			else{
				LexRes.push_back((result){name , name , " "});
			}
		}
	}
	
	/* 跑dfa */
	void goDFA(string typeLine){
		DFA dfa = dfaList[DFAnum[typeLine]];
		int now = dfa.start;
		string name = "" , type = typeLine;
		for(;;){
			int nc = nextchar();
			if(!dfa.mp[now].count(nc)){
				if(dfa.isEnd[now]){
					putRes(name , typeLine);
					int oc = prechar();
					//cout << (char) oc << endl;
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
	
	void goNotes(){
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
		addDFA("VariableOrKeywordDFA.txt");
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
				goDFA("Integer");
				goEnd();
			}
			else if(nc == '_' || isalpha(nc)){
				prechar();
				goDFA("VariableOrKeyword");
				goEnd();
			}
			else if(inEnd(nc)){
				prechar();
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


