# Tools

This directory includes scripts and files to aid the development of this project. Below are some basic overviews of
the scripts included.

## MAKE_PACKAGE.PY

This script is designed to automatically generate our FreeBSD package Makefile and pkg-plist. This also attempts to
compile the package automatically if you run it on a FreeBSD system. Files are rendered using Jinja2 and templates are
found in the `templates` subdirectory.

### Usage

To build in-place on FreeBSD:
`python3 tools/make_package.py --branch <BRANCH NAME TO TARGET> --tag <VERSION TO ASSIGN PACKAGE>`

To build on a remote FreeBSD host using SSH/SCP:
`python3 tools/make_package --host <IP or HOSTNAME OF FREEBSD> --branch <BRANCH NAME TO TARGET> --tag <VERSION TO ASSIGN PACKAGE>`

For example:
`python3 tools/make_package --host 192.168.1.25 --branch master --tag 0.0.4
`

### Dependencies

- `Jinja2` package must be installed before running (`python3 -m pip install jinja2`)
- `php80-composer2` FreeBSD package must be installed before running (`sudo pkg install php80-composer2`)

### Output

Command will output the FreeBSD make command output. Outputs the following files:

- `pfsense-restapi/pfSense-pkg-RESTAPI/Makefile` : The rendered Makefile
- `pfsense-restapi/pfSense-pkg-RESTAPI/pkg-plist`: The rendered pkg-plist
- `pfsense-restapi/pfSense-pkg-RESTAPI/work/pkg/pfSense-pkg-RESTAPI-<VERSION>.pkg` : The built FreeBSD package distribution file.

If you have run the script using the `--host` flag, the built package will be copied to your local system using SCP.

### Notes

- This script heavily depends on its relative file paths. You may execute the script from any directory, but do not move
  the script to another directory.
- When utilized with the `--host` flag to execute a remote build, it is required to have SSH access to the remote FreeBSD
  instance in order to build and fetch package.
