# DataMaker-Qt5
ACM-ICPC数据生成器，提供了ui界面

基于DataMaker项目编写。

# 使用说明

在输入数据构造的输入框中，输入用于构造数据的函数

函数格式：

一个无返回值的函数，有一个int型参数num，用来确定当前生成的数据是第几组。

使用cout来生成数据。

下面是编写样例：

```cpp
void make(int index){ // index表示当前生成的数据编号
  // 输出使用cout
  // 比如本题是a+b，输入数据只有a和b （a, b <= 1e9）
  // 我需要一组数据，使得啊a = 1e9， b = 1e9，其他组数据随机
  int a = 0,b = 0;
  if(index == 1){// 第一组a和b都是1e9
    a = 1e9;
    b = 1e9;
  }else{//其他数据都随机生成
    a = rand() % (int)(1e9);
    b = rand() % (int)(1e9);
  }
  cout << a << ' ' << b << endl;
  // 样例编写完毕
}
```

在std标程输入框内，粘贴std标程，也可以选择本地的.cpp格式的文件读入。

std标程不需要进行重定向输入输出，只需要使用cin、cout进行读、写操作即可

下面是a + b读std程序样例：
 ```cpp
#include<iostream>
#include<stdlib.h>
using namespace std;

int main(int argc,char* argv[]){
    int a,b;
    cin >> a >> b;
    cout << a + b << endl;
    return 0;
}
 ```


生成数据文本框内，如果不输入值默认生成12组数据。

所有需要的内容输入后，按下生成数据按钮生成数据。
