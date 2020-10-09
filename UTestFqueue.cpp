#include "fqueue.h"
#include <string>
#include <iostream>
#include <cstring>


void test_0() {
    Fqueue fq("mwt.txt");
    fq.push("mwt1", 4);
    std::cout << "队列: records" << fq.records() << std::endl;
    Fqueue::record rec;
    rec = fq.pop();
    std::cout << "弹出: "<< rec.size << std::endl;
}

/*int main() {
    test_0();
}*/