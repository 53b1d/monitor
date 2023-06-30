#!/bin/bash
machine_name=$(uname -n)
echo "device: $machine_name"
kernel_version=$(uname -v)
echo "kernel: $kernel_version"
kernel_release=$(uname -r)
echo "kernel release: $kernel_release"
hardware_architecture=$(uname -m)
echo "hardware architecture: $hardware_architecture"
user=$(whoami)
echo "User: $user"