//
// Created by Tomer Avgil on 3/4/26.
//

#include "WebhookPublisher.h"

#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
using namespace std;


static string escapeJSON(const string& data) {
    string buffer;
    buffer.reserve(data.size());
    for (char c : data) {
        switch (c) {
            case '"':  buffer += "\\\""; break;
            case '\\': buffer += "\\\\"; break;
            case '\b': buffer += "\\b";  break;
            case '\f': buffer += "\\f";  break;
            case '\n': buffer += "\\n";  break;
            case '\r': buffer += "\\r";  break;
            case '\t': buffer += "\\t";  break;
            default:   buffer += c;
        }
    }
    return buffer;
}

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
        int fileIdx = 0;
        for (const auto& [path, content] : files) {
            part = curl_mime_addpart(mime);
            string fieldName = "file" + to_string(fileIdx++);
            curl_mime_name(part, fieldName.c_str()); 
            
            // Extract just the filename to be safe
            size_t slashPos = path.find_last_of("/\\");
            string safeFilename = (slashPos != string::npos) ? path.substr(slashPos + 1) : path;
            
            curl_mime_filename(part, safeFilename.c_str());
            curl_mime_data(part, content.data(), content.size());
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
                string safeContent = escapeJSON(content);
                if (safeContent.length() > 2000) {
                    safeContent = safeContent.substr(0, 2000) + "\\n... [FILE TRUNCATED DUE TO SLACK LIMITS]";
                }
                fileAppend += "File: " + path + "\\n```\\n" + safeContent + "```\\n";
            }
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
