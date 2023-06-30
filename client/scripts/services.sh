#!/bin/bash

get_active_services() {
    local services=$(systemctl list-units --type=service --state=running --no-pager --plain)
    echo "Active Services:"
    echo "$services"
}

get_active_services
