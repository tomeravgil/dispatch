//
// Created by Tomer Avgil on 3/2/26.
//

#include "CommandProcessor.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

CommandProcessor::CommandProcessor(int argc, char* argv[]) {
    if (argc < 2) throw invalid_argument("Usage: dispatch \"command\" --output-files file1.txt file2.png");

    this->command = argv[1];
    bool parsingFiles = false;

    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--output-files") {
            parsingFiles = true;
            continue;
        }

        if (parsingFiles) {
            this->fileToOutput[arg] = "";
        } else {
            this->command += " " + arg;
        }
    }
}

void CommandProcessor::processCommand() {
    char buffer[256];

    if (this->command.empty()) {
        throw new invalid_argument("No Commands");
    }

    FILE* pipe = popen(this->command.c_str(), "r");

    if (!pipe) {
        throw runtime_error("popen() failed");
    }

    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            this->commandOutput += buffer;
        }
    } catch (exception e) {
        pclose(pipe);
        throw runtime_error(e.what());
    }

    pclose(pipe);
}

string CommandProcessor::readFileOutputs() {
    for (auto &pair : this->fileToOutput) {
        ifstream file(pair.first, ios::binary | ios::ate);
        if (!file.is_open()) {
            cerr << "Warning: Could not open file " << pair.first << endl;
            continue;
        }

        streamsize size = file.tellg();
        file.seekg(0, ios::beg);

        string buffer(size, '\0');
        if (file.read(&buffer[0], size)) {
            this->fileToOutput[pair.first] = buffer;
        }
    }
    return "Files loaded into memory.";
}

string CommandProcessor::getCommandOutput() {

    return this->commandOutput;
}

map<string, string> CommandProcessor::getFileToOutputMap() const {
    return this->fileToOutput;
}

map<string,pair<string,string>> CommandProcessor::getCommandPayloads(map<string,string> selectedWebhooks) const {
    if (!this->commandOutput.empty()) {
        cerr<<"command Output is empty"<<endl;
    }
    string cleanedOutput = this->escapeForJSON(this->commandOutput);
    map<string, pair<string, string>> commandPayloads;
    if (selectedWebhooks.empty()) {
        cerr<<"No Selected Webhooks. Select Webhooks using config commands. Use \"config help\" for more"<<endl;
    }
    for (const auto &pair : selectedWebhooks) {
        string payload;
        string payloadType;
        if (pair.second.find("discord") != string::npos) {
            payload = this->createDiscordPayload(cleanedOutput);
            payloadType = "discord";
        }
        else if (pair.second.find("slack") != string::npos) {
            payload = this->createSlackPayload(cleanedOutput);
            payloadType = "slack";
        }
        else {
            cerr<<"Unknown payload type"<<endl;
        }

        commandPayloads[payloadType] = make_pair(pair.second, payload);
    }
    return commandPayloads;
}

string CommandProcessor::createDiscordPayload(const string& rawOutput) {
    std::string formatted = "**Dispatch Result:**\\n```bash\\n" + rawOutput + "```";

    return "{\"content\": \"" + formatted + "\"}";
}

string CommandProcessor::createSlackPayload(const string& rawOutput) {
    std::string formatted = "*Dispatch Result:*\\n```" + rawOutput + "```";

    return "{\"text\": \"" + formatted + "\"}";
}

string CommandProcessor::escapeForJSON(string data) {
    std::string buffer;
    buffer.reserve(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        switch (data[i]) {
            case '"':  buffer += "\\\""; break;
            case '\\': buffer += "\\\\"; break;
            case '\b': buffer += "\\b";  break;
            case '\f': buffer += "\\f";  break;
            case '\n': buffer += "\\n";  break;
            case '\r': buffer += "\\r";  break;
            case '\t': buffer += "\\t";  break;
            default:   buffer += data[i];
        }
    }
    return buffer;
}