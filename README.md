![Dispatch Discord Notification](images/dispatch_logo.png)

# Dispatch

**Dispatch** is an easy-to-use notification CLI tool for executing terminal commands and automatically broadcasting the standard output, execution time, and exit status directly to your Discord or Slack channels via Webhooks.

It's designed to streamline long-running scripts (like ML training, compiling, or deployments) by letting you know exactly when your process finishes and sending the relevant logs or generated artifacts straight to your phone or desktop.

## Features
- 🚀 **Zero-Friction Notifications**: Wrap any arbitrary terminal command inside `dispatch "..."` to run it and beam the result to chat.
- ⏱️ **Execution Profiles**: Automatically tracks how long the task took and whether it succeeded/failed based on standard POSIX exit codes.
- 📄 **File Attachments**: Did your script generate a graph or a log? Use `--output-files` to upload them natively to Discord or Slack.
- ⚠️ **Large Output Handling**: Discord/Slack restrict message sizes (e.g. 2000 chars limit). If your command produces massive stdout, Dispatch automatically truncates the messy text and safely uploads the full unedited console output as an attached `output.txt` file instead!
- 🔀 **Multi-Channel Support**: Save, toggle, and broadcast identical output to an unlimited number of different Discord Server or Slack channels simultaneously through its config profiles.

## Installation
Dispatch provides an automated installation script for **macOS** and **Linux (Debian/Ubuntu)**. It requires CMake and libcurl. 

```bash
git clone https://github.com/tomeravgil/dispatch.git
cd dispatch
./setup.sh
```

The setup script will compile the project and install the `dispatch` binary locally to `~/.local/bin/dispatch`. If `~/.local/bin` isn't already on your system `$PATH`, the script places the export logic into your `~/.zshrc` or `~/.bashrc` automatically. 

After running the script, restart your terminal or `source ~/.zshrc`.

## Configuration
Before running any commands, you must configure at least one Webhook URL for Dispatch to push data to. You can easily generate a webhook URL through your Discord Server's or Slack Workspace's integration settings.

```bash
# Display the interactive help menu
dispatch config help

# Add a new webhook profile (Use 'discord' or 'slack' somewhere in the name so Dispatch knows how to format it!)
dispatch config add my_discord_server https://discord.com/api/webhooks/...
dispatch config add team_slack_chat https://hooks.slack.com/services/...

# Select which webhooks you want to broadcast to 
dispatch config select my_discord_server

# List all selected/active webhooks
dispatch config list-selected
```

## Usage
Simply prefix your long-running or volatile terminal commands with `dispatch` and encase your command in quotes.

### Standard Commands
```bash
dispatch "npm run build"
dispatch "python3 train_ml_model.py"
dispatch "make clean && make all"
```
Dispatch will synchronously wait, capture standard output stream, measure the time, and push the results!

### Attaching Files
If your script produces log files, images, or data you'd like forwarded, simply append `--output-files` at the end followed by the requested local files.

```bash
dispatch "python3 generate_charts.py" --output-files chart1.png chart2.png
dispatch "pytest" --output-files test_results.xml
```

## Testing & CI 
This repository contains a full C++ unit testing suite built with **GoogleTest (GTest)**. The tests simulate complex logic states like webhook profile isolation, Discord multi-part file payloads, Slack message JSON escaping, and file truncation boundaries. 

To run the tests locally:
```bash
cd build
cmake ..
make dispatch_test
./dispatch_test
```

This project includes a **GitHub Actions CI/CD Pipeline**. The test suite is automatically executed independently on both `ubuntu-latest` and `macos-latest` runners whenever new code is merged into the master branch.

## Troubleshooting
libcurl not found: If compilation fails, ensure you have the development headers installed:

Ubuntu/Debian: sudo apt-get install libcurl4-openssl-dev

macOS: brew install curl

Webhook 404/401: Double-check your Webhook URL. Ensure you haven't deleted the Webhook in your Discord/Slack settings.

Command not found: Ensure ~/.local/bin is in your $PATH. Run echo $PATH to verify.

## Security & Privacy
Webhook Safety: Your Webhook URLs are stored locally in ~/.dispatch/config. Never share this file or check it into Version Control (Git).

Data Handling: Dispatch only sends data to the URLs you specify. It does not store your logs on any external servers other than the ones provided by your Webhook providers.

## Contributing
Contributions are welcome! If you have a feature request or found a bug:

Open an Issue to discuss the change.

Submit a Pull Request with your proposed fix.

Ensure all C++ tests pass by running ./dispatch_test.