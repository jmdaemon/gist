#!/bin/bash

cd build
./src/gist -l

./src/gist -c << EOF 
TestFile.txt
Example json file for use in GitHub REST API
This is a test file.
EOF
./src/gist -u << EOF
TestFile.txt
Updated json file for GitHub Gists
This is an updated test file.
EOF

cd ..
