#include "fqueue.h"
#include <string>
#include <iostream>
#include <cstring>

void test_0(char* fname) {
    File_IO f(fname);
    assert(f.is_open() == true);
    assert(f.size() == 0);
    char* ch1= "test_0";
    f.write(ch1, 6);
    assert(f.size() == 6);
    f.seek(1);
    char readstr[5];
    f.read(readstr, 5);
    assert(strcmp(readstr, "est_0") == 0);
    f.close();
    assert(true == f.remove_file());
}

/*
int main() {
    char testfilename[] = "mwt";
    test_0(testfilename);
    std::cout << "run all no error" << std::endl;
}
*/