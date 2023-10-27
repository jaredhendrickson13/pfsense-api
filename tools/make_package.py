#!/usr/bin/python3
# Copyright 2023 Jared Hendrickson
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Script that is used to build the pfSense-pkg-API package on FreeBSD."""

import argparse
import getpass
import os
import pathlib
import platform
import subprocess
import sys
import jinja2


class MakePackage:
    """Class that groups together variables and methods required to build the pfSense-pkg-API FreeBSD package."""
    def __init__(self):
        self.__start_argparse__()
        self.port_version = self.args.tag.split("_")[0]
        self.port_revision = self.args.tag.split("_", maxsplit=1)[1]

        # Run tasks for build mode
        if self.args.host:
            self.build_on_remote_host()
        else:
            self.generate_makefile()

    def dirname(self, path):
        """Custom filter for Jinja2 to determine the directory of a given file path"""
        return os.path.dirname(path)

    def generate_makefile(self):
        """Generates the Makefile for this build."""
        # Many variables are needed for various files and filepaths
        # pylint: disable=too-many-locals

        # Set filepath and file variables
        root_dir = pathlib.Path(__file__).absolute().parent.parent
        pkg_dir = root_dir.joinpath("pfSense-pkg-API")
        template_dir = root_dir.joinpath("tools").joinpath("templates")
        files_dir = pkg_dir.joinpath("files")
        file_paths = {"dir": [], "file": [], "port_version": self.port_version, "port_revision": self.port_revision}
        excluded_files = ["pkg-deinstall.in", "pkg-install.in", "etc", "usr"]

        # Set Jijna2 environment and variables
        j2_env = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath=str(template_dir)))
        j2_env.filters["dirname"] = self.dirname
        plist_template = j2_env.get_template("pkg-plist.j2")
        makefile_template = j2_env.get_template("Makefile.j2")

        # Loop through each of our files and directories and store them for Jinja2 to render
        for root, directories, files in os.walk(files_dir, topdown=True):
            root = pathlib.Path(str(root).replace(str(files_dir), ""))
            for directory in directories:
                if directory not in excluded_files:
                    file_paths["dir"].append(str(root.joinpath(directory)))
            for file in files:
                if file not in excluded_files:
                    file_paths["file"].append(str(root.joinpath(file)))

        # Generate pkg-plist file
        with open(pkg_dir.joinpath("pkg-plist"), "w", encoding="utf-8") as pkg_plist:
            pkg_plist.write(plist_template.render(files=file_paths))
        # Generate Makefile file
        with open(pkg_dir.joinpath("Makefile"), "w", encoding="utf-8") as makefile:
            makefile.write(makefile_template.render(files=file_paths).replace("   ", "\t"))

        self.build_package(pkg_dir)

    def run_ssh_cmd(self, cmd):
        """Formats the SSH command to use when building on remote hosts."""
        ssh_cmd = f"ssh {self.args.username}@{self.args.host} '{cmd}'"
        return subprocess.call(ssh_cmd, shell=True)

    def run_scp_cmd(self, src, dst, recurse=False):
        """Formats the SCP command to use when copying over the built package."""
        scp_cmd = f"scp {'-r' if recurse else ''} {src} {dst}"
        return subprocess.call(scp_cmd, shell=True)

    def build_package(self, pkg_dir):
        """Builds the package when the local system is FreeBSD."""
        # If we are running on FreeBSD, make package. Otherwise display warning that package was not compiled
        if platform.system() == "FreeBSD":
            subprocess.call(["/usr/bin/make", "package", "-C", pkg_dir, "DISABLE_VULNERABILITIES=yes"])
        else:
            print("WARNING: System is not FreeBSD. Generated Makefile and pkg-plist but did not attempt to build pkg.")

    def build_on_remote_host(self):
        """Runs the build on a remote host using SSH."""
        # Automate the process to pull, install dependencies, build and retrieve the package on a remote host
        build_cmds = [
            "mkdir -p ~/build/",
            "rm -rf ~/build/pfsense-api",
            "git clone https://github.com/colindclare/pfsense-api.git ~/build/pfsense-api/",
            "git -C ~/build/pfsense-api checkout " + self.args.branch,
            "composer install --working-dir ~/build/pfsense-api",
            "rm -rf ~/build/pfsense-api/vendor/composer && rm ~/build/pfsense-api/vendor/autoload.php",
            "cp -r ~/build/pfsense-api/vendor/* ~/build/pfsense-api/pfSense-pkg-API/files/etc/inc/",
            f"python3 ~/build/pfsense-api/tools/make_package.py --tag {self.args.tag}"
        ]

        # Run each command and exit on bad status if failure
        for cmd in build_cmds:
            if self.run_ssh_cmd(cmd) != 0:
                print(f"Command '{cmd}' failed.")
                sys.exit(1)

        # Retrieve the built package
        src = "{u}@{h}:~/build/pfsense-api/pfSense-pkg-API/work/pkg/pfSense-pkg-API-{v}{r}.pkg"
        src = src.format(
            u=self.args.username,
            h=self.args.host,
            v=self.port_version,
            r="_" + self.port_revision if self.port_revision != "0" else ""
        )
        self.run_scp_cmd(src, f"{self.args.filename}")

    def __start_argparse__(self):
        # Custom tag type for argparse
        def tag(value_string):
            if "." not in value_string:
                raise ValueError(f"{value_string} is not a semantic version tag")

            # Remove the leading 'v' if present
            if value_string.startswith("v"):
                value_string = value_string[1:]

            # Convert the patch section to be prefixed with _ if it is prefixed with .
            if len(value_string.split(".")) == 3:
                value_string = value_string[::-1].replace(".", "_", 1)[::-1]

            return value_string

        parser = argparse.ArgumentParser(
            description="Build the pfSense API on FreeBSD"
        )
        parser.add_argument(
            '--host', '-i',
            dest="host",
            type=str,
            required=bool("--remote" in sys.argv or "-r" in sys.argv),
            help="The host to connect to when using --build mode"
        )
        parser.add_argument(
            '--branch', '-b',
            dest="branch",
            type=str,
            default="master",
            help="The branch to build"
        )
        parser.add_argument(
            '--username', '-u',
            dest="username",
            type=str,
            default=getpass.getuser(),
            help="The username to use with SSH."
        )
        parser.add_argument(
            '--tag', '-t',
            dest="tag",
            type=tag,
            required=True,
            help="The version tag to use when building."
        )
        parser.add_argument(
            '--filename', '-f',
            dest="filename",
            type=str,
            default=".",
            required=False,
            help="The filename to use for the package file."
        )
        self.args = parser.parse_args()

try:
    MakePackage()
except KeyboardInterrupt:
    sys.exit(1)
