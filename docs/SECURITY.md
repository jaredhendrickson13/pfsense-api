# Security Policy

Security is a top priority for this project. The REST API package is designed to be secure and provide granular controls
to help admins implement a multi-layered security approach to API access. This document outlines the security policy 
for the project and provides information on how to report a security vulnerability.

## Supported Versions

Currently, there are two supported versions of the package: the v2 package (pfSense-pkg-RESTAPI) and the legacy v1 
package (pfSense-pkg-API). The v2 package is the latest version of the package and is actively developed and fully 
maintained. The legacy v1 package is no longer actively developed and is only receiving compatibility fixes and critical
security updates when necessary. It is highly recommended to regularly update to the latest version of the package to 
ensure you are receiving important bug fixes and security updates.

## Reporting a Vulnerability

Should you discover a vulnerability in the codebase, please report the issue using the order of preference below:

1. A pull request with code that fixes the discovered vulnerability
2. A private email to either the project owner or the respective code owner
3. As a last resort, you may open a public issue on the repository

Please do not disclose the details of the vulnerability publicly until it has been addressed by the maintainers. 
The maintainers will work to address the vulnerability as quickly as possible and will provide updates on the issue 
as they become available.

!!! Notice
    This is an open-source project and is developed and maintained by members of the pfSense community. No bug bounty
    can be offered for reported vulnerabilities.
