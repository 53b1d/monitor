#!/bin/bash

if [ -z "$1" ]; then
    echo "provide a service name as a parameter."
    exit 1
fi

start_service() {
    local service_name=$1
    echo "Starting service: $service_name"
    
    if systemctl start "$service_name"; then
        echo "Service started successfully."
    else
        echo "Failed to start service: $service_name"
    fi
}

start_service "$1"
