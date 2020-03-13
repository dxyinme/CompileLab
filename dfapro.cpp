#include<bits/stdc++.h>
#define ll long long 
using namespace std;

const int endL = 282;
const int endR = 314;
struct edge{
	int u,v,w;
};

//INTEGER
void IntegerDFA(){
	ofstream out;
	out.open("IntegerDFA.txt");
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

void DoubleDFA(){
	
}
int main(){
	IntegerDFA();
	return 0;
}