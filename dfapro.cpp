#include<iostream>
#include<cstdio>
#include<fstream>
#include<vector>
#define ll long long 
using namespace std;
const int endL = 282;
const int endR = 315;
string prefix = "txt/";
struct edge{
	int u,v,w;
};

//INTEGER
void IntegerDFA(){
	ofstream out;
	out.open(prefix+"IntegerDFA.txt");
	int n = 2;
	vector<edge> vec;
	for(int i=0;i<=9;i++){
		vec.push_back((edge){ 0 , 1 , i + '0'});
	}
	for(int i=0;i<=9;i++){
		vec.push_back((edge){ 1 , 1 , i + '0'});
	}
	out << n << " " << vec.size() << endl ;
	out << 0 << endl;
	out << 1 << " " << 1 << endl;
	for(int i = 0 ; i < vec.size(); i++){
		out << vec[i].u << " " << vec[i].v << " " << vec[i].w << endl;
	}
	out.close();
}


// VariableOrKeyword
void VariableOrKeywordDFA(){
	ofstream out ;
	out.open(prefix+"VariableOrKeywordDFA.txt");
	vector<edge> vec;
	int n = 2;
	for(int i=0;i<26;i++) {
		vec.push_back((edge){ 0 , 1 , (int)'a' + i});
		vec.push_back((edge){ 0 , 1 , (int)'A' + i});
	}
	vec.push_back((edge){0 , 1 , (int)'_'});
	for(int i=0;i<26;i++) {
		vec.push_back((edge){ 1 , 1 , (int)'A' + i});
		vec.push_back((edge){ 1 , 1 , (int)'a' + i});
	}
	for(int i=0;i<10;i++){
		vec.push_back((edge){ 1 , 1 , (int)'0' + i});
	}
	vec.push_back((edge){1 , 1 , (int)'_'});
	out << n << " " << vec.size() << endl ;
	out << 0 << endl;
	out << 1 << " " << 1 << endl;
	for(int i = 0 ; i < vec.size(); i++){
		out << vec[i].u << " " << vec[i].v << " " << vec[i].w << endl;
	}
	out.close();
}

// Notes
void NotesDFA(){
	ofstream out ; 
	out.open(prefix+"NotesDFA.txt");
	vector<edge> vec;
	int n = 5 ;
	vec.push_back((edge){ 0 , 1 , (int)'/'});
	vec.push_back((edge){ 1 , 2 , (int)'*'});
	for(int i=-128;i<256;i++){
		if(i == -1)continue;
		if((char)i == '*')continue;
		if((char)i == '/')continue;
		
		vec.push_back((edge){ 2 , 2 , i});
		vec.push_back((edge){ 3 , 2 , i});
		
	}
	vec.push_back((edge){2 , 3 , (int)'*'});
	vec.push_back((edge){3 , 4 , (int)'/'});
	vec.push_back((edge){3 , 3 , (int)'*'});
	out << n << " " << vec.size() << endl ;
	out << 0 << endl;
	out << 1 << " " << 4 << endl;
	for(int i = 0 ; i < vec.size() ; i++){
		out << vec[i].u <<" " << vec[i].v <<" "<<vec[i].w << endl;
	}
	out.close();
}

int main(){
	//char s[4]  = "孑";
	//cout << (int)s[0] <<" "<< (int)s[1] << endl; 
	IntegerDFA();
	VariableOrKeywordDFA();
	NotesDFA();
	return 0;
}