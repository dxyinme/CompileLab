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
	const int classLen = 12;
	const int BUFFSZ = 1e3;
	
	int cntDFA ;
	
	vector<pair<int , string> > errorVec;
	
	void ERROR(int line , string msg = ""){
		errorVec.push_back({line , msg});
	}
	
	string classFix(string o){
		if(o.size() >= classLen) return o;
		while(o.size() < classLen)o.push_back(' ');
		return o;
	}
	
	struct result{
		string type;
		string name;
		string value;
		pair<string , pair<string , string> > toNormal(){
			return {name , {type , value}} ;
		}
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
	vector<int> lineRec;
	vector<DFA> dfaList;
	map<string , string> endTran;
	map<string , int> DFAnum;
	vector<string> context;
	map<string , int> endNum;
	int nowCol , nowRow;
	
	void addDFA(string FileName){
		DFA now;
		now.build(FileName);
		dfaList.push_back(now);
		string name = "";
		for(int i=4;i + 7 < FileName.size();i++){
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
		errorVec.clear();
		nowRow = 0;
		nowCol = -1;
	}
	
	
	int nextchar(){
		++nowCol ;
		if(nowCol >= context[nowRow].size()){
			nowRow ++;
			nowCol = 0;
		}
		
		//cout << nowRow << " " << nowCol << endl;
		
		if(nowRow >= context.size()){
			nowRow = context.size();
			return -1;
		}
		return context[nowRow][nowCol];
	}
	
	int prechar(){
		--nowCol ;
		if(nowCol < 0){
			nowRow --;
			if(nowRow < 0) {
				nowRow = 0;
				return -1;
			}
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
	
	/* 查看nc是否是注释的开头 */
	bool isNotes(int nc){
		string name = "";
		name.push_back(nc);
		name.push_back(nextchar());
		bool res;
		if(name == "/*") res = 1;
		else res = 0;
		prechar();
		return res;
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
			LexRes.push_back((result){endTran[name2] , name2 , "_"});
			lineRec.push_back(nowRow);
			return ;
		}
		prechar();
		if(endNum.count(name)){
			LexRes.push_back((result){endTran[name] , name , "_"});
            lineRec.push_back(nowRow);
			return ;
		}
		//cout << name << " " << name2 <<endl;
		
		ERROR(nowRow,"error : " + name);
		return ;
	}
	
	
	/* 把解析结果存下来 */
	void putRes(string name , string type){
		//cout << name << endl;
		if(type == "number"){
			LexRes.push_back((result){"Const" , name , name});
            lineRec.push_back(nowRow);
		}
		if(type == "VariableOrKeyword"){
			if(!endNum.count(name)){
				LexRes.push_back((result){"Variable" , name , name});
                lineRec.push_back(nowRow);
			}
			else{
				LexRes.push_back((result){endTran[name] , name , "_"});
                lineRec.push_back(nowRow);
			}
		}
		if(type == "Notes"){
			name = "/* ... */";
			LexRes.push_back((result){"Notes" , name , name});
            lineRec.push_back(nowRow);
		}
	}
	
	/* 跑dfa */
	void goDFA(string typeLine){
		DFA dfa = dfaList[DFAnum[typeLine]];
		int now = dfa.start;
		string name = "" , type = typeLine;
		for(;;){
			int nc = nextchar();  // cout << nowRow << " " << nowCol << " " << nc << endl;
			if(nc == -1) {
				ERROR(nowRow , "error ：" + name);
				return ;
			}
			if(nc < -1) nc = 32;
			if(typeLine == "Notes"){
				//cout << nc <<" " << now << endl;
				if(dfa.isEnd[now]){
					//cout << name <<endl;
					putRes(name , typeLine);
					prechar();
					return ;
				}
			}
			if(!dfa.mp[now].count(nc)){
				if(dfa.isEnd[now]){
					putRes(name , typeLine);
					int oc = prechar();
					//cout << (char) oc << endl;
					return ;
				}
				else{
					while(nc != -1 && !inEnd(nc)){
						name.push_back((char)nc);
						nc = nextchar();
					}
					ERROR(nowRow , "error : " + name);
					prechar();
					return ;
				}
			}
			else{
				now = dfa.mp[now][nc];
				name.push_back((char)nc);
			}
		}
	}
	
	
	/* 词法分析 */
	int goLex( string FileName ){
		string prefix = "txt/";
		ifstream in;
		in.open(prefix+"reend2.txt");
		int num; in >> num;
		for(int i=1;i<=num;i++){
			string End;
			int Num;
			in >> End >> Num;
			endNum[End] = Num;
		}

		in.close();
		//读入词号转换表
        in.open(prefix+"tran.txt");
        in >> num;
        for(int i=1;i<=num;i++){
            string End,Tran;
            in >> End >> Tran;
            endTran[End] = Tran;
        }
        in.close();

		init();
		addDFA(prefix+"numberDFA.txt");
		addDFA(prefix+"VariableOrKeywordDFA.txt");
		addDFA(prefix+"NotesDFA.txt");
		string now = "";
		in.open(FileName.c_str());
		while(getline(in , now)){
			now.push_back('\n');
			context.push_back(now);
		}
		in.close();
		//cout << context.size() << endl;
		//for (auto it : context){cout << it << endl;}
		int nc;

		while((nc = nextchar()) != -1){
			if(isdigit(nc)){
				prechar();
				goDFA("number");
				goEnd();
			}
			else if(nc == '_' || isalpha(nc)){
				prechar();
				goDFA("VariableOrKeyword");
				goEnd();
			}
			else if(isNotes(nc)){
				prechar();
				//cout << "??????????" << endl;
				goDFA("Notes");
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
	bool print(){
		if(errorVec.size() == 0){
			for(int i=0;i<LexRes.size();i++){
				cout << classFix(LexRes[i].name) << " <" << classFix(LexRes[i].type) << "," << classFix(LexRes[i].value) << ">" <<lineRec[i]<< "\n";
			}
			return true;
		}
		else{
			for(int i=0;i<errorVec.size();i++){
				cout << "line:" << errorVec[i].first << " " << errorVec[i].second << "\n";
			}
			return false;
		}
	}
}


