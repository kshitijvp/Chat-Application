#include "netserver.h"

int main() {

    Server s(60000);
    s.Start();
    while(1) {
        s.Update();
    }  
    return 0;
}