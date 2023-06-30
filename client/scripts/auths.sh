#!/bin/bash

LOG_FILE="/var/log/auth.log"

if [ ! -f "$LOG_FILE" ]; then
    echo "Authentication log file not found: $LOG_FILE"
    exit 1
fi

AUTH_ERRORS=$(grep -i "authentication failure" "$LOG_FILE")

if [ -z "$AUTH_ERRORS" ]; then
    echo "No authentication errors found in the log file."
else
    echo "Authentication errors found:"
    echo "$AUTH_ERRORS"
fi