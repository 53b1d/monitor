#!/bin/bash
get_cpu_temperature() {
    if [[ -f "/sys/class/thermal/thermal_zone0/temp" ]]; then
        local temp=$(cat /sys/class/thermal/thermal_zone0/temp)
        echo "CPU Temperature: $((temp/1000))°C"
    else
        echo "CPU temperature information not available."
    fi
}

get_ram_usage() {
    local ram_usage=$(free -m | awk 'NR==2{printf "%.2f%%", ($3/$2)*100}')
    echo "RAM Usage: $ram_usage"
}

get_cpu_usage() {
    local cpu_usage=$(top -bn1 | grep load | awk '{printf "%.2f%%", $(NF-2)}')
    echo "CPU Usage: $cpu_usage"
}

get_io_usage() {
    local io_usage=$(iostat -d -y | awk '/sda/ {print $3}')
    echo "I/O Usage: $io_usage"
}

get_cpu_temperature
get_ram_usage
get_cpu_usage
get_io_usage
