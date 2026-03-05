#include <gtest/gtest.h>
#include "ConfigManager.h"
#include <fstream>
#include <filesystem>
#include <cstdlib>

class ConfigManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a unique temporary directory for the config so tests are isolated
        temp_dir = std::filesystem::temp_directory_path().string() + "/dispatch_test_temp";
        std::filesystem::create_directory(temp_dir);
        setenv("HOME", temp_dir.c_str(), 1);
    }
    
    void TearDown() override {
        std::filesystem::remove_all(temp_dir);
    }
    
    std::string temp_dir;
};

TEST_F(ConfigManagerTest, InitializationCreatesFile) {
    ConfigManager cm;
    // adding a webhook will save to file
    char* argvAdd[] = { (char*)"dispatch", (char*)"config", (char*)"add", (char*)"slack", (char*)"http://slack.webhook" };
    cm.readConfig(5, argvAdd);

    // Verify it was added by creating another config manager Instance to read from disk
    ConfigManager cm2;
    std::string path = temp_dir + "/.dispatchconfig.txt";
    EXPECT_TRUE(std::filesystem::exists(path));
}

TEST_F(ConfigManagerTest, RemoveWebhook) {
    ConfigManager cm;
    char* argvAdd[] = { (char*)"dispatch", (char*)"config", (char*)"add", (char*)"discord", (char*)"http://discord.webhook" };
    cm.readConfig(5, argvAdd);
    
    char* argvRemove[] = { (char*)"dispatch", (char*)"config", (char*)"remove", (char*)"discord" };
    cm.readConfig(4, argvRemove);
    
    ConfigManager cm2;
    auto selected = cm2.getSelectedWebhooks();
    EXPECT_TRUE(selected.find("discord") == selected.end());
}

TEST_F(ConfigManagerTest, UpdateWebhook) {
    ConfigManager cm;
    char* argvAdd[] = { (char*)"dispatch", (char*)"config", (char*)"add", (char*)"test", (char*)"url1" };
    cm.readConfig(5, argvAdd);
    
    char* argvUpdate[] = { (char*)"dispatch", (char*)"config", (char*)"update", (char*)"test", (char*)"url2" };
    cm.readConfig(5, argvUpdate);
    
    ConfigManager cm2;
    // by default, first added hook is selected if selected was empty
    auto selected = cm2.getSelectedWebhooks();
    EXPECT_TRUE(selected.find("test") != selected.end());
    EXPECT_EQ(selected["test"], "url2");
}

TEST_F(ConfigManagerTest, SelectDeselectWebhook) {
    ConfigManager cm;
    char* argvAdd[] = { (char*)"dispatch", (char*)"config", (char*)"add", (char*)"slack", (char*)"url1" };
    cm.readConfig(5, argvAdd);
    char* argvAdd2[] = { (char*)"dispatch", (char*)"config", (char*)"add", (char*)"zebra", (char*)"url2" };
    cm.readConfig(5, argvAdd2);
    
    char* argvSelect[] = { (char*)"dispatch", (char*)"config", (char*)"select", (char*)"zebra" };
    cm.readConfig(4, argvSelect);

    ConfigManager cm2;
    auto selected = cm2.getSelectedWebhooks();
    EXPECT_TRUE(selected.find("zebra") != selected.end());
    
    char* argvDeselect[] = { (char*)"dispatch", (char*)"config", (char*)"deselect", (char*)"zebra" };
    cm.readConfig(4, argvDeselect);
    
    ConfigManager cm3;
    selected = cm3.getSelectedWebhooks();
    EXPECT_TRUE(selected.find("zebra") == selected.end());
}
