#!/bin/bash

USERS=$(cut -d: -f1 /etc/passwd)

for USER in $USERS; do
    echo "Username: $USER"

    HOME_DIR=$(eval echo ~$USER)
    echo "Home Directory: $HOME_DIR"

    SHELL=$(grep "^$USER:" /etc/passwd | cut -d: -f7)
    echo "Default Shell: $SHELL"

    USER_UID=$(id -u $USER)
    USER_GID=$(id -g $USER)
    echo "UID: $USER_UID"
    echo "GID: $USER_GID"

    echo
done