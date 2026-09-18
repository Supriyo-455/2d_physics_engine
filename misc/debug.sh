#!/usr/bin/env bash

# Extract project name from 4coder config, fallback to a default if missing
PROJECT_NAME=$(awk -F'"' '/project_name/ {print $2}' project.4coder 2>/dev/null)
PROJECT_NAME="${PROJECT_NAME:-2d_phyics}"

qtcreator "${PROJECT_NAME}.creator" &