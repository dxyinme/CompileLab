//
// Created by sinemora on 2020/4/7.
//
#include <map>
#include <sstream>
#include <fstream>
using namespace std;
namespace Parser{
    bool debug=false;
    struct Node;
    void eliSelfRecur(int mId);
    void eliPassRecur();
    void eliCommonPreffix();
    void getFirst(const Node &node);
    void getFirstSet();
    void getEndSet();
    void getSelectSet();
    void eliNone();
    struct Node{
        bool isEnd;
        int id;
        Node(int _id,bool _isEnd):id(_id),isEnd(_isEnd){}
        bool operator == (const Node &b) const{
            return isEnd==b.isEnd&&id==b.id;
        }
    };

    // = lexer::result
    struct result{
		string type;
		string name;
		string value;
	};


    vector<string> errVec; 

    void ERROR(string msg){
        errVec.push_back(msg);
    }

    void ERROR(int line,string type,string sgn){
        stringstream ss;
        ss.str("");
        ss << line;
        string o;
        ss >> o;
        errVec.push_back("Error at Line "+o+": can't select "+type+" from "+sgn);
    }

    struct Rule{
        vector<Node> rights;
        Rule getSubRule(int lp,int rp){
            Rule res;
            res.rights.clear();
            for(int i = lp;i <= rp;i++){
                res.rights.push_back(rights[i]);
            }
            return res;
        }
        Rule operator + (const Rule &b){
            Rule res;
            res.rights.clear();
            for(auto &t:rights)res.rights.push_back(t);
            for(auto &t:b.rights)res.rights.push_back(t);
            return res;
        }
        bool contain(const Rule &b){
            if(b.rights.size() > rights.size()) return false;
            for(int i = 0;i < b.rights.size();i++){
                if(!(b.rights[i] == rights[i])) return false;
            }
            return true;
        }
        Rule getSuffix(int p){
            Rule res;
            res.rights.clear();
            for(int i = p;i < rights.size();i++) {
                res.rights.push_back(rights[i]);
            }
            return res;
        }
        bool isNone() const{
            return rights.empty();
        }
    };
    struct Mid{
        string sgn;
        bool canNone=false,canEnd=false,vis=false;
        vector<Rule> rules;
        set<int> follow,first;
        map<int,int> select;
        Mid(){}
        Mid(string &_name):sgn(_name){}
    };
    vector<string> ends;
    map<string,int> endToId;
    vector<Mid> mids;
    map<string,int> midToId;
    map<string,int> cnts;
    map<string,string> endTran;

    // token 所在的行号
    vector<int> lineRec;

