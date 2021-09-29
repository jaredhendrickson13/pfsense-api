# Copyright 2021 Jared Hendrickson
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

import argparse
import pathlib
import os
import subprocess

# Variables
args = None
php = """<?php
require_once("api/framework/APITools.inc");
session_start();

$user_privs = get_user_privileges(getUserEntry($_SESSION["Username"]));
# Redirect user if they do not have privilege to access this page
if (!in_array("page-system-api", $user_privs) and !in_array("page-all", $user_privs)) {
    header("Location: /");
    exit();
}
?>
"""
css = """
<style>
    title {
        content:"pfSense API Documentation";
    }
    body {
        display: block;
        width: 90%;
        max-width: 1700px;
        margin: 0 auto;
    }
    footer {
        text-align: center;
    }
    footer p {
        margin: 2px;
    }
    div.col-md-12 {
        margin-top: 20px;
    }
    
    li:before {
        content:"• ";
    }
    
    summary:hover {
        cursor: pointer;
        color: darkred;
    }
    
    details[open] summary ~ * {
        animation: open 0.3s ease-in-out;
    }
    
    @keyframes open {
        0% {
            opacity: 0;
        }
        100% {
            opacity: 1;
        }
    }
    
    details summary::-webkit-details-marker {
        display: none;
    }
    
    details summary {
        width: 100%;
        padding: 0.5rem 0;
        border-top: 1px solid black;
        position: relative;
        cursor: pointer;
        font-size: 14px;
        font-weight: 500;
        list-style: none;
    }
    
    details summary:before {
        content: "+ ";
        color: black;
        position: relative;
        font-size: 12px;
        line-height: 0;
        margin-top: 0.75rem;
        font-weight: 400;
        transform-origin: center;
        transition: 200ms linear;
    }
    
    details[open] summary:before {
        content: "- ";
        font-size: 12px;
    }
    
    details summary {
        outline: 0;
    }
    
    details p {
        font-size: 14px;
        margin: 0 0 1rem;
        padding-top: 1rem;
    }
    
    strong.border-success, strong.border-warning, strong.border-info, strong.border-danger {
        border-radius: 5px;
        padding: 2px;
    }
    </style>
"""
js = """
<script type="text/javascript">
    $(document).ready(function() {
        document.title = 'pfSense REST API Documentation';
        var curr_year = new Date().getFullYear();
        document.querySelector('footer').innerHTML = "<a href='/api/'>Back to pfSense</a> | <a href='https://github.com/jaredhendrickson13/pfsense-api'>View on Github</a> | <a href='https://github.com/jaredhendrickson13/pfsense-api/issues/new'>Report an Issue</a><p>Copyright &copy; " + curr_year + " - Jared Hendrickson</p>";
    });

</script>
"""

# Parses command line arguments and generates help pages
def start_argparse():
    global args
    # Custom file path type for argparse
    def fp(value_string):
        if not pathlib.Path(value_string).exists():
            raise argparse.ArgumentTypeError("No file found at %s" % value_string)
        return value_string

    parser = argparse.ArgumentParser(description="Converts Postman collection to embedded API documentation")
    parser.add_argument('--json', dest="json", type=fp, required=True, help="Path to the Postman collection JSON file")
    args = parser.parse_args()

# Runs our docgen command to convert the Postman collection to HTML and Markdown
def run_docgen():
    docgen_html = subprocess.check_output(
        ["docgen", "build", "-i", args.json, "-o", "documentation.html"],
        stderr=subprocess.STDOUT
    )
    docgen_md = subprocess.check_output(
        ["docgen", "build", "-i", args.json, "-o", "documentation.md", "-m"],
        stderr=subprocess.STDOUT
    )
    print(docgen_html.decode().replace("html", "php").replace("\n", ""))
    print(docgen_md.decode().replace("\n", ""))

# Formats the docs docgen created to add custom CSS and other content overrides
def format_docs():
    # Read the contents of the HTML document docgen created, then delete the file
    with open("documentation.html", "r") as dr:
        html_doc = dr.read()
    os.remove("documentation.html")

    # Write a new PHP script containing the HTML created by docgen, then override custom attributes and CSS
    with open("documentation.php", "w") as dw:
        html_doc = php + html_doc
        html_doc = html_doc + js + css
        html_doc = html_doc.replace("{{$hostname}}", "<?echo $_SERVER['HTTP_HOST'];?>")
        html_doc = html_doc.replace("<h5 class=\"label label-info\">Query</h5>", "<h5 class=\"label label-info\">Fields</h5>")
        html_doc = html_doc.replace("<small class=\"pull-right text-muted\">raw</small>", "<small class=\"pull-right text-muted\"></small>")
        html_doc = html_doc.replace("<h5 class=\"label label-primary\">Body</h5>", "<h5 class=\"label label-primary\">Example Request</h5>")
        html_doc = html_doc.replace("<th>Value<th>", "<th>Type<th>")
        dw.write(html_doc)

    # Read the contents of the Markdown document docgen created, then delete the file
        with open("documentation.md", "r") as mr:
            md_doc = mr.read()
        os.remove("documentation.md")

    # Write a new Markdown document containing the Markdown created by docgen, then add custom attributes
        with open("documentation.md", "w") as mw:
            md_doc = md_doc.replace("Type: RAW\n", "")
            md_doc = md_doc.replace("***Query params:***", "***Fields:***")
            md_doc = md_doc.replace("***Body:***", "***Example Request:***")
            md_doc = md_doc.replace("| Value |", "| Type |")
            md_doc = "\n".join(md_doc.split("\n")[:-2] + [""])
            mw.write(md_doc)

# Moves the docs to their appropriate location
def move_docs():
    php_path = pathlib.Path("documentation.php").absolute()
    md_path = pathlib.Path("documentation.md").absolute()
    root_dir = pathlib.Path(__file__).absolute().parent.parent
    os.rename(md_path, root_dir.joinpath("README.md"))
    os.rename(php_path, root_dir.joinpath("pfSense-pkg-API/files/usr/local/www/api/documentation/index.php"))

# RUN TIME
start_argparse()
run_docgen()
format_docs()
move_docs()
