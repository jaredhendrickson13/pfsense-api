#!/usr/bin/python3
# Copyright [2020] [Jared Hendrickson]
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

import os
import sys
import pathlib
import jinja2
import platform
import subprocess
import getpass
import argparse

class MakePackage:
    def __init__(self):
        self.__start_argparse__()
        self.port_version = ".".join(self.args.tag.split(".")[0:2])
        self.port_revision = self.args.tag.split(".")[2]

        # Run tasks for build mode
        if self.args.build:
            self.build_on_remote_host()
        else:
            self.generate_makefile()

    # Custom filter for Jinja2 to determine the directory of a given file path
    def dirname(self, path):
        return os.path.dirname(path)

    def generate_makefile(self):
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

        self.build_package(pkg_dir)

    def run_ssh_cmd(self, cmd):
        ssh_cmd = "ssh {u}@{h} '{c}'".format(u=self.args.username, h=self.args.host, c=cmd)
        return subprocess.call(ssh_cmd, shell=True)

    def run_scp_cmd(self, src, dst, recurse=False):
        scp_cmd = "scp {r} {s} {d}".format(r="-r" if recurse else "", s=src, d=dst)
        return subprocess.call(scp_cmd, shell=True)

    def build_package(self, pkg_dir):
        # If we are running on FreeBSD, make package. Otherwise display warning that package was not compiled
        if platform.system() == "FreeBSD":
            s = subprocess.call(["/usr/bin/make", "package", "-C", pkg_dir, "DISABLE_VULNERABILITIES=yes"])
        else:
            print("WARNING: System is not FreeBSD. Generated Makefile and pkg-plist but did not attempt to build pkg.")

    def build_on_remote_host(self):
        # Automate the process to pull, build and retrieve the package on a remote host
        build_cmds = [
            "mkdir -p ~/build/",
            "rm -rf ~/build/pfsense-api",
            "git clone https://github.com/jaredhendrickson13/pfsense-api.git ~/build/pfsense-api/",
            "git -C ~/build/pfsense-api checkout " + self.args.branch,
            "python3 ~/build/pfsense-api/tools/make_package.py --tag {t}".format(t=self.args.tag)
        ]

        # Join our build commands into a single command to run via SSH
        self.run_ssh_cmd(" && ".join(build_cmds))

        # Retrieve the built package
        if self.args.freebsd == 11:
            src = "{u}@{h}:~/build/pfsense-api/pfSense-pkg-API/pfSense-pkg-API/pfSense-pkg-API-{v}{r}.txz"
            src = cmd.format(
                u=self.args.username,
                h=self.args.host,
                v=self.port_version,
                r="_" + self.port_revision if self.port_revision != "0" else ""
            )
            self.run_scp_cmd(src, ".")
        else:
            src = "{u}@{h}:~/build/pfsense-api/pfSense-pkg-API/pfSense-pkg-API/work/pkg/pfSense-pkg-API-{v}{r}.txz"
            src = src.format(
                u=self.args.username,
                h=self.args.host,
                v=self.port_version,
                r="_" + self.port_revision if self.port_revision != "0" else ""
            )
            self.run_scp_cmd(src, ".")

    def __start_argparse__(self):
        # Custom port type for argparse
        def tag(value_string):
            value = str(value_string).split(".")
            valid = True

            # Require 3 items (M.m.p)
            if len(value) == 3:
                for i in value:
                    # Value cannot be blank
                    if i == "":
                        valid = False
            else:
                valid = False

            # Return value if valid, otherwise throw error
            if valid:
                return value_string
            else:
                raise argparse.ArgumentTypeError("%s is not a semantic version tag" % value_string)

        parser = argparse.ArgumentParser(
            description="Build the pfSense API on FreeBSD"
        )
        parser.add_argument(
            '--host', '-i',
            dest="host",
            type=str,
            required=True if "--remote" in sys.argv or "-r" in sys.argv else False,
            help="The host to connect to when using --build mode"
        )
        parser.add_argument(
            '--freebsd', '-f',
            dest="freebsd",
            type=int,
            default=12,
            help="The version of FreeBSD running on the remote host"
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
            '--remote', '-r',
            dest="build",
            action="store_true",
            required=False,
            help='Enable remote build mode'
        )
        self.args = parser.parse_args()

try:
    MakePackage()
except KeyboardInterrupt:
    sys.exit(1)
