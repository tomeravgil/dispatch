#!/bin/bash

#!/bin/bash

# Function to check and install dependencies
install_dep() {
    if ! command -v $1 &> /dev/null; then
        echo "$1 not found. Installing..."
        if [[ "$OSTYPE" == "darwin"* ]]; then
            brew install $1
        elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
            sudo apt update && sudo apt install -y $1
        fi
    else
        echo "$1 is already installed."
    fi
}

# Check for both the library and the build tool
install_dep "cmake"
install_dep "curl" # This usually brings libcurl-dev on Linux

# 2. Build the project
echo "Building dispatch..."
mkdir -p build
cd build
cmake ..
make
cd ..

# 3. Add to PATH (Permanent for Bash/Zsh)
BINARY_PATH=$(pwd)/build
if [[ ":$PATH:" != *":$BINARY_PATH:"* ]]; then
    echo "Adding $BINARY_PATH to PATH..."
    # Detect shell
    if [[ "$SHELL" == *"zsh"* ]]; then
        echo "export PATH=\"\$PATH:$BINARY_PATH\"" >> ~/.zshrc
        echo "Please run 'source ~/.zshrc' or restart terminal."
    else
        echo "export PATH=\"\$PATH:$BINARY_PATH\"" >> ~/.bashrc
        echo "Please run 'source ~/.bashrc' or restart terminal."
    fi
else
    echo "dispatch is already in PATH."
fi

echo "Setup complete! Try running: dispatch help"