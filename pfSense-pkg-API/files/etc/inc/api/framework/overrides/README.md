File Overrides
==============
This directory contains files that intend to override an existing pfSense file. In specific cases, the existing pfSense
code must be changed to enable the API to work as intended. A file override is used to do this. File overrides are the 
same exact file that exists on pfSense originally with the extended code as required. File overrides are specific to 
the version of pfSense running. In the case that a file override does not exist for your version of pfSense, the 
default file override will be used (typically the override from the newest pfSense version). A warning will be displayed
when the API is installed using a default override. Uninstalling the API package will restore the files to their original
state. Below are a description of file overrides required by the package:

#### SYSTEM.INC
This file will override the existing `/etc/inc/system.inc` file to extend the capabilities of pfSense's NGINX web server.
This is required for the API to support endpoint URLs without a trailing slash as well as alternate request methods like
`PUT` and `DELETE`.