#include "queue.h"
#include <iostream>

using namespace std;


int main() {

    SrsFileQueue s("mwt");

    char teststr[11] = "mwtmwtmwt1";

    int pushnums = 0;
    int popnums = 0;
    for(int i = 0; i < 200; i++) {
        int j = random() % 2;
        if(j) {
            pushnums++;
            s.push(teststr,10);
        } else {
            if(!s.empty()) {
                popnums++;
                Fqueue::record r = s.pop();
                cout << "pop str: " << r.ptr.get() << "str.size(): " << r.size << endl;
            }         
        }
    }

    //最后把队列所有的数据出栈
    while(!s.empty()) {
        Fqueue::record r = s.pop();
        popnums++;
        cout << "pop str: " << r.ptr.get() << "str.size(): " << r.size << endl;        
    }
    std::cout << "push nums: " << pushnums << std::endl;
    std::cout << "pop nums: " << popnums << std::endl;
    //在这个文件队列析构的时候会销毁空闲池

}