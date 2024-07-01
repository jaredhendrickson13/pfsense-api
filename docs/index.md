# pfSense REST API

The pfSense REST API package is an unofficial, open-source REST API for pfSense CE and pfSense Plus firewalls that is
designed to be light-weight, fast, and easy to use. This guide will help you get started with the REST API package and
provide you with the information you need to configure and use the package effectively.

!!! Tip
    Looking for documentation on REST API endpoints and parameters? An interactive [Swagger documentation](./SWAGGER_AND_OPENAPI.md)
    site is available on your pfSense instance after [installing the package](./INSTALL_AND_CONFIG.md) under 
    'System' -> 'REST API' -> 'Documentation'. Alternatively, a simplified version of the Swagger documentation is 
    available [here](https://pfrest.org/api-docs/).

!!! Note
    These docs are only applicable to the REST API v2 package and later. If you are using the legacy v1 package, please
    refer to the docs on the [legacy branch](https://github.com/jaredhendrickson13/pfsense-api/tree/legacy).

## Key Features

- 100+ endpoints available for managing your firewall and associated services
- Easy to use querying and filtering
- Configurable security settings
- Supports HATEOAS driven development
- Customizable authentication options
- Built-in Swagger documentation

New features are constantly being added. If you have a feature request, please open an issue on the project's [GitHub
repository](https://github.com/jaredhendrickson13/pfsense-api/issues/new?labels=backlog%2C+feature+request&projects=&template=feature-request.md).

## Source Code & Contributions

The source code for this project is available in its entirety on [GitHub](https://github.com/jaredhendrickson13/pfsense-api)
and is licensed under an [Apache 2.0 license](https://github.com/jaredhendrickson13/pfsense-api/blob/master/LICENSE).
Contributions are welcome and encouraged. If you would like to contribute to the project, please read through the
[contribution guidelines](CONTRIBUTING.md) before
opening a pull request.

### Maintainers

- <a href="https://github.com/jaredhendrickson13"><img src="https://github.com/jaredhendrickson13.png" alt="Jared Hendrickson" title="Jared Hendrickson" width="30" height="30"/> Jared Hendrickson</img></a> - github@jaredhendrickson.com

!!! Important
    Unless your inquiry is regarding a security vulnerability or other sensitive matter, please do not contact the
    maintainers directly. Instead, please [open an issue](https://github.com/jaredhendrickson13/pfsense-api/issues/new/choose)
    to report a bug or request a feature. For general questions or help requests, please [open a discussion](https://github.com/jaredhendrickson13/pfsense-api/discussions/new/choose).

## Disclaimers

!!! Caution
    - This package is not affiliated or supported by Netgate or the pfSense team. This package is developed and maintained
      by the community.
