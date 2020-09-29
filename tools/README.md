Tools
=====
This directory includes scripts and files to aide the development of this project. Below are some basic overviews of 
the scripts included.

## MAKE_PACKAGE.PY
This script is designed to automatically generate our FreeBSD package Makefile and pkg-plist. This also attempts to
compile the package automatically if you run it on a FreeBSD system. Files are rendered using Jinja2 and templates are 
found in the `templates` subdirectory

### Usage
`python3 tools/make_package.py`

### Dependencies
- `Jinja2` package must be installed before running (`python3 -m pip install jinja2`)

### Output
Command will output the FreeBSD make command output. Outputs the following files:

- `pfsense-api/pfSense-pkg-API/Makefile` : The rendered Makefile
- `pfsense-api/pfSense-pkg-API/pkg-plist`: The rendered pkg-plist
- `pfsense-api/pfSense-pkg-API/pfSense-pkg-API-<VERSION>.txz` : The FreeBSD package distribution file. On FreeBSD 11, 
this should be located in the `pfsense-api/pfSense-pkg-API` directory after completion. On FreeBSD 12 it should be 
located in the `pfsense-api/pfSense-pkg-API/work/pkg` directory.

### Notes
- This script heavily depends on it's relative filepaths. You may execute the script from any directory, but do not move
the script to another directory.

## MAKE_DOCUMENTATION.PY
This script is designed to automatically generate our API documentation. This takes a Postman Collection JSON file and
converts to HTML and Markdown using a tool called `docgen`. This script then formats the HTML document into a custom 
PHP script that will allow users to easily access API documentation within the webConfigurator. After generating and
formatting documentation, this script will move the files to their correct location.

### Usage
`python3 tools/make_documentation.py --json <PATH TO POSTMAN JSON FILE>`

### Dependencies
- `docgen` : DocGen must be installed and executable from your command line (https://github.com/thedevsaddam/docgen)

### Output
Command will print a brief status of each file and if successful, documents will be found in

- `pfsense-api/README.md`
- `pfsense-api/pfSense-pkg-API/files/usr/local/ww/api/documentation/index.php`

### Notes
- This script heavily depends on it's relative filepaths. You may execute the script from any directory, but do not move
the script to another directory.