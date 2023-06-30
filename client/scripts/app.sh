#!/bin/bash
if [ -z "$1" ]; then
    echo "provide an app name as a parameter."
    exit 1
fi

download_and_install_app() {
    local app_name=$1
    echo "Installing app: $app_name"

    sudo apt update

    if sudo apt install -y "$app_name"; then
        echo "App installed successfully."
    else
        echo "Failed to install app: $app_name"
    fi
}
download_and_install_app "$1"
