#!/bin/bash

cd build

list_gists() { 
    ./src/gist -l
}

create_gist() {
    ./src/gist << EOF 
    TestFile.txt
    Example json file for use in GitHub REST API
    This is a test file.
EOF
}

update_gist_fname() {
    ./src/gist -u fname "TestFile.txt" << EOF
    Updated json file for GitHub Gists
    This is an updated test file.
EOF
}

update_gist_id() {
    ./src/gist -u "id" "7d860cf5e09b3534c31f62b0ba2d88d7" << EOF
    Updated json file for GitHub Gists
    This is an updated test file.
EOF
}

list_gists
create_gist
update_gist_fname
update_gist_id

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
