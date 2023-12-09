"""Script used to test the /api/v1/system/package endpoint."""
import e2e_test_framework

# Constant
INSTALL_PKG_NAME = "pfSense-pkg-nmap"


class APIE2ETestSystemPackage(e2e_test_framework.APIE2ETest):
    """Class used to test the /api/v1/system/package endpoint."""
    uri = "/api/v1/system/package"

    get_privileges = ["page-all", "page-system-packagemanager-installed"]
    post_privileges = ["page-all", "page-system-packagemanager-installpackage"]
    delete_privileges = ["page-all", "page-system-packagemanager-installed"]

    get_tests = [{"name": "Read installed packages"}]
    post_tests = [
        {
            "name": "Check install of pfSense repo package",
            "resp_time": 60,
            "resp_data_empty": True,
            "post_test_callable": "is_package_installed",
            "req_data": {
                "name": INSTALL_PKG_NAME
            }
        },
        {
            "name": "Check package name required constraint",
            "status": 400,
            "return": 1073
        },
        {
            "name": "Check inability to install already installed package",
            "status": 400,
            "return": 1076,
            "resp_time": 60,
            "req_data": {
                "name": INSTALL_PKG_NAME
            }
        },
        {
            "name": "Check install package from pfSense repo that doesn't exist",
            "status": 400,
            "return": 1075,
            "resp_time": 5,
            "req_data": {
                "name": "pfSense-pkg-INVALID"
            }
        },

    ]
    delete_tests = [
            {
                "name": "Test deletion of installed package",
                "resp_time": 60,
                "resp_data_empty": True,
                "post_test_callable": "is_package_deleted",
                "req_data": {
                    "name": INSTALL_PKG_NAME
                }
            },
            {
                "name": "Check package name required constraint",
                "status": 400,
                "return": 1073
            },
            {
                "name": "Check inability to delete system packages, only pfSense packages",
                "status": 400,
                "return": 1074,
                "req_data": {
                    "name": "sudo"
                }
            }
        ]

    def is_package_installed(self):
        """Checks if a package is successfully installed."""
        # Local variables
        pkg_dirs_out = self.pfsense_shell("ls /usr/local/share")

        # Check if the package directory exists in /usr/local/share
        if INSTALL_PKG_NAME not in pkg_dirs_out:
            raise AssertionError(
                f"Expected {INSTALL_PKG_NAME} to have install directory in /usr/local/share, got {pkg_dirs_out}"
            )

    def is_package_deleted(self):
        """Checks if a package is successfully deleted."""
        # Local variables
        pkg_dirs_out = self.pfsense_shell("ls /usr/local/share")

        # Check if the package directory exists in /usr/local/share
        if INSTALL_PKG_NAME in pkg_dirs_out:
            raise AssertionError(
                f"Unxpected package {INSTALL_PKG_NAME} with install directory in /usr/local/share, got {pkg_dirs_out}"
            )



APIE2ETestSystemPackage()
