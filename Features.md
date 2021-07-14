# Features

## TODO

### Multiple Files

Implement support for multiple files:

- Create multiple gists from multiple files
- Delete multiple gists with id specifiers
    - Create -p flag to make gist private       `gist -p a.rb`
    - Create -d flag to set gist description    `gist -d "Random rbx bug" a.rb`
    - Create -f flag to set custom filename
        - Ability to read contents from STDIN `gist -f test.rb < a.rb`
- Read contents directly from files instead specifying it via cli

  Ability to specify -c to create new gist from command line with prompts

### Gist Search

Implement `gist search` for:

- Filename
- ID
- Creation Date
- Last modified date
- Public/Private

  Create user input prompts for STDIN.

### Feature Parity with Ruby Gists

Implement:

- Gist login with                   `gist --login`
- Specify remote file type with     `gist -t, --type`
- Update with                       `gist -u [ URL | ID ]`
- Display raw gist url with         `gist -R, --raw`
- List all gists with               `gist -l, --list [USER]`
- Read gist into stdout with        `gist -r, --read [FILENAME]`
- Gist deletion with                `gist --delete [ URL | ID ]`

### Misc

Miscellaneous Features:

- Create Gist Class with gist configurations
    - Pass gist object around to send requests
- Create unit tests with doctest
    - Mock Gists REST API with seznam/httpmockserver
- Create project docs with pmarsceill/just-the-docs
- Show binary version with --version
- Show verbose output with --verbose
