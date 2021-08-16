#!/bin/bash

update_id="96827011f3a14a3e4bd7c2a7779bab9f"
delete_id="7c5d92adac82b97705d6c25fa2e766ee"
bin="./build/src/gist"

list_gists() { 
    $bin -l
}

new_gist() {
    $bin << EOF 
    TestFile.txt
    Example json file for use in GitHub REST API
    This is a test file.
EOF
}

create_gist() {
    $bin TestFile.txt 
}

update_gist_fname() {
    $bin -u fname "TestFile.txt" << EOF
    Updated json file for GitHub Gists
    This is an updated test file.
EOF
}

update_gist_id() {
    $bin -u $update_id -d "Updated json file for GitHub Gists" TestFile.txt
    #$bin -u $update_id -d "Updated json file for GitHub Gists" << EOF
    #This is an updated test file.
#EOF
}
update_gist_url() {
    $bin -u "https://gist.github.com/jmdaemon/${update_id}" -d "Updated json file for GitHub Gists" TestFile.txt
}

delete_gist() {
    $bin -D $delete_id
}

show_version() {
    $bin -v
}

#list_gists
#create_gist
#new_gist
#update_gist_fname
#update_gist_id
update_gist_url
#delete_gist
#show_version
#$bin --help

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
