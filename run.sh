#!/bin/bash

cd build

update_id="7c5d92adac82b97705d6c25fa2e766ee"
delete_id="8c118afbf19f2d035e04e8a6a87e66f8"

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
    ./src/gist -u "id" $update_id << EOF
    Updated json file for GitHub Gists
    This is an updated test file.
EOF
}

delete_gist() {
    ./src/gist -D $delete_id
}

show_version() {
    ./src/gist -v
}
show_version

#list_gists
#create_gist
#update_gist_fname
#update_gist_id
#delete_gist

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
