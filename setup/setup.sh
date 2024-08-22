#!/bin/bash

# Update package list
sudo apt update

# Install necessary packages and dependencies
sudo apt install -y build-essential g++-13 libjsoncpp-dev

echo "All dependencies have been installed!"