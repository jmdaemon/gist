#!/bin/bash

cd build
./src/gist -l
./src/gist -c << EOF 
TestFile.txt
Example json file for use in GitHub REST API
This is a test file.
EOF
./src/gist -u fname << EOF
TestFile.txt
Updated json file for GitHub Gists
This is an updated test file.
EOF
./src/gist -u "id" << EOF
0def81b74c970d14746262bbd36c0e07
Updated json file for GitHub Gists
This is an updated test file.
EOF
# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
