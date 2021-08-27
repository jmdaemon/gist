#!/bin/bash

update_id="7225bff51156a73315cfff557bce8608"
delete_id="7985b97c4570c8897a653b190f632980"
bin="./build/src/gist"
testfile="TestFile.txt"

new_gist() {
    $bin << EOF 
    $testfile
    Example json file for use in GitHub REST API
    This is a test file.
EOF
}

test_gist() {
    msg=$1
    cmd=$2
    echo "$msg"
    echo "================"
    echo "Executing Command: $cmd"
    eval $cmd
    echo ""
}

test_gist_bin() {
    test_gist "List all user gists" "$bin -l"
    test_gist "Create new gist" "$bin"
    #new_gist
    test_gist "Update user gist by ID" "$bin -u $update_id -d \"Updated json file for GitHub Gists\" $testfile"
    test_gist "Update user gist by URL" "$bin -u \"https://gist.github.com/jmdaemon/${update_id}\" -d \"Updated json file for GitHub Gists\" $testfile"
    test_gist "Delete user gist by ID" "$bin -D $delete_id"
    test_gist "Show project version" "$bin -v"
    test_gist "Show help message" "$bin --help"
}

test_gist_search() {
    test_gist "Search ID" "$bin search -i "$update_id""
    test_gist "Search ID Raw" "$bin search -i "$update_id" -r"
    test_gist "Search ID Filename" "$bin search -f "$testfile""
    test_gist "Search ID Creation Date 1" "$bin search -c "2021-08-13""
    test_gist "Search ID Creation Date 2" "$bin search -c "2021-07-14""
    test_gist "Search ID Creation Date 1 Raw" "$bin search -c "2021-08-13" -r"
    test_gist "Search ID Creation Date 2 Raw" "$bin search -c "2021-07-14" -r"
    test_gist "Search ID Last Modified" "$bin search -d "2021-08-16""
}

# Gist Tests
#test_gist_bin

# Gist Search Tests
test_gist_search

# fmt::print("Aur   URL : {}\n", getRaw(o, "aur-list.pkg"));
# fmt::print("Arch  URL : {}\n", getRaw(o, "pacman-list.pkg"));
# fmt::print("Aur   ID  : {}\n", getId(o, "aur-list.pkg"));
# fmt::print("Arch  ID  : {}\n", getId(o, "pacman-list.pkg"));
