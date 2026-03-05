//
// Created by Tomer Avgil on 3/2/26.
//

#ifndef DISPATCH_CONFIGMANAGER_H
#define DISPATCH_CONFIGMANAGER_H
#include <string>
#include <map>
using namespace std;

class ConfigManager {
public:
    ConfigManager();

    void readConfig(int argc, char* argv[]);
    map<string,string> getSelectedWebhooks() const;
    friend ostream& operator<<(ostream& out, const ConfigManager& configManager);

private:
    bool overwriteConfigFile() const;
    bool addWebhook(const string& appName, const string& webhookUrl);
    bool removeWebhook(const string& appName);
    bool updateWebhook(const string& appName, const string& webhookUrl);
    void printHelp() const;
    bool selectWebhook(const string& appName);
    bool deselectWebhook(const string& appName);
    void listSelectedWebhooks() const;


    const string configFileLocation = "~/.dispatchconfig.txt";
    map<string,string> webhooks;
    map<string,string> selectedWebhooks;
};


#endif //DISPATCH_CONFIGMANAGER_H