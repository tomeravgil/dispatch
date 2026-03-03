#include <iostream>
#include "CommandProcessor.h"
#include "ConfigManager.h"
using namespace std;

int main(int argc, char *argv[]) {
    CommandProcessor processor(argc, argv);
    processor.processCommand();

    cout<<processor.getCommandOutput()<<endl;
}