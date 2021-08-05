#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

std::string strip(std::string s)
{
    s.erase(std::find_if( s.rbegin(), s.rend(),
                [](int ch) { return !std::isspace(ch); }
            ).base(), s.end());
    return s;
}

int main(int argc, char** argv)
{
    std::clog << "Start server" << std::endl;

    while(true) {

        // Blocking call on named pipes.
        std::ifstream ifs(argv[1]);
        std::stringstream datas;
        datas << ifs.rdbuf();
        ifs.close();

        std::string data = strip(datas.str());

        std::ofstream ofs(argv[2]);
        std::clog << "Received: <" << data << ">" << std::endl;
        ofs << data << std::endl;
        ofs.close();

        if(data == "exit") {
            break;
        }
    }

    std::clog << "Stop server" << std::endl;
}
