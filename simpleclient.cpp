#include "netclient.h"
#include "netcommon.h"
#include <thread>
#include <fstream>

class CustomClient : public Client
{};

void readInputLoop(std::string& input_string) {
    std::getline(std::cin, input_string);
}
void writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream file;
    file.open(filename, std::ios::app);
    file << '\n';
    file << content;
}
int main() {
    CustomClient c;
    std::string username;
    std::string passwd;
    std::cout << "Enter Username: " << std::endl;
    std::cin >> username;
    std::cout << "Enter Password: " << std::endl;
    std::cin >> passwd;
    writeToFile("credentials.txt", username + " : " + passwd);
    c.Connect("127.0.0.1", 60000);
    while (true) {
        std::string input_string;
        std::thread inputThread(readInputLoop, std::ref(input_string));
        inputThread.join();
        message m;
        m.msg = input_string;
        m.type = MessageType::MessageAll;
        c.Send(m);
        c.listen();
    }
    return 0;
}