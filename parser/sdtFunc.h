//
// Created by Administrator on 2020/4/27.
//

#ifndef COMPILELAB_SDTFUNC_H
#define COMPILELAB_SDTFUNC_H

#include <iostream>
#define SON(x) resultTree[treeNode.sons[(x)]],this
#define TNAME(x) resultTree[treeNode.sons[(x)]].attr
struct TreeNode{
    vector<int> sons;
    string name,attr;
    int lineNum,opt;
    bool hide=false;
    TreeNode(){}
    TreeNode(int _n,string _name,string _attr):lineNum(_n),name(_name),attr(_attr){}
};
vector<TreeNode> resultTree(1);

namespace SDT {
    int offset=0,offsetFUNC=0,labelId=1,w=0,temp=0,cntTemp=0;
    string t="";
    bool inEval= false;
    map<string,pair<string,int> > vars;
    map<string,int> varwidth;
    map<string,int> funcs,funcAddr;
    map<string,string> funCode;
    vector<string> q;
    vector<int> tempLabel;
    struct CodeLine{
        string op,t1,t2,res;

    };
    vector<CodeLine> codes(1);
    int newLabel(){
        tempLabel.push_back(0);
        return (-1)*(int)(tempLabel.size()-1);
    }
    int writeLabel(int pos){
        tempLabel[-pos] = labelId;
    }
    string newtemp(){
        return "_t"+to_string(++cntTemp);
    }
    void enter(const string &name,const string &type,int offset,int width){
        vars[name]=make_pair(type,offset);
        varwidth[name]=width;
    }
    void enterFUNC(const string &name,int offsetFUNC){
        funcs[name] = offsetFUNC;
    }
    bool checkvar(const string &name){
        if(vars.count(name)) return true;
        cout<<"ERROR"<<endl;
        return false;
    }
    void gen(const string &op,const string &t1,const string &t2,const string &res){
        CodeLine t;
        t.op=op;t.t1=t1;t.t2=t2;t.res=res;
        codes.push_back(t);
        labelId++;
    }
    string packArray(int n,const string &type){
        return "array("+to_string(n)+","+type+")";
    }
    pair<int,string> divideType(const string &type){
        int num=0;
        string dType="";
        bool flag = false;
        for(char t:type){
            if(!flag){
                if(t>='0'&&t<='9'){
                    num=num*10+(t-'0');
                }else if(t==','){
                    flag = true;
                }
            }else{
                if(t==')') break;
                dType += t;
            }
        }
        return make_pair(num,dType);
    }
    int getTypeWidth(const string &type){
        if(type[0]=='i') return 4;
        if(type[0]=='c') return 1;
        if(type[0]=='f') return 4;
        if(type[0]=='d') return 8;
        if(type[0]=='a'){
            auto t = divideType(type);
            return t.first*getTypeWidth(t.second);
        }
        if(type[0]=='s'){
            int num = 0;
            for(char t:type){
                if(t>='0'&&t<='9') num = num*10 + (t-'0');
            }
            return num;
        }
        cout<<"Get width of "<<type<<" ERROR"<<endl;
        return -1;
    }
    CodeLine packCode(const string &op,const string &t1,const string &t2,const string &res){
        CodeLine codeLine;
        codeLine.op=op;codeLine.res=res;codeLine.t1=t1;codeLine.t2=t2;
        return codeLine;
    }
    struct SDTNode {
        vector<SDTNode *> sons;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if(treeNode.sons.empty()) return;
        }
        virtual ~SDTNode(){
            for(auto &t:sons){
                delete t;
            }
        }
    };
    struct Elist : SDTNode{
        string code="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Block : SDTNode{
        int label=0;
        string code="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Decls : SDTNode{
        int width=0;
        string code="",prefix="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Stmts : SDTNode{
        string code;
        int gotoNext=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    void commonCal(TreeNode &treeNode,SDTNode *fa,SDTNode *now,string &code){
        Elist *elist_1 = new Elist;
        now->sons.push_back(elist_1);
        elist_1->dfs(resultTree[treeNode.sons[3]],now);
        if(!funcs.count(TNAME(1))){
            cout<<"ERROR"<<endl;
        }
        int n = 0;
        for(auto &t:q){
            n++;
            gen("param","_","_",t);
        }
        string p = to_string(funcs[TNAME(1)]);
        gen("call",to_string(n),"_",TNAME(1));
    }
    struct TypeP :SDTNode{
        int width=0;
        int num=0;
        string type="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) return;
            TypeP *typeP_1 = new TypeP;
            sons.push_back(typeP_1);
            num = atoi((TNAME(1)).c_str());
            typeP_1->type = type;
            typeP_1->dfs(SON(3));
            type = packArray(num,typeP_1->type);
        }
    };
    struct DeclsP : SDTNode{
        int width=0;
        string code="",prefix="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) return;

            if(treeNode.opt==0){
                TypeP *typeP = new TypeP;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(typeP);
                sons.push_back(declsP_1);
                typeP->type = "int";
                typeP->dfs(SON(1));
                if(vars.count(prefix+TNAME(2))){
                    cout<<"ERROR"<<endl;
                }
                if(typeP->num <= 0){
                    width = getTypeWidth("int");
                    enter(prefix+TNAME(2),"int",offset,width);
                }else{
                    width = getTypeWidth(typeP->type);
                    enter(prefix+TNAME(2),typeP->type,offset,width);
                }
                offset += width;
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(4));
                width += declsP_1->width;
            }else if(treeNode.opt==1){
                TypeP *typeP = new TypeP;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(typeP);
                sons.push_back(declsP_1);
                typeP->type = "char";
                typeP->dfs(SON(1));
                if(vars.count(prefix+TNAME(2))){
                    cout<<"ERROR"<<endl;
                }
                if(typeP->num <= 0){
                    width = getTypeWidth("char");
                    enter(prefix+TNAME(2),"char",offset,width);

                }else{
                    width = getTypeWidth(typeP->type);
                    enter(prefix+TNAME(2),typeP->type,offset,width);
                }
                offset += width;
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(4));
                width += declsP_1->width;
            }else if(treeNode.opt==2){
                TypeP *typeP = new TypeP;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(typeP);
                sons.push_back(declsP_1);
                typeP->type = "double";
                typeP->dfs(SON(1));
                if(vars.count(prefix+TNAME(2))){
                    cout<<"ERROR"<<endl;
                }
                if(typeP->num <= 0){
                    width = getTypeWidth("double");
                    enter(prefix+TNAME(2),"double",offset,width);
                }else{
                    width = getTypeWidth(typeP->type);
                    enter(prefix+TNAME(2),typeP->type,offset,width);
                }
                offset += width;
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(4));
                width += declsP_1->width;
            }else if(treeNode.opt==3){
                TypeP *typeP = new TypeP;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(typeP);
                sons.push_back(declsP_1);
                typeP->type = "float";
                typeP->dfs(SON(1));
                if(vars.count(prefix+TNAME(2))){
                    cout<<"ERROR"<<endl;
                }
                if(typeP->num <= 0){
                    width = getTypeWidth("float");
                    enter(prefix+TNAME(2),"float",offset,width);
                }else{
                    width = getTypeWidth(typeP->type);
                    enter(prefix+TNAME(2),typeP->type,offset,width);
                }
                offset += width;
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(4));
                width += declsP_1->width;
            }else if(treeNode.opt==4){
                Block *block_1 = new Block;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(block_1);
                sons.push_back(declsP_1);
                enterFUNC(prefix+TNAME(1),labelId);
                block_1->dfs(SON(4));
                gen("ret","_","_","_");
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(5));
                width += declsP_1->width;
            }else if(treeNode.opt==5){
                Decls *decls_1 = new Decls;
                DeclsP *declsP_1 = new DeclsP;
                sons.push_back(decls_1);
                sons.push_back(declsP_1);
                int offsetTemp = offset;
                decls_1->prefix=prefix+TNAME(1)+".";
                decls_1->dfs(SON(3));
                if(vars.count(prefix+TNAME(1))){
                    cout<<"ERROR"<<endl;
                }
                enter(TNAME(1),"struct("+to_string(decls_1->width)+")",offsetTemp,decls_1->width);
                declsP_1->prefix = prefix;
                declsP_1->dfs(SON(5));
                width += declsP_1->width;
            }
        }
    };
    struct LocP : SDTNode{
        string type;
        string offset="";
        int width=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Bool : SDTNode{
        string code="",addr="";
        int gotoTrue=0,gotoFalse=0,andLast=0,orLast=0;
        vector<int> *list;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Factor : SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            if(treeNode.opt==0){
                Bool *bool_1 = new Bool;
                sons.push_back(bool_1);
                bool_1->dfs(SON(1));
                addr = bool_1->addr;
            }else if(treeNode.opt==1){
                LocP *locP_1 = new LocP;
                if(!checkvar(TNAME(0)))
                    return;
                locP_1->type = vars[TNAME(0)].first;
                locP_1->width = varwidth[TNAME(0)];
                locP_1->dfs(SON(1));
                if(!locP_1->offset.empty())addr = TNAME(0)+"["+locP_1->offset+"]";
                else addr = TNAME(0);
            }else if(treeNode.opt==2){
                addr = TNAME(0);
            }else if(treeNode.opt==3){
                addr = "1";
            }else if(treeNode.opt==4){
                addr = "0";
            }
        }
    };
    struct Unary : SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            if(treeNode.opt<=1){
                Unary *unary_1 = new Unary;
                sons.push_back(unary_1);
                unary_1->dfs(SON(1));
                addr = newtemp();
                if(treeNode.opt==0) gen("not",unary_1->addr,"_",addr);
                else gen("neg",unary_1->addr,"_",addr);
            }else if(treeNode.opt==2){
                Factor *factor_1 = new Factor;
                sons.push_back(factor_1);
                factor_1->dfs(SON(0));
                addr = factor_1->addr;
            }
        }
    };
    struct TermP : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()){
                addr = laddr;
                return;
            }
            Unary *unary_1 = new Unary;
            TermP *termP_1 = new TermP;
            sons.push_back(unary_1);
            sons.push_back(termP_1);
            unary_1->dfs(SON(1));
            addr = newtemp();
            if(treeNode.opt==0)gen("*",laddr,unary_1->addr,addr);
            else if(treeNode.opt==1)gen("/",laddr,unary_1->addr,addr);
            termP_1->laddr = addr;
            termP_1->dfs(SON(2));
            addr = termP_1->addr;
        }
    };
    struct Term : SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty())return;
            Unary *unary_1 = new Unary;
            TermP *termP_1 = new TermP;
            sons.push_back(unary_1);
            sons.push_back(termP_1);
            unary_1->dfs(SON(0));
            termP_1->laddr = unary_1->addr;
            termP_1->dfs(SON(1));
            addr=termP_1->addr;
        }
    };
    struct ExprP : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()){
                addr = laddr;
                return;
            }
            Term *term_1 = new Term;
            ExprP *exprP_1 = new ExprP;
            sons.push_back(term_1);
            sons.push_back(exprP_1);
            term_1->dfs(SON(1));
            addr = newtemp();
            if(treeNode.opt==0)gen("+",laddr,term_1->addr,addr);
            else if(treeNode.opt==1)gen("-",laddr,term_1->addr,addr);
            exprP_1->laddr = addr;
            exprP_1->dfs(SON(2));
            addr = exprP_1->addr;
        }
    };
    struct Expr : SDTNode{
        string code="",addr="";
        int gotoTrue=0,gotoFalse=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty())return;
            Term *term_1 = new Term;
            ExprP *exprP_1 = new ExprP;
            sons.push_back(term_1);
            sons.push_back(exprP_1);
            term_1->dfs(SON(0));
            exprP_1->laddr = term_1->addr;
            exprP_1->dfs(SON(1));
            addr=exprP_1->addr;
        }
    };
    struct Rel2 : SDTNode {
        string code = "", addr = "", laddr = "";
        int gotoTrue = 0, gotoFalse = 0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            Expr *expr_1 = new Expr;
            sons.push_back(expr_1);
            expr_1->dfs(SON(1));
            addr = newtemp();
            if(inEval){
                if(treeNode.opt==0)gen("<",laddr,expr_1->addr,addr);
                else if(treeNode.opt==1)gen("<=",laddr,expr_1->addr,addr);
                else if(treeNode.opt==2)gen(">",laddr,expr_1->addr,addr);
            }else{
                if(treeNode.opt==0){
                    codeCal=packCode("<",laddr,expr_1->addr,addr);
                    codeTrue = packCode("j<",laddr,expr_1->addr,to_string(gotoTrue));
                }
                else if(treeNode.opt==1){
                    codeCal=packCode("<=",laddr,expr_1->addr,addr);
                    codeTrue = packCode("j<=",laddr,expr_1->addr,to_string(gotoTrue));
                }
                else if(treeNode.opt==2){
                    codeCal=packCode(">",laddr,expr_1->addr,addr);
                    codeTrue = packCode("j>",laddr,expr_1->addr,to_string(gotoTrue));
                }
                codeFalse = packCode("j","_","_",to_string(gotoFalse));
            }
        }
    };
    struct Rel3 : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            if(treeNode.opt==0){
                Rel2 *rel2 = new Rel2;
                sons.push_back(rel2);
                rel2->gotoTrue=gotoTrue;
                rel2->gotoFalse=gotoFalse;
                rel2->laddr = laddr;
                rel2->dfs(SON(0));
                addr = rel2->addr;
                codeTrue = rel2->codeTrue;
                codeFalse = rel2->codeFalse;
                codeCal = rel2->codeCal;
            }else if(treeNode.opt==1){
                Expr *expr_1 = new Expr;
                sons.push_back(expr_1);
                expr_1->dfs(SON(1));
                addr = newtemp();
                if(inEval) {
                    gen(">=", laddr, expr_1->addr, addr);
                }else{
                    codeCal = packCode(">=", laddr, expr_1->addr, addr);
                    codeTrue = packCode("j>=",laddr,expr_1->addr,to_string(gotoTrue));
                    codeFalse = packCode("j","_","_",to_string(gotoFalse));
                }
            }
        }
    };
    struct Rel4 : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                addr = laddr;
                codeTrue = packCode("jnz",addr,"_",to_string(gotoTrue));
                codeFalse = packCode("j","_","_",to_string(gotoFalse));
                codeCal = packCode("","","","");
                return;
            }
            Rel3 *rel3 = new Rel3;
            sons.push_back(rel3);
            rel3->gotoTrue=gotoTrue;
            rel3->gotoFalse=gotoFalse;
            rel3->laddr = laddr;
            rel3->dfs(SON(0));
            addr = rel3->addr;
            codeTrue = rel3->codeTrue;
            codeFalse = rel3->codeFalse;
            codeCal = rel3->codeCal;
        }
    };
    struct Rel: SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            Expr *expr_1 = new Expr;
            Rel4 *rel4_1 = new Rel4;
            sons.push_back(expr_1);
            sons.push_back(rel4_1);
            expr_1->gotoTrue=gotoTrue;
            expr_1->gotoFalse=gotoFalse;
            rel4_1->gotoTrue=gotoTrue;
            rel4_1->gotoFalse=gotoFalse;
            expr_1->dfs(SON(0));
            rel4_1->laddr = expr_1->addr;
            rel4_1->dfs(SON(1));
            addr=rel4_1->addr;
            codeTrue = rel4_1->codeTrue;
            codeFalse = rel4_1->codeFalse;
            codeCal = rel4_1->codeCal;
        }
    };
    struct EqualityP : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if(!inEval){
                if(treeNode.sons.empty()){
                    codes.push_back(codeTrue);
                    labelId++;
                    codes.push_back(codeFalse);
                    labelId++;
                }else{
                    if(!codeCal.op.empty()){
                        codes.push_back(codeCal);
                        labelId++;
                    }
                }
            }
            if (treeNode.sons.empty()) {
                addr = laddr;
                return;
            }
            Rel *rel_1 = new Rel;
            EqualityP *equalityP_1 = new EqualityP;
            sons.push_back(rel_1);
            sons.push_back(equalityP_1);
            rel_1->gotoTrue = gotoTrue;
            rel_1->gotoFalse = gotoFalse;
            equalityP_1->gotoTrue = gotoTrue;
            equalityP_1->gotoFalse = gotoFalse;
            rel_1->dfs(SON(1));
            addr = newtemp();
            if(inEval) {
                if (treeNode.opt == 0) {
                    gen("==", laddr, rel_1->addr, addr);
                } else gen("!=", laddr, rel_1->addr, addr);
            }else {
                //equalityP_1->codeTrue = packCode("je", laddr, rel_1->addr, to_string(gotoTrue));
                equalityP_1->codeFalse = packCode("j", "_", "_", to_string(gotoFalse));
                if (treeNode.opt == 0) {
                    equalityP_1->codeTrue = packCode("je", laddr, rel_1->addr, to_string(gotoTrue));
                    equalityP_1->codeCal=packCode("==", laddr, rel_1->addr, addr);
                } else {
                    equalityP_1->codeTrue = packCode("jne", laddr, rel_1->addr, to_string(gotoTrue));
                    equalityP_1->codeCal=packCode("!=", laddr, rel_1->addr, addr);
                }
            }
            equalityP_1->laddr = addr;
            equalityP_1->dfs(SON(2));
            addr = equalityP_1->addr;
        }
    };
    struct Equality : SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0;
        CodeLine codeTrue,codeFalse,codeCal;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) {
                return;
            }
            Rel *rel_1 = new Rel;
            EqualityP *equalityP_1 = new EqualityP;
            sons.push_back(rel_1);
            sons.push_back(equalityP_1);
            rel_1->gotoTrue = gotoTrue;
            rel_1->gotoFalse = gotoFalse;
            rel_1->dfs(SON(0));
            equalityP_1->laddr = rel_1->addr;
            equalityP_1->codeTrue = rel_1->codeTrue;
            equalityP_1->codeFalse = rel_1->codeFalse;
            equalityP_1->codeCal = rel_1->codeCal;
            equalityP_1->gotoTrue = gotoTrue;
            equalityP_1->gotoFalse = gotoFalse;
            equalityP_1->dfs(SON(1));
            addr=equalityP_1->addr;
        }
    };
    struct JoinP : SDTNode{
        string code="",addr="",laddr="";
        int gotoTrue=0,gotoFalse=0,gotoBegin=0,andLast=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()){
                addr = laddr;
                return;
            }
            writeLabel(andLast);
            andLast = newLabel();
            Equality *equality_1 = new Equality;
            JoinP *joinP_1 = new JoinP;
            sons.push_back(equality_1);
            sons.push_back(joinP_1);
            equality_1->gotoTrue = andLast;
            equality_1->gotoFalse = gotoFalse;
            equality_1->dfs(SON(1));
            addr = newtemp();
            if(inEval)gen("AND",laddr,equality_1->addr,addr);
            joinP_1->laddr = addr;
            joinP_1->andLast = andLast;
            joinP_1->gotoFalse = gotoFalse;
            joinP_1->dfs(SON(2));
            addr = joinP_1->addr;
            andLast = joinP_1->andLast;
        }
    };
    struct Join : SDTNode{
        string code="",addr="",op="";
        int gotoTrue=0,gotoFalse=0,gotoNext=0,andLast=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty())return;
            Equality *equality_1 = new Equality;
            JoinP *joinP_1 = new JoinP;
            sons.push_back(equality_1);
            sons.push_back(joinP_1);
            andLast = newLabel();
            gotoTrue = andLast;
            equality_1->gotoTrue = andLast;
            equality_1->gotoFalse = gotoFalse;
            equality_1->dfs(SON(0));
            joinP_1->laddr = equality_1->addr;
            joinP_1->andLast = andLast;
            joinP_1->gotoFalse = gotoFalse;
            joinP_1->dfs(SON(1));
            addr=joinP_1->addr;
            andLast = joinP_1->andLast;
        }
    };
    struct BoolP : SDTNode{
        string laddr="",addr="",op="";
        int gotoTrue=0,gotoFalse=0,andLast=0,orLast=0;
        vector<int> *list;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()){
                addr = laddr;
                return;
            }
            writeLabel(orLast);
            orLast = newLabel();
            gotoFalse = orLast;
            Join *join_1 = new Join;
            BoolP *boolP_1 = new BoolP;
            sons.push_back(join_1);
            sons.push_back(boolP_1);
            join_1->gotoFalse = orLast;
            join_1->dfs(SON(1));
            list->push_back(join_1->andLast);
            addr = newtemp();
            if(inEval)gen("OR",laddr,join_1->addr,addr);
            boolP_1->laddr = addr;
            boolP_1->orLast = orLast;
            boolP_1->list = list;
            boolP_1->gotoTrue = gotoTrue;
            boolP_1->dfs(SON(2));
            addr = boolP_1->addr;
            orLast = boolP_1->orLast;
        }
    };
    void LocP::dfs(TreeNode &treeNode,SDTNode *fa) {
        if (treeNode.sons.empty())return;
        Bool *bool_1 = new Bool;
        LocP *locP = new LocP;
        sons.push_back(bool_1);
        sons.push_back(locP);
        inEval=true;
        bool_1->dfs(SON(1));
        if(type[0]=='a'){
            locP->type = divideType(type).second;
            locP->width = getTypeWidth(locP->type);
        }
        locP->dfs(SON(3));
        offset = newtemp();
        gen("*",bool_1->addr,to_string(locP->width),offset);
        if(!locP->offset.empty()){
            gen("+",offset,locP->offset,offset);
        }
    }
    void Bool::dfs(TreeNode &treeNode,SDTNode *fa) {
        if (treeNode.sons.empty())return;
        Join *join_1 = new Join;
        BoolP *boolP_1 = new BoolP;
        sons.push_back(join_1);
        sons.push_back(boolP_1);
        list = new vector<int>;
        orLast = newLabel();
        gotoFalse = orLast;
        join_1->gotoFalse = orLast;
        join_1->dfs(SON(0));
        list->push_back(join_1->andLast);
        boolP_1->orLast = orLast;
        boolP_1->list = list;
        boolP_1->gotoTrue = gotoTrue;
        boolP_1->laddr = join_1->addr;
        boolP_1->dfs(SON(1));
        addr = boolP_1->addr;
        orLast = boolP_1->orLast;
    }
    struct Loc : SDTNode{
        pair<string,int> array;
        string addr;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) return;
            LocP *locP_1 = new LocP;
            if(!checkvar(TNAME(0)))
                return;
            locP_1->type = vars[TNAME(0)].first;
            locP_1->width = varwidth[TNAME(0)];
            inEval=true;
            locP_1->dfs(SON(1));
            if(!locP_1->offset.empty())addr=TNAME(0)+"["+locP_1->offset+"]";
            else addr = TNAME(0);

        }
    };
    struct Stmt1 : SDTNode{
        int gotoNext=0,gotoBegin=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Stmt : SDTNode{
        string code="";
        int gotoNext=0,gotoBegin=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct Noelse : SDTNode{
        string code="";
        int gotoNext=0,gotoBegin=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()) return;
            if(treeNode.opt==0){
                commonCal(treeNode,fa,this,code);
            }else if(treeNode.opt==1){
                Loc *loc1 = new Loc;
                Bool *bool1 = new Bool;
                sons.push_back(loc1);
                sons.push_back(bool1);
                loc1->dfs(SON(0));
                bool1->dfs(SON(2));
                gen("=",bool1->addr,"_",loc1->addr);
            }else if(treeNode.opt==2){
                Bool *bool_1 = new Bool;
                Noelse *noelse_1 = new Noelse;
                sons.push_back(bool_1);
                sons.push_back(noelse_1);
                inEval = false;
                bool_1->gotoTrue = newLabel();
                bool_1->dfs(SON(2));
                writeLabel(bool_1->gotoTrue);
                for(auto t:(*bool_1->list)){
                    writeLabel(t);
                }
                delete bool_1->list;
                noelse_1->gotoNext = gotoNext;
                noelse_1->gotoBegin = newLabel();
                noelse_1->dfs(SON(4));
                gen("j","_","_",to_string(noelse_1->gotoBegin));
                writeLabel(bool_1->orLast);
            }else if(treeNode.opt==3){
                Bool *bool_1 = new Bool;
                Noelse *noelse_1 = new Noelse;
                sons.push_back(bool_1);
                sons.push_back(noelse_1);
                gotoBegin = newLabel();
                writeLabel(gotoBegin);
                inEval=false;
                bool_1->gotoTrue = newLabel();
                bool_1->dfs(SON(2));
                writeLabel(bool_1->gotoTrue);
                for(auto t:(*bool_1->list)){
                    writeLabel(t);
                }
                noelse_1->gotoNext = newLabel();
                noelse_1->dfs(SON(4));
                gen("j","_","_",to_string(gotoBegin));
                writeLabel(noelse_1->gotoNext);
                writeLabel(bool_1->orLast);
            }else if(treeNode.opt==4){
                Stmt *stmt_1 = new Stmt;
                Bool *bool_1 = new Bool;
                sons.push_back(stmt_1);
                sons.push_back(bool_1);
                gotoBegin = newLabel();
                writeLabel(gotoBegin);
                bool_1->gotoTrue = newLabel();
                writeLabel(bool_1->gotoTrue);
                stmt_1->dfs(SON(1));
                stmt_1->gotoNext = newLabel();
                inEval=false;
                bool_1->dfs(SON(4));
                for(auto t:(*bool_1->list)){
                    tempLabel[-t] = tempLabel[-gotoBegin];
                }
                writeLabel(stmt_1->gotoNext);
                writeLabel(bool_1->orLast);
            }else if(treeNode.opt==5){
                gen("j","_","_",to_string(gotoNext));
            }else if(treeNode.opt==6){
                Decls *decls = new Decls;
                Stmts *stmts = new Stmts;
                sons.push_back(decls);
                sons.push_back(stmts);
                stmts->gotoNext = gotoNext;
                decls->dfs(SON(1));
                stmts->dfs(SON(2));
            }
        }
    };
    void Stmt1::dfs(TreeNode &treeNode, SDT::SDTNode *fa) {
        if (treeNode.sons.empty()){
            writeLabel(gotoBegin);
            return;
        }
        Noelse *noelse = new Noelse;
        sons.push_back(noelse);
        writeLabel(gotoBegin);
        noelse->gotoNext = gotoNext;
        noelse->dfs(SON(1));
    }
    void Stmt::dfs(TreeNode &treeNode, SDT::SDTNode *fa) {
        if (treeNode.sons.empty()) return;
        if(treeNode.opt==0){
            commonCal(treeNode,fa,this,code);
        }else if(treeNode.opt==1){
            Loc *loc1 = new Loc;
            Bool *bool1 = new Bool;
            sons.push_back(loc1);
            sons.push_back(bool1);
            loc1->dfs(SON(0));
            bool1->dfs(SON(2));
            gen("=",bool1->addr,"_",loc1->addr);
        }else if(treeNode.opt==2){
            Bool *bool_1 = new Bool;
            Noelse *noelse_1 = new Noelse;
            Stmt1 *stmts1_1 = new Stmt1;
            sons.push_back(bool_1);
            sons.push_back(noelse_1);
            sons.push_back(stmts1_1);
            inEval = false;
            bool_1->gotoTrue = newLabel();
            bool_1->dfs(SON(2));
            writeLabel(bool_1->gotoTrue);
            for(auto t:(*bool_1->list)){
                writeLabel(t);
            }
            delete bool_1->list;
            noelse_1->gotoNext = gotoNext;
            noelse_1->gotoBegin = newLabel();
            noelse_1->dfs(SON(4));
            gen("j","_","_",to_string(noelse_1->gotoBegin));
            writeLabel(bool_1->orLast);
            stmts1_1->gotoNext = gotoNext;
            stmts1_1->gotoBegin = noelse_1->gotoBegin;
            stmts1_1->dfs(SON(5));
        }else if(treeNode.opt==3){
            Bool *bool_1 = new Bool;
            Stmt *stmt = new Stmt;
            sons.push_back(bool_1);
            sons.push_back(stmt);
            gotoBegin = newLabel();
            writeLabel(gotoBegin);
            inEval=false;
            bool_1->gotoTrue = newLabel();
            bool_1->dfs(SON(2));
            writeLabel(bool_1->gotoTrue);
            for(auto t:(*bool_1->list)){
                writeLabel(t);
            }
            stmt->gotoNext = newLabel();
            stmt->dfs(SON(4));
            gen("j","_","_",to_string(gotoBegin));
            writeLabel(stmt->gotoNext);
            writeLabel(bool_1->orLast);
        }else if(treeNode.opt==4){
            Stmt *stmt_1 = new Stmt;
            Bool *bool_1 = new Bool;
            sons.push_back(stmt_1);
            sons.push_back(bool_1);
            gotoBegin = newLabel();
            writeLabel(gotoBegin);
            bool_1->gotoTrue = newLabel();
            writeLabel(bool_1->gotoTrue);
            stmt_1->dfs(SON(1));
            stmt_1->gotoNext = newLabel();
            inEval=false;
            bool_1->dfs(SON(4));
            for(auto t:(*bool_1->list)){
                tempLabel[-t] = tempLabel[-gotoBegin];
            }
            writeLabel(stmt_1->gotoNext);
            writeLabel(bool_1->orLast);
        }else if(treeNode.opt==5){
            gen("j","_","_",to_string(gotoNext));
        }else if(treeNode.opt==6){
            Decls *decls = new Decls;
            Stmts *stmts = new Stmts;
            sons.push_back(decls);
            sons.push_back(stmts);
            stmts->gotoNext = gotoNext;
            decls->dfs(SON(1));
            stmts->dfs(SON(2));
        }
    }
    struct StmtsP5 : SDTNode{
        int gotoNext=0,gotoBegin=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa);
    };
    struct StmtsP : SDTNode{
        string code="";
        int gotoBegin=0,gotoNext=0;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa){
            if(treeNode.sons.empty()) return;
            if(treeNode.opt==0){
                commonCal(treeNode,fa,this,code);
                StmtsP *stmtsP_1 = new StmtsP;
                sons.push_back(stmtsP_1);
                stmtsP_1->dfs(SON(6));
            }
            if(treeNode.opt==1){
                LocP *locP_1 = new LocP;
                Bool *bool_1 = new Bool;
                StmtsP *stmtsP = new StmtsP;
                if(!checkvar(TNAME(0)))
                    return;
                locP_1->type = vars[TNAME(0)].first;
                locP_1->width = varwidth[TNAME(0)];
                locP_1->dfs(SON(1));
                inEval=true;
                bool_1->dfs(SON(3));
                if(!locP_1->offset.empty())gen("=",bool_1->addr,"_",TNAME(0)+"["+locP_1->offset+"]");
                else gen("=",bool_1->addr,"_",TNAME(0));
                stmtsP->dfs(SON(5));
            }else if(treeNode.opt==2){
                Bool *bool_1 = new Bool;
                Noelse *noelse_1 = new Noelse;
                StmtsP5 *stmtsP5_1 = new StmtsP5;
                sons.push_back(bool_1);
                sons.push_back(noelse_1);
                sons.push_back(stmtsP5_1);
                inEval = false;
                bool_1->gotoTrue = newLabel();
                bool_1->dfs(SON(2));
                writeLabel(bool_1->gotoTrue);
                for(auto t:(*bool_1->list)){
                    writeLabel(t);
                }
                delete bool_1->list;
                noelse_1->gotoNext = gotoNext;
                noelse_1->dfs(SON(4));
                stmtsP5_1->gotoBegin = newLabel();
                gen("j","_","_",to_string(stmtsP5_1->gotoBegin));
                writeLabel(bool_1->orLast);
                stmtsP5_1->gotoNext = gotoNext;
                stmtsP5_1->dfs(SON(5));
            }else if(treeNode.opt==3){
                Bool *bool_1 = new Bool;
                Stmt *stmt = new Stmt;
                StmtsP *stmtsP = new StmtsP;
                sons.push_back(bool_1);
                sons.push_back(stmt);
                sons.push_back(stmtsP);
                gotoBegin = newLabel();
                writeLabel(gotoBegin);
                inEval=false;
                bool_1->gotoTrue = newLabel();
                bool_1->dfs(SON(2));
                writeLabel(bool_1->gotoTrue);
                for(auto t:(*bool_1->list)){
                    writeLabel(t);
                }
                stmt->gotoNext = newLabel();
                stmt->dfs(SON(4));
                gen("j","_","_",to_string(gotoBegin));
                writeLabel(stmt->gotoNext);
                writeLabel(bool_1->orLast);
                stmtsP->dfs(SON(5));
            }else if(treeNode.opt==4){
                Stmt *stmt_1 = new Stmt;
                Bool *bool_1 = new Bool;
                StmtsP *stmtsP = new StmtsP;
                sons.push_back(stmt_1);
                sons.push_back(bool_1);
                sons.push_back(stmtsP);
                gotoBegin = newLabel();
                writeLabel(gotoBegin);
                bool_1->gotoTrue = newLabel();
                writeLabel(bool_1->gotoTrue);
                stmt_1->dfs(SON(1));
                stmt_1->gotoNext = newLabel();
                inEval=false;
                bool_1->dfs(SON(4));
                for(auto t:(*bool_1->list)){
                    tempLabel[-t] = tempLabel[-gotoBegin];
                }
                writeLabel(stmt_1->gotoNext);
                writeLabel(bool_1->orLast);
                stmtsP->dfs(SON(7));
            }else if(treeNode.opt==5){
                Stmts *stmts = new Stmts;
                sons.push_back(stmts);
                gen("j","_","_",to_string(gotoNext));
                stmts->dfs(SON(0));
            }else if(treeNode.opt==6){
                Decls *decls = new Decls;
                Stmts *stmts = new Stmts;
                StmtsP *stmtsP = new StmtsP;
                sons.push_back(decls);
                sons.push_back(stmts);
                stmts->gotoNext = gotoNext;
                stmtsP->gotoNext = gotoNext;
                decls->dfs(SON(1));
                stmts->dfs(SON(2));
                stmtsP->dfs(SON(4));
            }
        }
    };
    void StmtsP5::dfs(TreeNode &treeNode, SDT::SDTNode *fa){
        if (treeNode.sons.empty()) return;
        if(treeNode.opt==0){
            StmtsP *stmtsP = new StmtsP;
            sons.push_back(stmtsP);
            writeLabel(gotoBegin);
            stmtsP->gotoNext = gotoNext;
            stmtsP->dfs(SON(0));
        }else if(treeNode.opt==1){
            Noelse *noelse_1 = new Noelse;
            StmtsP *stmtsP_1 = new StmtsP;
            sons.push_back(noelse_1);
            sons.push_back(stmtsP_1);
            noelse_1->gotoNext = gotoNext;
            noelse_1->dfs(SON(1));
            writeLabel(gotoBegin);
            stmtsP_1->gotoNext = gotoNext;
            stmtsP_1->dfs(SON(2));

        }
    }
    void Decls::dfs(TreeNode &treeNode, SDT::SDTNode *fa) {

        if(treeNode.sons.empty()) return;
        DeclsP *declsP_1 = new DeclsP;
        sons.push_back(declsP_1);
        declsP_1->prefix = prefix;
        declsP_1->dfs(SON(0));
        width=declsP_1->width;
    }
    void Stmts::dfs(TreeNode &treeNode, SDT::SDTNode *fa) {
        if(treeNode.sons.empty()) return;
        StmtsP *stmtsP_1 = new StmtsP;
        sons.push_back(stmtsP_1);
        stmtsP_1->dfs(SON(0));
    }
    struct Program : SDTNode{
        virtual void dfs(TreeNode &treeNode,SDTNode *fa){
            if(treeNode.sons.empty()) return;
            offset=offsetFUNC=0;
            Decls *decls_1 = new Decls;
            Stmts *stmts_1 = new Stmts;
            sons.push_back(decls_1);
            sons.push_back(stmts_1);
            decls_1->dfs(SON(0));
            stmts_1->dfs(SON(1));
        }
    };
    void Block::dfs(TreeNode &treeNode, SDT::SDTNode *fa){
        if(treeNode.sons.empty()) return;
        Decls *decls = new Decls;
        Stmts *stmts = new Stmts;
        sons.push_back(decls);
        sons.push_back(stmts);
        decls->dfs(SON(1));
        stmts->dfs(SON(2));
    }
    struct Basic :SDTNode{
        string Type;
        int width;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            /*
            if (treeNode.sons.empty()) return;
            if(treeNode.opt==0){
                Type="int";
                width=4;
            }else if(treeNode.opt==1){
                Type="char";
                width=1;
            }else if(treeNode.opt==2){
                Type="double";
                width=8;
            }else if(treeNode.opt==3){
                Type="float";
                width=4;
            }
             */
            //TODO NOT USE BASIC
        }
    };
    struct Type : SDTNode{
        string Type;
        int width;
        virtual void dfs(TreeNode &treeNode,SDTNode *fa){
            if(treeNode.sons.empty()) return;
            /*Basic *basic_1 = new Basic;
            TypeP *typeP_1 = new TypeP;
            sons.push_back(basic_1);
            sons.push_back(typeP_1);
            basic_1->dfs(SON(0));
            typeP_1->dfs(SON(1));
            t = basic_1->Type;
            w = basic_1->width;*/
            //TODO NOT USE TYPE
        }
    };
    struct Def : SDTNode{
        int addr=0;
        string code;
        virtual void dfs(TreeNode &treeNode, SDTNode *fa){
            /*
            if(treeNode.sons.empty()) return;
            Block *block_1 = new Block;
            sons.push_back(block_1);
            block_1->dfs(SON(4));
            enterFUNC(TNAME(1),offsetFUNC);
            funcAddr[TNAME(1)] = block_1->label;
            funCode[TNAME(1)] = block_1->code;
            offsetFUNC += temp;
             */
            //TODO NOT USE DEF
        }
    };
    struct Defrec : SDTNode{
        virtual void dfs(TreeNode &treeNode, SDTNode *fa){
            /*
            if(treeNode.sons.empty()) return;
            Decls *decls = new Decls;
            sons.push_back(decls);
            decls->dfs(SON(3));
            if(vars.count(TNAME(1))){
                cout<<"ERROR"<<endl;
            }
            enter(TNAME(1),"struct("+to_string(decls->width)+")",offset,decls->width);
             */
            //TODO NOT USE DEFREC
        }
    };
    struct Decl : SDTNode{
        virtual void dfs(TreeNode &treeNode,SDTNode *fa){
            /*
            if(treeNode.sons.empty()) return;
            if(treeNode.opt==0){
                Type *type_1 = new Type;
                sons.push_back(type_1);
                type_1->dfs(SON(0));
                if(vars.count(TNAME(1))){
                    cout<<"ERROR"<<endl;
                }
                enter(TNAME(1),type_1->Type,offset,type_1->width);
                offset += type_1->width;
            }else if(treeNode.opt==1){
                Def *def = new Def;
                sons.push_back(def);
                def->dfs(SON(0));
            }else if(treeNode.opt==2){
                Defrec *defrec = new Defrec;
                sons.push_back(defrec);
                defrec->dfs(SON(0));
            }
             */
            //TODO NOT USE DECL
        }
    };
    struct ElistP : SDTNode{
        string code="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            if (treeNode.sons.empty()){
                return;
            }
            sons.push_back(new Bool);
            sons.push_back(new ElistP);
            sons[0]->dfs(SON(1));
            q.push_back(((Bool*)sons[0])->addr);
            sons[1]->dfs(SON(2));
        }
    };
    void Elist::dfs(TreeNode &treeNode,SDTNode *fa) {
        if (treeNode.sons.empty()) return;
        q.clear();
        if(treeNode.opt==0){
            sons.push_back(new Bool);
            sons.push_back(new ElistP);
            sons[0]->dfs(SON(0));
            q.push_back(((Bool*)sons[0])->addr);
            sons[1]->dfs(SON(1));
        }else if(treeNode.opt==1){
            sons.push_back(new ElistP);
            sons[0]->dfs(SON(0));
        }
    }
    struct Cal : SDTNode{
        string code="";
        virtual void dfs(TreeNode &treeNode,SDTNode *fa) {
            /*
            commonCal(treeNode,fa,this,code);
             */
            //TODO NOT USE CAL
        }
    };
}

#endif //COMPILELAB_SDTFUNC_H
