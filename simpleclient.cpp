#include "netclient.h"

class CustomClient : public Client
{};
int main() {
    CustomClient c;
    c.Connect("127.0.0.1", 60000);
    message m;
    m.msg = "Hello";
	m.type = MessageType::MessageAll;
    c.Send(m);
    c.listen();
    while (1) {
        int b = 0;
    }
    return 0;
}