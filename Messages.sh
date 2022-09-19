#! /usr/bin/env bash

# SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Extract for Qt Linguist
$EXTRACT_TR_STRINGS `find src/ -name \*.cpp -o -name \*.h` -o $podir/ghostwriter_qt.pot