    struct TreeNode{
        vector<int> sons;
        string name,attr;
        int lineNum;
        bool hide=false;
        TreeNode(){}
        TreeNode(int _n,string _name,string _attr):lineNum(_n),name(_name),attr(_attr){}
    };
    vector<TreeNode> resultTree(1);
    void print(const Rule &rule){
        for(auto &t:rule.rights){
            if(t.isEnd) cout<<"<"<<ends[t.id]<<"> ";
            else cout<<mids[t.id].sgn<<" ";
        }
    }
    void print(const Mid &mid){
        cout<<mid.sgn<<" -> ";
        for(auto &t:mid.rules){
            print(t);
            cout<<"|";
        }
        cout<<endl;
    }
    void print(const TreeNode &treeNode,int deep){
        for(int i = 1;i < deep;i++){
            cout<<"  ";
        }
        cout<<treeNode.name;
        if(treeNode.attr!="_"&&treeNode.attr!=" "){
            cout<<" :"<<treeNode.attr;
        }
        cout<<"("<<treeNode.lineNum<<")"<<endl;
        for(int t:treeNode.sons){
            print(resultTree[t],deep+1);
        }
    }
    bool judgeId(const Node &node,int id){
        return !node.isEnd && node.id == id;
    }
    Node getNode(int id){
        return Node(id,false);
    }
    Rule getRule(int id){
        Rule res;
        res.rights.clear();
        res.rights.push_back(getNode(id));
        return res;
    }
    /**
     * 获取所有终结节点的名称，并标号
     */
    void getEnds(){
        ends.clear();
        for(auto it:endTran){
            ends.push_back(it.second);
            endToId[it.second] = (int)ends.size()-1;
        }
    }
    int addMid(string midName){
        if(midToId.count(midName)) return midToId[midName];
        mids.emplace_back(midName);
        midToId[midName] = (int)mids.size()-1;
        return midToId[midName];
    }
    /**
     * 初始化
     * @param fileName 词法规则文件
     */
    void init(const string &fileName){
        getEnds();
        ifstream in;
        in.open(fileName);
        if(!in.is_open()){
            cerr<<"open "<<fileName<<" wrong"<<endl;
            return;
        }
        string nowString;
        Rule nowRule;
        int stat=0,nowMid=0;
        while(in>>nowString){
            if(nowString=="#"){
                stat = 0;
            }else if(nowString=="->"){
                stat = 1;
            }else if(stat==0){
                nowMid = addMid(nowString);
            }else if(nowString != "$"){
                if(endTran.count(nowString)){
                    nowRule.rights.emplace_back(endToId[endTran[nowString]],true);
                }else if(nowString == "none"){
                    mids[nowMid].canNone = true;
                }else{
                    nowRule.rights.emplace_back(addMid(nowString),false);
                }
            }else if(nowString == "$"){
                mids[nowMid].rules.push_back(nowRule);
                nowRule.rights.clear();
            }else{
                ERROR("Parse Error");
                cerr<<"Parse Error"<<endl;
                return;
            }
        }
        eliPassRecur();
        eliCommonPreffix();
        eliNone();
        getFirstSet();
        getEndSet();
        getSelectSet();
        /*for(int i = 0;i < mids.size();i++){
            cout<<mids[i].sgn<<" -> ";
            for(auto t:mids[i].follow){
                cout<<ends[t]<<" ";
            }
            cout<<endl;
        }*/
        for(int i = 0;i < mids.size();i++){
            print(mids[i]);
        }
    }
    void eliSelfRecur(int mId){

        if(mids[mId].rules.empty()) return;
        vector<Rule> &now = mids[mId].rules;
        int spl = 0,n = now.size();
        for(int i = 0;i < n;i++){
            if(!now[i].rights.empty() && judgeId(now[i].rights[0],mId)){
                swap(now[i],now[spl]);
                spl++;
            }
        }
        if(spl==0)return;
        int nId = addMid(mids[mId].sgn+"'");
        vector<Rule> &nxt = mids[nId].rules;
        for(int i = 0;i < spl;i++){
            nxt.push_back(now[i].getSuffix(1)+getRule(nId));
        }
        mids[nId].canNone = true;
        nxt.emplace_back();
        for(int i = spl;i < n;i++){
            now[i] = now[i] + getRule(nId);
        }
        now.erase(now.begin(),now.begin()+spl);
    }
    void eliPassRecur(){
        vector<Rule> copyRule;
        for(int i = 0;i < mids.size();i++){
            for(int j = 0;j < i;j++){
                copyRule.clear();
                auto it = mids[i].rules.begin();
                while(it!=mids[i].rules.end()){
                    if(!((*it).rights.empty()) && judgeId((*it).rights[0],j)){
                        for(auto &tmp:mids[j].rules){
                            copyRule.push_back(tmp+(*it).getSuffix(1));
                        }
                    }else{
                        copyRule.push_back(*it);
                    }
                    ++it;
                }
                mids[i].rules = copyRule;
            }
            eliSelfRecur(i);
        }
    }
    Rule getLCP(const Rule& a,const Rule& b){
        Rule res;
        res.rights.clear();
        if(a.isNone()||b.isNone()) return res;
        for(int i = 0;i < a.rights.size() && i < b.rights.size();i++){
            if(a.rights[i] == b.rights[i]) res.rights.push_back(a.rights[i]);
            else break;
        }
        return res;
    }
    void eliCommonPreffix(){
        int cnt=0;
        Rule LCP;
        for(int i = 0;i < mids.size();i++){
            bool flag = true;
            while(flag){
                flag = false;
                for(int j = 0;j < mids[i].rules.size();j++){
                    for(int k = j+1;k < mids[i].rules.size();k++){
                        LCP = getLCP(mids[i].rules[j],mids[i].rules[k]);
                        if(!LCP.isNone()){
                            flag=true;
                            int nId = addMid(mids[i].sgn+to_string(++cnt));
                            mids[nId].rules.push_back(mids[i].rules[j].getSuffix((int)LCP.rights.size()));
                            mids[nId].rules.push_back(mids[i].rules[k].getSuffix((int)LCP.rights.size()));
                            mids[i].rules[j] = LCP + getRule(nId);
                            mids[i].rules.erase(mids[i].rules.begin()+k);
                            for(int t = 0;t < mids[i].rules.size();t++){
                                if(t==j||t==k) continue;
                                if(mids[i].rules[t].contain(LCP)){
                                    mids[nId].rules.push_back(mids[i].rules[t].getSuffix((int)LCP.rights.size()));
                                    mids[i].rules.erase(mids[i].rules.begin()+t);
                                }
                            }
                        }
                        if(flag) break;
                    }
                    if(flag)break;
                }
            }
        }
    }
    void eliNone(){
        for(auto &now:mids){
            auto it = now.rules.begin();
            while(it != now.rules.end()){
                if(it->rights.empty()){
                    now.canNone=true;
                    //it = now.rules.erase(it);
                    ++it;
                }else ++it;
            }
        }
    }
    void getFirst(const Rule &rule, bool &canNone,set<int> &first){
        bool flag = true;
        for(auto &nt:rule.rights){
            if(nt.isEnd) {first.insert(nt.id);flag=false;break;}
            else{
                getFirst(nt);
                for(auto it:mids[nt.id].first){

                    first.insert(it);
                }
                if(!mids[nt.id].canNone){
                    flag = false;
                    break;
                }
            }
        }
        if(flag) canNone = true;
    }
    void getFirst(const Node &node){
        if(node.isEnd) return;
        if(mids[node.id].vis) return;
        mids[node.id].vis = true;
        for(auto &t:mids[node.id].rules){
            getFirst(t,mids[node.id].canNone,mids[node.id].first);
        }
    }
    void getFirstSet(){
        int n = (int)mids.size();
        for(int i = 0;i < n;i++){
            getFirst(getNode(i));
        }
    }
    void getEndSet(){
        mids[0].canEnd = true;
        bool flag = true;
        while(flag){
            flag = false;
            for(int i = 0;i < mids.size();i++){
                for(int j = 0;j < mids.size();j++){
                    for(int tt = 0;tt < mids[j].rules.size();tt++) {
                        for (int k = 0; k < mids[j].rules[tt].rights.size(); k++) {
                            Node &now = mids[j].rules[tt].rights[k];
                            if (now == getNode(i)) {
                                bool canNone = false;
                                set<int> first;
                                getFirst(mids[j].rules[tt].getSuffix(k + 1), canNone, first);
                                if (canNone) {
                                    if (mids[j].canEnd && !mids[i].canEnd) {
                                        flag = true;
                                        mids[i].canEnd = true;
                                    }
                                    for (auto t:mids[j].follow) {
                                        if (!mids[i].follow.count(t)) {
                                            flag = true;
                                            mids[i].follow.insert(t);
                                        }
                                    }
                                }
                                for (auto t:first) {
                                    if (!mids[i].follow.count(t)) {
                                        flag = true;
                                        mids[i].follow.insert(t);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    void getSelectSet(){
        for(auto &mid:mids){
            for(int j = 0;j < mid.rules.size();j++){
                bool canNone=false;
                set<int> first;
                getFirst(mid.rules[j],canNone,first);
                for(auto t:first){
                    if(mid.select.count(t)){
                        cout<<"Error Select generate "<<mid.sgn<<" "<<ends[t]<<endl;
                        return;
                    }
                    mid.select[t] = j;
                }
                if(canNone){
                    for(auto t:mid.follow){
                        if(mid.select.count(t)){
                            cout<<"Error Select generate "<<mid.sgn<<" "<<ends[t]<<endl;
                            return;
                        }
                        mid.select[t] = j;
                    }
                }
            }

            cout<<mid.sgn<<" -> ";
            for(auto &t:mid.select){
                cout<<"("<<ends[t.first]<<","<<t.second<<")";
            }
            cout<<endl;

        }
    }
    int extend(Mid &mid,int treeNode,int &pos,vector<result> &lex){
        resultTree[treeNode].hide=true;
        if(pos >= lex.size()){
            if(mid.canNone) return 0;
            ERROR("Error :nothing to read!");
            cerr<<"Error :nothing to read!"<<endl;
            return -1;
        }
        if(!mid.select.count(endToId[lex[pos].type])){
            ERROR(lineRec[pos],lex[pos].type,mid.sgn);
            cerr<<"Error at Line "<<lineRec[pos]<<": can't select "<<lex[pos].type<<" from "<<mid.sgn<<endl;
            return -1;
        }

        Rule &rule = mid.rules[mid.select[endToId[lex[pos].type]]];
        for(auto &node:rule.rights){
            if(node.isEnd && node.id==endToId[lex[pos].type]){
                resultTree.emplace_back(lineRec[pos],lex[pos].type,lex[pos].value);
                resultTree[treeNode].sons.push_back((int)resultTree.size()-1);
                //cout<<resultTree.size()<<endl;
                pos++;
            }else if(!node.isEnd){
                resultTree.emplace_back(lineRec[pos],mids[node.id].sgn,"_");
                resultTree[treeNode].sons.push_back((int)resultTree.size()-1);
                //cout<<treeNode.sons.size()<<endl;
                int ret = extend(mids[node.id],(int)resultTree.size()-1,pos,lex);
                if(ret==-1)return -1;
            }else{
                //ERROR(lineRec[pos],lex[pos].type,mid.sgn);
                cerr<<"Error at Line "<<lineRec[pos]<<": wrong sign "<<lex[pos].type<<" from "<<mid.sgn<<endl;
            }
        }
        for(auto t:resultTree[treeNode].sons){
            if(!resultTree[t].hide){
                resultTree[treeNode].hide = false;
                break;
            }
        }
        return 0;
    }
    void eliHide(TreeNode &treeNode){
        auto it = treeNode.sons.begin();
        while(it!=treeNode.sons.end()){
            if(resultTree[*it].hide){
                it = treeNode.sons.erase(it);
            }else{
                ++it;
            }
        }
    }
    void goParser(vector<Parser::result> &lex){
        resultTree[0].lineNum = 0;
        resultTree[0].name = mids[0].sgn;
        resultTree[0].attr = "_";
        int pos = 0;
        if(extend(mids[0],0,pos,lex)==-1)return;
        for(int i = 0;i < resultTree.size();i++) eliHide(resultTree[i]);
        // print(resultTree[0],1);
    }

    //输出到文件
    void printFile(string filename){
        freopen(filename.c_str() , "w" , stdout);
        if(errVec.size() == 0){
            print(resultTree[0] , 1);
        }
        else{
            for(auto s : errVec){
                cout << s << endl;
            }
        }
        fclose(stdout);
    }
}