#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
#include "JSON.hpp"
using namespace std;
using namespace autojson;

std::string RunBashCommand(const char* cmd) {
    //    Info("Runbashcommand: ", cmd);
    FILE* pipe(popen(cmd, "r"));
    if (!pipe)
        return "ERROR";

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string ReadFile(const string& file, std::function<void()> die_func = [](){}) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string file_information;

    if (fin) {
        fin.seekg(0, fin.end);
        file_information.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(&file_information[0], file_information.size());
        fin.close();
    } else {
        die_func();       
    }

    return file_information;
}

std::vector<std::string> Split(const std::string& txt, char split_char) {
    std::string current_txt = "";
    std::vector<std::string> output;
    for (int i = 0; i < (int)txt.size(); i += 1) {
        if (split_char == txt[i]) {
            output.push_back(current_txt);
            current_txt = "";
        } else {
            current_txt += txt[i];
        }
    }

    output.push_back(current_txt);
    return output;
}

vector<string> ParseCompileCommands() {
    auto compile_commands = ReadFile("compile_commands.json", [](){
        cerr << "[ERROR] No compile_commands.json file found\n";
        exit(-1);
    });

    auto j = JSON::readFromFile("compile_commands.json");

    vector<string> res;
    for (JSON itr : j) {
        auto compile = itr["command"];
        auto commands = Split(compile, ' ');
        for (int i = 1; i < (int)commands.size(); i += 1) {
            if (commands[i] == "-o") {
                i += 1;
                continue;
            }

            if (commands[i] == "-c") {
                i += 1;
                continue;
            }

            if (commands[i] != "") {
                res.push_back(commands[i]);
            }
        }
    }

    sort(res.begin(), res.end());
    res.resize(unique(res.begin(), res.end()) - res.begin());

    return res;
}

void CheckGitIgnored() {
    auto gitignorecontent = ReadFile(".gitignore");
    if (gitignorecontent.find(".ycm_extra_conf.py") != std::string::npos) {
        return;
    } else {
        gitignorecontent += ".ycm_extra_conf.py\n";
        gitignorecontent += ".ycm_extra_conf.pyc\n";
        ofstream fout(".gitignore");
        fout << gitignorecontent;
        fout.close();
    }
}

void CreateYCMConf(vector<string> commands) {
    string content = 
    "import sys\n"
    "config_file = '/.ycm_extra_conf.py'\n"
    "with open(config_file) as f:\n"
    "    code = compile(f.read(), config_file, 'exec')\n"
    "    exec(code, globals(), locals())\n"
    "BASE_FLAGS += [\n";
        
    for (auto itr : commands) {
        content += "'" + itr + "',\n";
    }

    content += "]\n";
    
    ofstream fout(".ycm_extra_conf.py");

    fout << content;
    fout.close();
}

int main() {
    auto commands = ParseCompileCommands();
    CheckGitIgnored();
    ReadFile("/.ycm_extra_conf.py", []() {
        cerr << "[ERROR] No /.ycm_extra_conf.py file!\n";
        exit(-1);
    });

    CreateYCMConf(commands);
}

