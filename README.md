# Gist

A Github gists cli tool

1. [Usage](#usage)
2. [Build](#build)
3. [Install](#install)
4. [Uninstall](#uninstall)
5. [Configure](#configure)
6. [Contribute](#contribute)
7. [Potential Features](#features)

## Usage

``` bash
# Lists out gist json response
gist list

# Create new gist from STDIN
gist new
# Filename: helloworld.c
# Contents:
# int main () {
#     puts("Hello World!");
# }
# Description: Hello world c example
# Make Private? [y/n]: n

# Create new gist from cli arguments
gist create -f "filename" -d "desc" -p # Create private gist

# Delete gists with matching id
gist delete -i "[id]"

# or by matching file name
gist delete name -f asdf

# or by date timestamps
gist delete date -c "2022-08-23T21:37:26Z" -t exact # Choices: [exact, before, after]

# Update gist information
gist update -i "[id]" -f aaaa -d "Revised Description"
gist update -i "[id]" -f aaaa -n asdf # Rename gist file name to asdf

# Search gists for matching id
gist search id -i "[id]"

# or by file name
gist search name -f asdf

# or by date
gist search date -c "2022-08-23T21:37:26Z" -t after
```

For more information, see `gist -h`

## Build

To build with `ninja`:

``` bash
cmake --preset gcc-release-ninja
cd build/gcc-release-ninja
ninja
```

To build with `make`:

``` bash
cmake --preset gcc-release-unix-makefiles
cd build/gcc-release-unix-makefiles
make
```

## Install
To install to `/usr/local/`:

``` bash
sudo cmake --install .
```

If you want to install to `/usr` instead:

``` bash
sudo cmake --install --prefix /usr
```

## Uninstall

To uninstall:

``` bash
cmake uninstall .
sudo make uninstall
# or
sudo ninja uninstall
```

## Configure

If you want to use a different file/path for configuring gist, either:

1. Set the `GIST_CONFIG_HOME` variable to your config file path.
2. Use the `-g` option and specify the full path to your config.

An example config file is provided in the repository.

## Contribute

Contributions are welcome. Feel free to use the issues page to report issues, or suggest features.

## Features

More potential features that would make this application really killer:
- CLI option to receive gist content as output.
- Manpage to view arguments/options and commands.
- Doctest unit tests & automated testing.
- Better documentation
- Automated releases
