#!/usr/bin/env bash
cd "$(dirname "$0")/.."

# Get the absolute path to the workspace
WORKSPACE_PATH=$(pwd)

# URL encode the configuration name if needed
CONFIG_NAME="Debug%20Game"

# Construct the VS Code URI to automatically trigger the debug session
# format: vscode://vscode.env/debug?folder=<path>&id=<config-name>
URI="vscode://vscode.env/debug?folder=${WORKSPACE_PATH}&id=${CONFIG_NAME}"

# Open the URI using VS Code's URL handler
code --open-url "$URI" 2>/dev/null || xdg-open "$URI"
