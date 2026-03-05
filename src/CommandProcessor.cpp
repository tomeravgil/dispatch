//
// Created by Tomer Avgil on 3/2/26.
//

#include "CommandProcessor.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <sys/wait.h>
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

    auto startTime = chrono::steady_clock::now();
    FILE* pipe = popen(this->command.c_str(), "r");

    if (!pipe) {
        throw runtime_error("popen() failed");
    }

    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            this->commandOutput += buffer;
        }
    } catch (exception e) {
        int status = pclose(pipe);
        if (WIFEXITED(status)) {
            this->exitCode = WEXITSTATUS(status);
        } else {
            this->exitCode = -1;
        }
        this->success = false;
        throw runtime_error(e.what());
    }

    int status = pclose(pipe);
    auto endTime = chrono::steady_clock::now();
    this->durationMs = chrono::duration<double, std::milli>(endTime - startTime).count();
    
    if (WIFEXITED(status)) {
        this->exitCode = WEXITSTATUS(status);
        this->success = (this->exitCode == 0);
    } else {
        this->exitCode = -1;
        this->success = false;
    }
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

map<string,pair<string,string>> CommandProcessor::getCommandPayloads(map<string,string> selectedWebhooks) {
    if (this->commandOutput.empty()) {
        cerr<<"command Output is empty"<<endl;
    }
    
    string payloadOutput = this->commandOutput;
    if (payloadOutput.length() > 1800) {
        this->fileToOutput["output.txt"] = this->commandOutput;
        payloadOutput = payloadOutput.substr(0, 500) + "\n... [TRUNCATED - Full output attached as file]";
    }
    
    string cleanedOutput = this->escapeForJSON(payloadOutput);
    map<string, pair<string, string>> commandPayloads;
    if (selectedWebhooks.empty()) {
        cerr<<"No Selected Webhooks. Select Webhooks using config commands. Use \"config help\" for more"<<endl;
    }
    for (const auto &pair : selectedWebhooks) {
        string payload;
        string payloadType;
        if (pair.second.find("discord") != string::npos) {
            payload = this->createDiscordPayload(cleanedOutput, this->success, this->exitCode, this->durationMs);
            payloadType = "discord";
        }
        else if (pair.second.find("slack") != string::npos) {
            payload = this->createSlackPayload(cleanedOutput, this->success, this->exitCode, this->durationMs);
            payloadType = "slack";
        }
        else {
            cerr<<"Unknown payload type"<<endl;
        }

        commandPayloads[payloadType] = make_pair(pair.second, payload);
    }
    return commandPayloads;
}

string CommandProcessor::createDiscordPayload(const string& rawOutput, bool success, int exitCode, double durationMs) {
    ostringstream ss;
    if (durationMs >= 1000) {
        ss << fixed << setprecision(2) << (durationMs / 1000.0) << "s";
    } else {
        ss << fixed << setprecision(2) << durationMs << "ms";
    }
    string timeString = ss.str();

    string statusEmoji = success ? "✅" : "❌";
    string statusText = success ? "Success" : "Failed (Code " + to_string(exitCode) + ")";

    std::string formatted = "**Dispatch Result:** " + statusEmoji + " " + statusText + " | ⏱️ `" + timeString + "`\\n```bash\\n" + rawOutput + "```";

    return "{\"content\": \"" + formatted + "\"}";
}

string CommandProcessor::createSlackPayload(const string& rawOutput, bool success, int exitCode, double durationMs) {
    ostringstream ss;
    if (durationMs >= 1000) {
        ss << fixed << setprecision(2) << (durationMs / 1000.0) << "s";
    } else {
        ss << fixed << setprecision(2) << durationMs << "ms";
    }
    string timeString = ss.str();

    string statusEmoji = success ? "✅" : "❌";
    string statusText = success ? "Success" : "Failed (Code " + to_string(exitCode) + ")";

    std::string formatted = "*Dispatch Result:* " + statusEmoji + " " + statusText + " | ⏱️ `" + timeString + "`\\n```" + rawOutput + "```";

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