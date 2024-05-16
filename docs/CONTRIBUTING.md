# Development and Contribution Guide

To get started with development on the pfSense REST API package, you will need to have a good understanding of the
package's structure, object-oriented PHP, and how to contribute to the project. This guide will help you get started
with the REST API package and provide you with the information you need to develop effectively.

## Project Structure

The majority of the pfSense REST API package code can be found at `pfSense-pkg-RESTAPI/files/usr/local/pkg/RESTAPI`.
The project utilizes PHP namespaces to organize its codebase.

Below is a brief overview of the project's structure:

- `RESTAPI/`: The main namespace for the REST API package.
  - `.resources/`: While not a PHP namespace, this directory contains a common place resources and tools:
    - `cache/`: Contains the cache files for the REST API package. Cache files are typically JSON datasets that are
      populated by `\RESTAPI\Core\Cache` classes and are refreshed on the schedule defined in the class.
    - `includes/`: Contains additional PHP libraries and classes required by the REST API package. Because pfSense does
      not include a PHP package manager, these libraries are installed to this directory via composer when the package
      is built.
    - `scripts/`: Contains helper scripts for the REST API package. These scripts are used to automate tasks such as
      generating OpenAPI documentation, running tests, and more.
  - `Auth/`: Contains the \RESTAPI\Core\Auth child classes
  - `Caches/`: Contains the \RESTAPI\Core\Cache child classes
  - `ContentHandlers/`: Contains the \RESTAPI\Core\ContentHandler child classes
  - `Core/`: Contains the core classes and interfaces for the REST API package framework.
  - `Dispatchers/`: Contains the \RESTAPI\Core\Dispatcher child classes
  - `Endpoints/`: Contains the \RESTAPI\Core\Endpoint child classes for the REST API package.
  - `Fields/`: Contains the \RESTAPI\Core\Field child classes
  - `Forms/`: Contains the \RESTAPI\Core\Form child classes
  - `Models/`: Contains the \RESTAPI\Core\Model child classes
  - `Responses/`: Contains the \RESTAPI\Core\Response child classes
  - `Tests/`: Contains the \RESTAPI\Core\TestCase child classes
  - `Validators/`: Contains the \RESTAPI\Core\Validator child classes
  - `autoloader.inc`: The autoloader script for the REST API package. This file can be included in your .php or .inc
    file's `require_once` statements to automatically load all RESTAPI classes.

!!! Tip
The full PHP API reference for this package including documentation for all applicable classes can be
found [here](https://pfrest.org/php_reference/).

## Style Guidelines

This projects uses opinionated code formatters to ensure a consistent code style across the project, this also ensures
that contributions are easier to review and maintain. The project uses the following code formatters:

- [Prettier](https://prettier.io) with the [PHP plugin](https://github.com/prettier/plugin-php) for PHP files
  - From the project root, run `npm install` to install Prettier and the Prettier-PHP plugin.
  - From the project root, run `./node_modules/.bin/prettier --write .` to format all files in the project.
- [Black](https://black.readthedocs.io/en/stable/) for Python files
  - From the project root, run `pip install -r requirements.txt` to install Black.
  - From the project root, run `black .` to format all Python files in the project.

## Contributing

You can contribute to the project in a variety of ways, including but not limited to:

- Bug fixes
- New features
- Documentation improvements
- Reporting and collaborating to open [issues](https://github.com/jaredhendrickson13/pfsense-api/issues)
- Opening and contributing to [discussions](https://github.com/jaredhendrickson13/pfsense-api/discussions)

To make a code contribution, please follow these steps:

1. Fork the repository on [GitHub](https://github.com/jaredhendrickson13/pfsense-api).
2. Clone your fork locally and make your changes.
3. Commit your changes and push them to your fork.
4. Open a pull request on the main repository to merge your changes into the project.
5. Wait for your pull request to be reviewed and merged.
