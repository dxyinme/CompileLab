
reend.txt 是关键字和终结符集合

XXXXXDFA.txt 命名规范

dfa撰写规范，请保证读入的是一个dfa


```
n m                            #n是状态数，m是转移数，状态编号从0开始
start                          #start是起始状态编号
cnt end[1] end[2] ... end[cnt] #表示若干个终结状态的编号
#接下去m行 ， 每行3个元素 (from,to,tran) from , to 都是编号 , tran是接受字符的ascii码,
#其他终结符请到reend里面找对应的编号

```

win

```cmd
g++ main.cpp -o main --std=gnu++17
main
```

linux/mac

```sh
g++ main.cpp -o main --std=c++17
./main
```


语法分析

规则存放于generator.txt:

格式为xxxx -> yyyyy \$ zzzzz ​\$ #

$表示“或”，#表示行结尾，none表示空

经过转化后必须能表示成LL(1)文法，否则报错

program:程序体，开始符号，包含声明和语句两部分

block：与程序体类似，包裹在大括号内

decls：声明部分

decl：声明语句，包括变量声明和函数声明，注意变量声明时不能初始化

def：函数定义

cal：函数调用

basic：四种变量类型

type：复合类型，如int[2][2][2]这样

stmts：语句部分

stmt：语句，包括函数调用，赋值，判断，循环，break，代码块，注意if和else中不能嵌套else

noelse：和语句含义一样，但是不能用else

loc：左值，用于赋值语句

bool：可以解析为布尔值的表达式

join：不包含或的表达式

equality：不包含或、与的表达式

rel：不包括或、与、相等的表达式

expr：不包括或、与、相等、大于小于的表达式

term：不包括或、与、相等、大于小于、加减的表达式

unary：不包括或、与、相等、大于小于、加减乘除的表达式

factor：最小的表达式单元
