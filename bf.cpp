#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <vector>
#include <thread>

int getPrintedLength(int val)
{
    if (val < 10) return 1;
    if (val < 100) return 2;
    else return 3;
}

class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

int main(int argc, char** args)
{
    int file_name_pos = -1;

    for (int i = 1; i < argc; i++)
    {
        if (args[i][0] != '-') file_name_pos = i;
    }

    if (file_name_pos == -1)
    {
        std::cout << "No input program." << std::endl;
        exit(0);
    }

    InputParser inputParser(argc, args);

    bool show_program = inputParser.cmdOptionExists("--show-program");
    bool show_memory = inputParser.cmdOptionExists("--show-memory");
    bool step_manually = inputParser.cmdOptionExists("--step-manually");
    bool show_progressive_output = inputParser.cmdOptionExists("--stream-output");
    
    const char* input_file = args[file_name_pos];

    std::ifstream inFile;
    inFile.open(input_file);

    std::stringstream strStream;
    strStream << inFile.rdbuf();
    std::string str = strStream.str();

    std::cout << str << std::endl;

    uint8_t memory[65536];
    int mem_ptr = 0;
    int pc_ptr = 0;

    std::vector<char> output;

    for (int i = 0; i < 65536; i++) memory[i] = 0;


    while(pc_ptr < str.size())
    {
        /* debug printing and stepping */

        if (show_program || show_memory || show_progressive_output) system("clear");

        if (show_program)
        {
            std::cout << str << std::endl;
            for (int i = 0; i < pc_ptr; i++) std::cout << " ";
            std::cout << "^" << std::endl;
        }

        if (show_memory)
        {
            for (int i = 0; i < 32; i++) std::cout << (int)memory[i] << " ";
            std::cout << std::endl;
            for (int i = 0; i < mem_ptr; i++)
            {
                std::cout << " ";
                for (int j = 0; j < getPrintedLength(memory[i]); j++)
                    std::cout << " ";
            }
            std::cout << "^" << std::endl;
        }

        if (show_progressive_output)
        {
            std::cout << std::endl;

            for (char c : output) std::cout << c;
            std::cout << std::endl;
        }

        if (step_manually) std::cin.get();
        //std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(10));

        /*-------------*/

        char ins = str[pc_ptr];

        if (ins == '>') mem_ptr += 1;
        if (ins == '<') mem_ptr -= 1;
        if (ins == '+') memory[mem_ptr] += 1;
        if (ins == '-') memory[mem_ptr] -= 1;
        if (ins == '.')
        {
            char c = memory[mem_ptr];
            if (c == 10) c = '\n';
            output.push_back(memory[mem_ptr]);
        }
        if (ins == ',')
        {
            char c = std::cin.get();
            //std::cout << "Read " << c << "(" << (int)c << ")" << std::endl;
            if (c != -1) memory[mem_ptr] = c;
        }
        if (ins == '[')
        {
            if (memory[mem_ptr] == 0)
            {
                int x = 0;
                while (x >= 0)
                {
                    pc_ptr += 1;
                    if (str[pc_ptr] == '[') x += 1;
                    if (str[pc_ptr] == ']') x -= 1;
                }
                pc_ptr -= 1;
            }
        }
        if (ins == ']')
        {
            if (memory[mem_ptr] != 0)
            {
                int x = 0;
                while (x >= 0)
                {
                    pc_ptr -= 1;
                    if (str[pc_ptr] == ']') x += 1;
                    if (str[pc_ptr] == '[') x -= 1;
                }
                pc_ptr -= 1;
            }
        }

        pc_ptr += 1;

        
    }

    std::cout << "Final output:\n-----------\n" << std::endl;

    for (char c : output) std::cout << c;
    std::cout << std::endl;


}