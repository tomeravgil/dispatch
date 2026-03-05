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

# 3. Install the executable
INSTALL_DIR="$HOME/.local/bin"
echo "Installing dispatch to $INSTALL_DIR..."
mkdir -p "$INSTALL_DIR"
cp build/dispatch "$INSTALL_DIR/"

# 4. Add to PATH if necessary (Permanent for Bash/Zsh)
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "Adding $INSTALL_DIR to PATH..."
    # Detect shell
    if [[ "$SHELL" == *"zsh"* ]]; then
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> ~/.zshrc
        echo "Please run 'source ~/.zshrc' or restart terminal."
    else
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> ~/.bashrc
        echo "Please run 'source ~/.bashrc' or restart terminal."
    fi
else
    echo "dispatch is ready to use!"
fi

echo "Setup complete! Try running: dispatch help"