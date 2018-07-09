#!/bin/bash

# macdeploy_helper.sh takes 2 arguments.
# First argument is destination path.
# Second argument is source path.

mkdir -pv $1
find $2 -maxdepth 1 -exec cp -v '{}' $1 \;

# update qt.conf to point to our translations path
printf "[Paths]\nPlugins = Plugins\nTranslations = Resources/translations\n" > $1/../qt.conf

