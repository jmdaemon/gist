#!/bin/bash

# Checklist
# Ensure the binary can run these commands
gist -v list
gist -v new
gist -v create -f "asdf"
gist -v create -p -f aaaa -d bbbb
gist -v delete -i "[id]"
gist -v update -i "[fname]" -f aaaa -n asdf
