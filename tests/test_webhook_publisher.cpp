#include <gtest/gtest.h>
#include "WebhookPublisher.h"
#include <map>
#include <string>

// We can't easily mock libcurl without dependency injection or linking a mock static library,
// so we execute tests with invalid URLs or unresolvable local URLs to ensure it handles network failures safely without crashing.

TEST(WebhookPublisherTest, EmptyPublishDoesNotCrash) {
    std::map<std::string, std::string> files;
    
    EXPECT_NO_THROW({
        WebhookPublisher::publishWebhook("slack", "", "{}", files);
    });
}

TEST(WebhookPublisherTest, InvalidSlackUrlDoesNotCrash) {
    std::map<std::string, std::string> files;
    EXPECT_NO_THROW({
        WebhookPublisher::publishWebhook("slack", "http://0.0.0.0:12345/webhook", "{\"text\":\"hello\"}", files);
    });
}

TEST(WebhookPublisherTest, SlackWithFilesDoesNotCrash) {
    std::map<std::string, std::string> files;
    files["invalid.txt"] = "This is a test block";
    
    EXPECT_NO_THROW({
        WebhookPublisher::publishWebhook("slack", "http://0.0.0.0:12345/webhook", "{\"text\":\"hello\"}", files);
    });
}

TEST(WebhookPublisherTest, DiscordWithFilesDoesNotCrash) {
    std::map<std::string, std::string> files;
    files["test_file.txt"] = "dummy file content";
    EXPECT_NO_THROW({
        WebhookPublisher::publishWebhook("discord", "http://0.0.0.0:12345/webhook", "{\"content\":\"hello\"}", files);
    });
}

TEST(WebhookPublisherTest, DiscordEmptyFilesDoesNotCrash) {
    std::map<std::string, std::string> files;
    EXPECT_NO_THROW({
        WebhookPublisher::publishWebhook("discord", "http://0.0.0.0:12345/webhook", "{\"content\":\"hello\"}", files);
    });
}
