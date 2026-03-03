//
// Created by Tomer Avgil on 3/2/26.
//

#include "ConfigManager.h"
#include <iostream>
#include <fstream>
using namespace std;

ConfigManager::ConfigManager() {
    ifstream configFile(this->configFileLocation);
    string line;
    while (getline(configFile, line)) {
        int splitPos = line.find("=");
        string key = line.substr(0, splitPos);
        string value = line.substr(splitPos + 1);
        this->webhooks[key] = value;

        if (selectedWebhooks.empty()) {
            this->selectedWebhooks[key] = this->webhooks[key];
        }
    }
}

bool ConfigManager::addWebhook(string appName, string webhookUrl) {
    if (this->webhooks.find(appName) != this->webhooks.end()) {
        cerr<<"webhook already exists"<<endl;
        return false;
    }
    this->webhooks[appName] = webhookUrl;
    ofstream configFile(this->configFileLocation);
    configFile<<appName<<"="<<webhookUrl<<endl;
    configFile.close();

    return true;
}

bool ConfigManager::removeWebhook(string appName) {
    if (this->webhooks.find(appName) == this->webhooks.end()) {
        cerr<<"webhook doesn't exist"<<endl;
        return false;
    }
    string toBeDeleted = appName + "=" + this->webhooks[appName];
    this->temporaryModifiedWebhooks[appName] = this->webhooks[appName];
    this->webhooks.erase(appName);

    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = this->temporaryModifiedWebhooks[appName];
        this->temporaryModifiedWebhooks.erase(appName);
        return false;
    }

    return true;
}

bool ConfigManager::overwriteConfigFile() const {
    try {
        ofstream outFile("temp.txt");
        string line;

        for (const auto& webhook : this->webhooks) {
            outFile<<webhook.first<<"="<<webhook.second<<endl;
        }

        outFile.close();

        remove(this->configFileLocation.c_str());
        rename("temp.txt", this->configFileLocation.c_str());
    }
    catch (exception& e) {
        cerr<<"failed to save config file"<<endl;
        return false;
    }

    return true;
}

bool ConfigManager::changeSelectedWebhook(string appName, string webhookUrl) {
    if (this->webhooks.find(appName) == this->webhooks.end()) {
        cerr<<"webhook doesn't exist"<<endl;
        return false;
    }

    this->temporaryModifiedWebhooks[appName] = this->webhooks[appName];

    this->webhooks[appName] = webhookUrl;

    if (!this->overwriteConfigFile()) {
        this->webhooks[appName] = this->temporaryModifiedWebhooks[appName];
        return false;
    }

    return true;
}

ostream& operator<<(ostream& out, const ConfigManager& configManager) {
    map<string,string> webhooks = configManager.webhooks;
    for (const auto& webhook : webhooks) {
        out<<webhook.first<<"="<<webhook.second<<endl;
    }
    return out;
}