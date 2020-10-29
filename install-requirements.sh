#! /usr/bin/env bash

echo "Installing.."
while read p; do ./vcpkg/vcpkg.exe install "$p"; done <requirements.txt
