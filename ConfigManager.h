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
    bool addWebhook(string appName, string webhookUrl);
    bool removeWebhook(string appName);
    bool changeSelectedWebhook(string appName, string webhookUrl);
    friend ostream& operator<<(ostream& out, const ConfigManager& configManager);

private:
    bool overwriteConfigFile() const;
    const string configFileLocation = "~/.dispatchconfig.txt";
    map<string,string> webhooks;
    map<string,string> temporaryModifiedWebhooks;
    map<string,string> selectedWebhooks;
};


#endif //DISPATCH_CONFIGMANAGER_H