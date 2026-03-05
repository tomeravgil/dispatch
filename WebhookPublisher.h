//
// Created by Tomer Avgil on 3/4/26.
//

#ifndef DISPATCH_WEBHOOKPUBLISHER_H
#define DISPATCH_WEBHOOKPUBLISHER_H
#include <string>
#include <map>
using namespace std;

class WebhookPublisher {
public:
    static void publishWebhook(const string &appType, const string &webhookUrl, const string &payload, const map<string, string> &files);
};


#endif //DISPATCH_WEBHOOKPUBLISHER_H