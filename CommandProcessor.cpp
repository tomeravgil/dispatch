//
// Created by Tomer Avgil on 3/2/26.
//

#include "CommandProcessor.h"
#include <cstdio>
#include <fstream>
#include <string>

CommandProcessor::CommandProcessor(int argc, char* argv[]) {

    if (argc < 2) {
        throw new invalid_argument("Not enough arguments");
    }

    this->command = string(argv[1]);
    for (int i = 2; i < argc; i++) {
        string arg = string(argv[i]);
        if (arg == "--output-files") {
            while (i < argc || arg.find("--") != string::npos) {
                this->fileToOutput[arg] = "";
                i++;
            }
        }
        this->command += " " + arg;
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
    string fileOutputs;
    for (const auto &pair : this->fileToOutput) {
        ifstream file(pair.first);
        string fileOutput;
        string lineOutput;
        if (getline(file, lineOutput)) {
            fileOutput += fileOutput;
        }
        fileOutputs += this-> fileToOutput[fileOutput] + "\n" + fileOutput + "\n";
        this->fileToOutput[pair.first] = fileOutput;
    }
    return fileOutputs;
}

string CommandProcessor::getCommandOutput() {
    return this->commandOutput;
}
