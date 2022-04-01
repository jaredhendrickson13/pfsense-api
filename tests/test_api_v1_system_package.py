import e2e_test_framework


class APIE2ETestSystemPackage(e2e_test_framework.APIE2ETest):
    uri = "/api/v1/system/package"
    get_tests = [{"name": "Read installed packages"}]
    post_tests = [
        {
            "name": "Check install of pfSense repo package",
            "resp_time": 30,
            "payload": {
                "name": "pfSense-pkg-nmap"
            }
        },
        {
            "name": "Check install of package via URL",
            "resp_time": 30,
            "payload": {
                "name": "https://github.com/jaredhendrickson13/pfsense-saml2-auth/releases/latest/download/pfSense-2.5-pkg-saml2-auth.txz"
            }
        },
        {
            "name": "Check forced install of pfSense repo package",
            "resp_time": 30,
            "payload": {
                "name": "pfSense-pkg-nmap",
                "force": True
            }
        },
        {
            "name": "Check forced install of package via URL",
            "resp_time": 30,
            "payload": {
                "name": "https://github.com/jaredhendrickson13/pfsense-saml2-auth/releases/latest/download/pfSense-2.5-pkg-saml2-auth.txz",
                "force": True
            }
        },
        {
            "name": "Check package name required constraint",
            "status": 400,
            "return": 1073
        },
        {
            "name": "Check inability to install already installed package when not forced",
            "status": 400,
            "return": 1076,
            "resp_time": 30,
            "payload": {
                "name": "pfSense-pkg-nmap"
            }
        },
        {
            "name": "Check inability to install already installed package via URL when not forced",
            "status": 400,
            "return": 1076,
            "resp_time": 30,
            "payload": {
                "name": "https://github.com/jaredhendrickson13/pfsense-saml2-auth/releases/latest/download/pfSense-2.5-pkg-saml2-auth.txz"
            }
        },
        {
            "name": "Check package install timeout enforcement",
            "status": 504,
            "return": 1078,
            "resp_time": 8,
            "payload": {
                "name": "https://1.2.3.4/",
                "timeout": 5
            }
        },
        {
            "name": "Check package install timeout maximum constraint",
            "status": 400,
            "return": 1079,
            "payload": {
                "name": "https://1.2.3.4/",
                "timeout": 121
            }
        },
        {
            "name": "Check invalid package file at URL",
            "status": 400,
            "return": 1077,
            "payload": {
                "name": "https://example.com/index.html"
            }
        },
        {
            "name": "Check package install via URL with unresolvable DNS name",
            "status": 502,
            "return": 13,
            "resp_time": 5,
            "payload": {
                "name": "https://doesnotexist.jaredhendrickson.com"
            }
        },
        {
            "name": "Check install package from pfSense repo that doesn't exist",
            "status": 400,
            "return": 1075,
            "resp_time": 5,
            "payload": {
                "name": "pfSense-pkg-INVALID"
            }
        },

    ]
    delete_tests = [
            {
                "name": "Test deletion of installed package",
                "resp_time": 30,
                "payload": {
                    "name": "pfSense-pkg-nmap"
                }
            },
            {
                "name": "Test deletion of URL installed package",
                "resp_time": 30,
                "payload": {
                    "name": "pfSense-pkg-saml2-auth"
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
                "payload": {
                    "name": "sudo"
                }
            }
        ]


APIE2ETestSystemPackage()
