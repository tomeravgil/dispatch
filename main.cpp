#include <iostream>
#include <curl/curl.h>
#include "CommandProcessor.h"
#include "ConfigManager.h"
#include "WebhookPublisher.h"

using namespace std;

void printGlobalHelp() {
    cout << "\n--- DISPATCH CLI HELP ---" << endl;
    cout << "Usage: dispatch <command> [options]" << endl;
    cout << "\nCore Commands:" << endl;
    cout << "  \"<shell command>\"      Execute a command and send output to webhooks." << endl;
    cout << "                         Example: dispatch \"ls -la\"" << endl;
    cout << "  config <subcommand>    Manage your Discord/Slack webhook URLs." << endl;
    cout << "  help                   Display this help menu." << endl;
    cout << "\nOptions:" << endl;
    cout << "  --output-files <f1>... Attach specific files to the notification." << endl;
    cout << "                         Example: dispatch \"python3 script.py\" --output-files logs.txt" << endl;
    cout << "\nTry \"dispatch config help\" for webhook management details.\n" << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printGlobalHelp();
        return 0;
    }

    string firstArg = argv[1];

    if (firstArg == "help" || firstArg == "--help" || firstArg == "-h") {
        printGlobalHelp();
        return 0;
    }

    if (firstArg == "config") {
        ConfigManager configManager;
        configManager.readConfig(argc, argv);
        return 0;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    ConfigManager configManager;

    try {
        CommandProcessor processor(argc, argv);
        processor.processCommand();
        processor.readFileOutputs();

        map<string, string> selectedWebhooks = configManager.getSelectedWebhooks();
        if (selectedWebhooks.empty()) {
            cerr << "Error: No webhooks selected. Use 'dispatch config select <name>' first." << endl;
            curl_global_cleanup();
            return 1;
        }

        auto payloads = processor.getCommandPayloads(selectedWebhooks);
        WebhookPublisher publisher;

        for (auto const& [type, data] : payloads) {
            publisher.publishWebhook(type, data.first, data.second, processor.getFileToOutputMap());
        }

        cout << "Dispatch successful!" << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    curl_global_cleanup();
    return 0;
}