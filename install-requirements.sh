#! /usr/bin/env bash

echo "Installing.."
while read p; do ./vcpkg/vcpkg install "$p"; done <requirements.txt
