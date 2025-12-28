#include "netserver.h"

int main() {

    Server s(60000);
    s.Start();
    while(1) {
        int b = 0;
    }  
    return 0;
}