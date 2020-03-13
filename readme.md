reend.txt 是关键字和终结符集合
dfa_IDN.txt 是变量的dfa
dfa撰写规范，请保证读入的是一个dfa
```
n m                            #n是状态数，m是转移数，状态编号从0开始
start                          #start是起始状态编号
cnt end[1] end[2] ... end[cnt] #表示若干个终结状态的编号
#接下去m行 ， 每行3个元素 (from,to,tran) from , to 都是编号 , tran是接受字符的ascii码,
#其他终结符请到reend里面找对应的编号

```