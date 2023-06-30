#!/bin/bash

get_ssh_connections() {
    local ssh_connections=$(netstat -tnpa | grep 'ESTABLISHED.*sshd' | awk '{print $5}')
    echo "SSH Connections:"
    echo "$ssh_connections"
}

get_ssh_connections