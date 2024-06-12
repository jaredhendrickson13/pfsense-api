# Installation and Configuration

## Requirements

### Hardware requirements

Overall, the REST API package is designed to be as lightweight as possible and should run fine on any hardware that can
run pfSense. It's recommended to follow Netgate's [minimum hardware requirements](https://docs.netgate.com/pfsense/en/latest/hardware/minimum-requirements.html).

!!! Warning
    While the package should behave identically on architectures other than amd64, it has only been tested on amd64
    builds of pfSense. Support on other architectures is not guaranteed.

### Supported pfSense versions

- pfSense CE 2.7.2 (amd64)
- pfSense Plus 24.03 (amd64)

!!! Tip
    Don't see your version of pfSense? Older versions of pfSense may be supported by older versions of this package.
    Check the [releases page](https://github.com/jaredhendrickson13/pfsense-api/releases).

## Installing the package

The pfSense REST API package is built just like any other pfSense package and can therefor be installed easily using
`pkg` from the pfSense command line:

```bash
pkg-static -C /dev/null add https://github.com/jaredhendrickson13/pfsense-api/releases/latest/download/pfSense-2.7.2-pkg-RESTAPI.pkg
```

!!! Note
    You may need to customize the installation command to reference the package built for your pfSense version. Check
    the [releases page](https://github.com/jaredhendrickson13/pfsense-api/releases) to find the package built for
    your version of pfSense.

!!! Important
    When updating pfSense, **you must reinstall this package afterward** as pfSense removes unofficial packages during
    system updates.

## Configuring the package

The REST API is designed to be used out of the box. However, there are a number of configuration options available to
you to customize the API to your needs. The REST API settings can be configured in pfSense webConfigurator under
'System' -> 'REST API' or via `PATCH` request to the [/api/v2/system/restapi/settings](https://pfrest.org/api-docs/#/SYSTEM/patchSystemRESTAPISettingsEndpoint)
endpoint.

## Deleting the package

To remove the REST API package from your pfSense instance, you can use the following command:

```bash
pfsense-restapi delete
```

!!! Note
    In the event that you are unable to use the `pfsense-restapi` command, you can manually remove the package by
    running `pkg-static delete pfSense-pkg-RESTAPI`.

## Updating the package

You can update the package to latest version available to your pfSense version by running the following command:

```bash
pfsense-restapi update
```

## Reverting the package to a specific version

If you need to revert or upgrade the package to a specific version, you can do so by running the following command:

```bash
pfsense-restapi revert <version>
```
