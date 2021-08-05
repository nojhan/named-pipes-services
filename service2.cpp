#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <cassert>
#include <algorithm>

#include <sys/stat.h>

enum ERROR { NOT_FIFO=1, NO_CONTEXT };

class Service
{
protected:
    bool _has_current_context;
    std::mutex _mutex;
    std::string _file_current_context;
    std::string _file_data;
    std::string _out;
    std::string _current_context;

    bool has_current_context()
    {
        std::lock_guard<std::mutex> guarded_scope(_mutex);
        return _has_current_context;
    }

    void has_current_context(bool flag)
    {
        std::lock_guard<std::mutex> guarded_scope(_mutex);
        _has_current_context = flag;
    }

public:

    Service(
        std::string context,
        std::string data,
        std::string out
    ) :
        _has_current_context(false),
        _file_current_context(context),
        _file_data(data),
        _out(out)
    {}

    std::string strip(std::string s)
    {
        s.erase(std::find_if( s.rbegin(), s.rend(),
                    [](int ch) { return !std::isspace(ch); }
                    ).base(), s.end());
        return s;
    }

    void update_current_context()
    {
        while(true) {
            std::clog << "Wait for context..." << std::endl;
            bool has_error = false;
            try {
                std::ifstream if_current_context(_file_current_context);
                std::stringstream current_context;
                current_context << if_current_context.rdbuf();
                if_current_context.close();
                _current_context = strip(current_context.str());
            } catch(...) {
                has_error = true;
            }
            if(not has_error) {
                this->has_current_context(true);
                std::clog << "\tReceived context: " << _current_context << std::endl;
            }
        }
    }

    void handle_data()
    {
        while(true) {
            if(this->has_current_context()) {
                std::string data;
                std::clog << "Wait for data..." << std::endl;
                bool has_error = false;
                try {
                    std::ifstream if_data(_file_data);
                    std::stringstream datas;
                    datas << if_data.rdbuf();
                    if_data.close();
                    data = strip(datas.str());
                } catch(...) {
                    has_error = true;
                }
                if(not has_error) {
                    std::clog << "\tReceived data: " << data << std::endl;

                    std::clog << "Do stuff..." << std::endl;
                    std::ostringstream result;
                    result << _current_context << ": " << data;
                    std::clog << "\tdone" << std::endl;

                    std::clog << "Output..." << std::endl;
                    std::ofstream out(_out);
                    out << result.str() << std::endl;
                    out.close();
                    std::clog << "\tdone" << std::endl;
                } // if not has_error
            }
        } // while true
    }
};

bool is_named_pipe_fifo(char* filename)
{
    struct stat st;
    stat(filename, &st);
    if(not S_ISFIFO(st.st_mode) ) {
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    assert(argc = 3);

    for(size_t i=1; i < 3; ++i) {
        if( not is_named_pipe_fifo(argv[i]) ) {
            std::cerr << "ERROR: " << argv[i] << " is not a named pipe FIFO" << std::endl;
            exit(ERROR::NOT_FIFO);
        }
    }

    std::clog << "Start server" << std::endl;
    std::clog.flush();
    Service server(argv[1], argv[2], argv[3]);

    std::thread do_current_context(&Service::update_current_context, &server);
    std::thread do_tasks(&Service::handle_data, &server);

    do_current_context.join();
    do_tasks.join();
    std::clog << "End" << std::endl;

}
