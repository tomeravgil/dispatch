#include <gtest/gtest.h>
#include "CommandProcessor.h"
#include <stdexcept>
#include <map>

// Test that instantiation throws an exception when there are insufficient arguments
TEST(CommandProcessorTest, InsufficientArgs) {
    char* argv[] = { (char*)"dispatch" };
    EXPECT_THROW(CommandProcessor(1, argv), std::invalid_argument);
}

// Test basic command parsing
TEST(CommandProcessorTest, ParsesCommandCorrectly) {
    char* argv[] = { (char*)"dispatch", (char*)"echo hello" };
    EXPECT_NO_THROW({
        CommandProcessor cp(2, argv);
    });
}

// Test parsing of output-files flag
TEST(CommandProcessorTest, ParsesOutputFilesFlag) {
    char* argv[] = { (char*)"dispatch", (char*)"echo", (char*)"hello", (char*)"--output-files", (char*)"file1.txt", (char*)"file2.png" };
    CommandProcessor cp(6, argv);
    
    std::map<std::string, std::string> filesMap = cp.getFileToOutputMap();
    EXPECT_EQ(filesMap.size(), 2);
    EXPECT_TRUE(filesMap.find("file1.txt") != filesMap.end());
    EXPECT_TRUE(filesMap.find("file2.png") != filesMap.end());
    
    // Check they are initialized as empty output
    EXPECT_EQ(filesMap["file1.txt"], "");
    EXPECT_EQ(filesMap["file2.png"], "");
}
