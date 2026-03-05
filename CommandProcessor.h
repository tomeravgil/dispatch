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
    map<string, string> getFileToOutputMap() const;

    map<string,pair<string,string>> getCommandPayloads(map<string, string> selectedWebhooks) const;

    string readFileOutputs();
private:
    static string escapeForJSON(string data);
    static string createDiscordPayload(const string& rawOutput);
    static string createSlackPayload(const string& rawOutput);
    string commandOutput;
    string command;
    map<string, string> fileToOutput;
};


#endif //DISPATCH_COMMANDPROCESSOR_H