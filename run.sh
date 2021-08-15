#!/bin/bash

cd build

update_id="444bb83f7cd318df267322d3283fe392"
delete_id="7c5d92adac82b97705d6c25fa2e766ee"

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

create_gist_fname() {
    cd ..
    ./build/src/gist TestFile.txt 
}

update_gist_fname() {
    ./src/gist -u fname "TestFile.txt" << EOF
    Updated json file for GitHub Gists
    This is an updated test file.
EOF
}

update_gist_id() {
    ./src/gist -u $update_id  -d "Updated json file for GitHub Gists" << EOF
    This is an updated test file.
EOF
}

delete_gist() {
    ./src/gist -D $delete_id
}

show_version() {
    ./src/gist -v
}

list_gists
create_gist
#create_gist_fname
#update_gist_fname
update_gist_id
delete_gist
#show_version
#./src/gist --help

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
