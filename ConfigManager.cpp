//
// Created by Tomer Avgil on 3/2/26.
//

#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <utility>
using namespace std;

ConfigManager::ConfigManager() {
    const char* homeDir = getenv("HOME");
    string path;
    if (homeDir) {
        path = string(homeDir) + "/.dispatchconfig.txt";
    } else {
        path = ".dispatchconfig.txt"; // Fallback to current dir
    }

    ifstream configFile(path);
    string line;
    while (getline(configFile, line)) {

        int splitPos = line.find("=");

        string value = line.substr(splitPos + 1);
        if (line[0] == '*') {
            string key = line.substr(1, splitPos);
            this->selectedWebhooks[key] = value;
            continue;
        }
        string key = line.substr(0, splitPos);
        this->webhooks[key] = value;

        if (selectedWebhooks.empty()) {
            this->selectedWebhooks[key] = this->webhooks[key];
        }
    }
}

bool ConfigManager::addWebhook(const string& appName, const string& webhookUrl) {
    if (this->webhooks.contains(appName)) {
        cerr<<"webhook already exists"<<endl;
        return false;
    }
    this->webhooks[appName] = webhookUrl;
    ofstream configFile(this->configFileLocation);
    configFile<<appName<<"="<<webhookUrl<<endl;
    configFile.close();

    if (!this->overwriteConfigFile()) {
        cerr<<"Error writing to file"<<this->configFileLocation<<endl;
    }

    return true;
}

bool ConfigManager::removeWebhook(const string& appName) {

    if (!this->webhooks.contains(appName)) {
        cerr<<"webhook doesn't exist"<<endl;
        return false;
    }
    string appNameValue = this->webhooks[appName];
    string toBeDeleted = appName + "=" + this->webhooks[appName];
    this->webhooks.erase(appName);

    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = appNameValue;
        return false;
    }

    return true;
}

bool ConfigManager::overwriteConfigFile() const {
    const char* homeDir = getenv("HOME");
    string actualPath = homeDir ? (string(homeDir) + "/.dispatchconfig.txt") : ".dispatchconfig.txt";
    string tempPath = actualPath + ".tmp";

    try {
        ofstream outFile(tempPath);
        for (const auto& webhook : this->webhooks) {
            if (this->selectedWebhooks.contains(webhook.first)) {
                outFile << "*";
            }
            outFile << webhook.first << "=" << webhook.second << endl;
        }
        outFile.close();

        // 1. Remove old file
        remove(actualPath.c_str());
        // 2. Move temp to actual
        if (rename(tempPath.c_str(), actualPath.c_str()) != 0) {
            cerr << "Rename failed!" << endl;
            return false;
        }
    }
    catch (exception& e) {
        cerr << "failed to save config file: " << e.what() << endl;
        return false;
    }
    return true;
}

bool ConfigManager::updateWebhook(const string& appName, const string& webhookUrl) {
    if (!this->webhooks.contains(appName)) {
        cerr<<"webhook doesn't exist"<<endl;
        return false;
    }

    string appNameValue = this->webhooks[appName];
    this->webhooks[appName] = webhookUrl;

    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = appNameValue;
        return false;
    }

    return true;
}

void ConfigManager::readConfig(int argc, char* argv[]) {
    // Basic safety check for command existence
    if (argc < 3) {
        cerr << "No command provided. Use \"config help\" for valid commands." << endl;
        return;
    }

    const string command = argv[2];
    bool validCommand = false;

    // 1. ADD
    if (command == "add") {
        validCommand = true;
        if (argc == 5) {
            this->addWebhook(argv[3], argv[4]);
            return;
        }
    }
    // 2. REMOVE
    else if (command == "remove") {
        validCommand = true;
        if (argc == 4) {
            this->removeWebhook(argv[3]);
            return;
        }
    }
    // 3. UPDATE
    else if (command == "update") {
        validCommand = true;
        if (argc == 5) {
            this->updateWebhook(argv[3], argv[4]);
            return;
        }
    }
    // 4. SELECT
    else if (command == "select") {
        validCommand = true;
        if (argc == 4) {
            this->selectWebhook(argv[3]);
            return;
        }
    }
    // 5. DESELECT
    else if (command == "deselect") {
        validCommand = true;
        if (argc == 4) {
            this->deselectWebhook(argv[3]);
            return;
        }
    }
    // 6. LIST SELECTED
    else if (command == "list-selected") {
        validCommand = true;
        this->listSelectedWebhooks();
        return;
    }
    // 7. PRINT ALL
    else if (command == "print-all") {
        validCommand = true;
        cout << *this; // Uses your overloaded ostream operator
        return;
    }
    // 8. HELP
    else if (command == "help") {
        this->printHelp();
        return;
    }

    // Error Handling
    if (validCommand) {
        cerr << "Invalid parameters for " << command << ". Use \"config help\" to list inputs." << endl;
    } else {
        cerr << "Command not found. Use \"config help\" for valid commands." << endl;
    }
}

ostream& operator<<(ostream& out, const ConfigManager& configManager) {
    map<string,string> webhooks = configManager.webhooks;
    for (const auto& webhook : webhooks) {
        out<<webhook.first<<"="<<webhook.second<<endl;
    }
    return out;
}

void ConfigManager::printHelp() const {
    cout << "\n--- Dispatch Config Help ---" << endl;
    cout << "Usage: dispatch config <command> [arguments]" << endl;
    cout << "\nCommands:" << endl;
    cout << "  add <name> <url>       Save a new webhook." << endl;
    cout << "  remove <name>          Delete a webhook by name." << endl;
    cout << "  update <name> <url>    Update the URL for an existing name." << endl;
    cout << "  select <name>          Mark a webhook as active (will receive posts)." << endl;
    cout << "  deselect <name>        Stop sending posts to a specific webhook." << endl;
    cout << "  list-selected          Show all webhooks currently marked with '*'." << endl;
    cout << "  print-all              Show every webhook saved in your config." << endl;
    cout << "  help                   Display this menu." << endl;
    cout << "----------------------------\n" << endl;
}

bool ConfigManager::selectWebhook(const string &appName) {
    if (!this->webhooks.contains(appName)) {
        cerr<<"webhook"<<appName<<" doesn't exist"<<endl;
        return false;
    }
    this->selectedWebhooks[appName] = this->webhooks[appName];
    string appNameValue = this->webhooks[appName];
    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = appNameValue;
        return false;
    }

    return true;
}

bool ConfigManager::deselectWebhook(const string &appName) {

    if (!this->webhooks.contains(appName)) {
        cerr<<"webhook"<<appName<<" doesn't exist"<<endl;
        return false;
    }
    this->selectedWebhooks.erase(appName);
    string appNameValue = this->webhooks[appName];
    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = appNameValue;
        return false;
    }

    return true;
}

void ConfigManager::listSelectedWebhooks() const {
    for (const auto& webhook : this->selectedWebhooks) {
        cout<<webhook.first<<"="<<webhook.second<<endl;
    }
}

map<string,string> ConfigManager::getSelectedWebhooks() const {
    return this->selectedWebhooks;
}
