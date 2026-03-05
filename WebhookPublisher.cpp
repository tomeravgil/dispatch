//
// Created by Tomer Avgil on 3/4/26.
//

#include "WebhookPublisher.h"

#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
using namespace std;


void WebhookPublisher::publishWebhook(const string &appType, const string &webhookUrl, const string &payload, const map<string, string>& files) {
    CURL* curl = curl_easy_init();
    if (!curl) return;

    if (appType == "discord" && !files.empty()) {
        // --- DISCORD MULTIPART (With Files) ---
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part;

        // 1. The Text Part (Discord needs the key "payload_json")
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "payload_json");
        curl_mime_data(part, payload.c_str(), CURL_ZERO_TERMINATED);

        // 2. The File Parts
        for (const auto& [path, content] : files) {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "file"); // Key for the attachment
            curl_mime_filedata(part, path.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_URL, webhookUrl.c_str());

        CURLcode res = curl_easy_perform(curl);
        curl_mime_free(mime);
    }
    else {
        // --- STANDARD JSON (Slack or Discord without files) ---
        string finalPayload = payload;

        // Slack Fallback: Append file contents to the message text
        if (appType == "slack" && !files.empty()) {
            finalPayload.pop_back(); // Remove closing }
            string fileAppend = "\\n\\n*Attached Files Content:*\\n";
            for (auto const& [path, content] : files) {
                fileAppend += "File: " + path + "\\n```\\n" + content + "```\\n";
            }
            // Use your escapeForJSON logic here if content has quotes!
            finalPayload += ", \"text\": \"" + fileAppend + "\"}";
        }

        curl_easy_setopt(curl, CURLOPT_URL, webhookUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, finalPayload.c_str());
        struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_perform(curl);
        curl_slist_free_all(headers);
    }

    curl_easy_cleanup(curl);
}
