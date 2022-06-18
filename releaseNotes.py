#!/bin/python

import sys

if len(sys.argv) != 2:
    print("Missing or invalid arguments.")
    print("Please provide release version number.")
    exit(1)

version = sys.argv[1]

changelog = open('CHANGELOG.md', 'r')
lines = changelog.readlines()

currentRelease = False

print("## Release Notes")

for line in lines:
    if not currentRelease and line.startswith("## ["):
        currentRelease = True
    elif currentRelease:
        if line.startswith("## ["):
            break # All done!
        else:
            print(line.rstrip())

print("## Downloads")
print("")
print(r'[![ubuntu](https://img.shields.io/static/v1?label=&color=purple&logo=ubuntu&message=Ubuntu&style=for-the-badge)](https://launchpad.net/~wereturtle/+archive/ubuntu/ppa) [![fedora](https://img.shields.io/static/v1?label=&color=blue&logo=fedora&message=Fedora&style=for-the-badge)](https://copr.fedorainfracloud.org/coprs/wereturtle/stable/)  [![Windows](https://img.shields.io/static/v1?label=&color=blue&logo=windows&message=Windows&style=for-the-badge)](https://github.com/wereturtle/ghostwriter/releases/download/' + version + r'/ghostwriter_' + version + r'_win64_portable.zip)')

