#!/bin/bash

get_cpu_info() {
    echo "CPU Information:"
    echo "$(lscpu)"
    echo
}

get_memory_info() {
    echo "Memory Information:"
    echo "$(free -h)"
    echo
}

get_disk_info() {
    echo "Disk Information:"
    echo "$(df -h)"
    echo
}

get_network_info() {
    echo "Network Interface Information:"
    echo "$(ifconfig)"
    echo
}

get_gpu_info() {
    echo "GPU Information:"
    echo "$(lspci | grep -i 'vga\|3d\|display')"
    echo
}

print_hardware_details() {
    get_cpu_info
    get_memory_info
    get_disk_info
    get_network_info
    get_gpu_info
}

print_hardware_details
