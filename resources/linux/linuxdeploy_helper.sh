#!/bin/bash
tool_name="linuxdeployqt-continuous-x86_64.AppImage"

output="${1}/ghostwriter.appImage"

if [ ! -f "${1}/${tool_name}" ]
then
    echo "please download ${tool_name} on ${1} and run this again"
    exit 1
else
    echo "deploying appImage for dir ${2} on ${output}"

    mkdir -pv "$2"
    pushd "$2"
    cp -pv ../ghostwriter .
    cp -pRv ../translations .
    pushd "$1"
    chmod +x "${1}/${tool_name}"
    ./"${tool_name}" "${2}/ghostwriter" "-appimage"
fi
