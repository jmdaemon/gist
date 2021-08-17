#!/bin/bash

update_id="96827011f3a14a3e4bd7c2a7779bab9f"
delete_id="7c5d92adac82b97705d6c25fa2e766ee"
bin="./build/src/gist"
testfile="TestFile.txt"

list_gists() { 
    $bin -l
}

new_gist() {
    $bin << EOF 
    $testfile
    Example json file for use in GitHub REST API
    This is a test file.
EOF
}

create_gist() {
    $bin 
}

update_gist_id() {
    $bin -u $update_id -d "Updated json file for GitHub Gists" $testfile
}

update_gist_url() {
    $bin -u "https://gist.github.com/jmdaemon/${update_id}" -d "Updated json file for GitHub Gists" $testfile
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
#update_gist_id
#update_gist_url
#delete_gist
#show_version
#$bin --help

## Search
#$bin search -i "$update_id"
$bin search -i "$update_id" -r
#$bin search -f "$testfile"
#$bin search -c "2021-08-13"
#$bin search -c "2021-07-14"
#$bin search -d "2021-08-16"

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));

cd ..
