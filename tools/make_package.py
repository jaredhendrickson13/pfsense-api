#!/usr/bin/python3
import os
import pathlib
import jinja2
import platform
import subprocess

# Functions
# Custom filter for Jinja2 to determine the directory of a given file path
def dirname(path):
    return os.path.dirname(path)

# Set filepath and file variables
root_dir = pathlib.Path(__file__).absolute().parent.parent
pkg_dir = root_dir.joinpath("pfSense-pkg-API")
template_dir = root_dir.joinpath("tools").joinpath("templates")
files_dir = pkg_dir.joinpath("files")
file_paths = {"dir": [], "file": []}
excluded_files = ["pkg-deinstall.in", "pkg-install.in", "etc", "usr"]

# Set Jijna2 environment and variables
j2_env = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath=str(template_dir)))
j2_env.filters["dirname"] = dirname
plist_template = j2_env.get_template("pkg-plist.j2")
makefile_template = j2_env.get_template("Makefile.j2")

# Loop through each of our files and directories and store them for Jinja2 to render
for root, dirs, files in os.walk(files_dir, topdown=True):
    root = pathlib.Path(str(root).replace(str(files_dir), ""))
    for dir in dirs:
        if dir not in excluded_files:
            file_paths["dir"].append(str(root.joinpath(dir)))
    for file in files:
        if file not in excluded_files:
            file_paths["file"].append(str(root.joinpath(file)))

# Generate pkg-plist file
with open(pkg_dir.joinpath("pkg-plist"), "w") as pw:
    pw.write(plist_template.render(files=file_paths))
# Generate Makefile file
with open(pkg_dir.joinpath("Makefile"), "w") as mw:
    mw.write(makefile_template.render(files=file_paths).replace("   ", "\t"))

# If we are running on FreeBSD, make package. Otherwise display warning that package was not compiled
if platform.system() == "FreeBSD":
    s = subprocess.call(["/usr/bin/make", "package", "-C", pkg_dir, "DISABLE_VULNERABILITIES=yes"])
else:
    print("WARNING: System is not FreeBSD. Generated Makefile and pkg-plist but did not attempt to make package.")
