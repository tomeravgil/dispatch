//
// Created by Tomer Avgil on 3/2/26.
//

#ifndef DISPATCH_COMMANDPROCESSOR_H
#define DISPATCH_COMMANDPROCESSOR_H
#include <string>
#include <map>
using namespace std;

class CommandProcessor {

public:
    CommandProcessor(int argc, char* argv[]);
    void processCommand();
    string getCommandOutput();
    string readFileOutputs();
private:
    string commandOutput;
    string command;
    map<string, string> fileToOutput;
};


#endif //DISPATCH_COMMANDPROCESSOR_H