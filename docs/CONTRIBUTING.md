# Development and Contribution Guide

To get started with development on the pfSense REST API package, you will need to have a good understanding of the
package's structure, object-oriented PHP, and how to contribute to the project. This guide will help you get started
with the REST API package and provide you with the information you need to develop effectively.

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

!!! Important
    If your contribution involves a security vulnerability, please do not open a public issue or pull request. Instead,
    please report the vulnerability to a [project maintainer](index.md#maintainers) directly.

## Project Structure

The majority of the pfSense REST API package code can be found at `pfSense-pkg-RESTAPI/files/usr/local/pkg/RESTAPI`.
The project utilizes PHP namespaces to organize its codebase.

Below is a brief overview of the project's structure:

### RESTAPI/

The main namespace for the REST API package. All classes and interfaces are organized under this namespace or a child
namespace within the `RESTAPI/` directory.

### .resources/

While not a PHP namespace, this directory contains common resources and tools used by the REST API package. This includes:

#### cache/

Contains the cache files for the REST API package. Cache files are typically JSON datasets populated by [\RESTAPI\Core\Cache](https://pfrest.org/php-docs/classes/RESTAPI-Core-Cache.html) child classes and are refreshed on the schedule defined in the class.

#### includes/

Contains additional PHP libraries and classes required by the REST API package. Because pfSense does not include a PHP package manager, these libraries are installed to this directory via composer when the package is built.

#### scripts/

Contains helper scripts for the REST API package. These scripts are used to automate tasks such as generating OpenAPI documentation, running tests, and more.

### Auth/

Contains the [\RESTAPI\Core\Auth](https://pfrest.org/php-docs/classes/RESTAPI-Core-Auth.html) child classes. Auth classes are used to define and manage authentication methods for the REST API package. For information about writing custom authentication classes, see the [Building Custom Authentication Classes](BUILDING_CUSTOM_AUTH_CLASSES.md) documentation.

### Caches/

Contains the [\RESTAPI\Core\Cache](https://pfrest.org/php-docs/classes/RESTAPI-Core-Cache.html) child classes. [Cache classes](DISPATCHERS_AND_CACHING.md) are used to define and manage cache data and files for the REST API package.

### ContentHandlers/

Contains the [\RESTAPI\Core\ContentHandler](https://pfrest.org/php-docs/classes/RESTAPI-Core-ContentHandler.html) child classes. ContentHandler classes are used to define and manage the supported `content-type` and `accept` header options for the REST API package.

### Core/

Contains the [core classes and interfaces](https://pfrest.org/php-docs/namespaces/restapi-core.html) for the REST API package framework.

### Dispatchers/

Contains the [\RESTAPI\Core\Dispatcher](https://pfrest.org/php-docs/classes/RESTAPI-Core-Dispatcher.html) child classes. Dispatcher classes are used to define and manage long-running processes that run in the background.

### Endpoints/

Contains the [\RESTAPI\Core\Endpoint](https://pfrest.org/php-docs/classes/RESTAPI-Core-Endpoint.html) child classes for the REST API package. Endpoint classes are used to define, customize, and manage the REST API endpoints. Each endpoint class corresponds to a specific REST API endpoint in the pfSense web root and is responsible for dynamically generating the endpoint's PHP file in the web root.

### Fields/

Contains the [\RESTAPI\Core\Field](https://pfrest.org/php-docs/classes/RESTAPI-Core-Field.html) child classes. Field classes are used to define and manage various data types supported by the REST API and are intended to be assigned to Model classes.

### Forms/

Contains the [\RESTAPI\Core\Form](https://pfrest.org/php-docs/classes/RESTAPI-Core-Form.html) child classes. Forms are used to dynamically generate pfSense webConfigurator pages for a specific Model class.

### Models/

Contains the [\RESTAPI\Core\Model](https://pfrest.org/php-docs/classes/RESTAPI-Core-Model.html) child classes. Model classes are used to define and manage the data structure of the object's the REST API package interacts with (i.e. firewall rules, static routes, etc.).

### QueryFilters/

Contains the [\RESTAPI\Core\QueryFilter](https://pfrest.org/php-docs/classes/RESTAPI-Core-QueryFilter.html) child classes. QueryFilter classes are used to define and manage the query filters that are used to filter data returned by the REST API. For information about writing query filters, see the [Building Custom Query Filter Classes](BUILDING_CUSTOM_QUERY_FILTER_CLASSES.md) documentation.

### Responses/

Contains the [\RESTAPI\Core\Response](https://pfrest.org/php-docs/classes/RESTAPI-Core-Response.html) child classes. Response classes are throwable classes that are used to generate and return a specific HTTP response code and message. Endpoints will automatically catch these exceptions if thrown and return the appropriate, JSON formatted response to the client along with the appropriate HTTP response code and headers.

### Tests/

Contains the [\RESTAPI\Core\TestCase](https://pfrest.org/php-docs/classes/RESTAPI-Core-TestCase.html) child classes. TestCase classes are used to define and manage the tests for the REST API package. See the [Testing](#testing) section for more information.

### Validators/

Contains the [\RESTAPI\Core\Validator](https://pfrest.org/php-docs/classes/RESTAPI-Core-Validator.html) child classes. Validator classes are reusable classes that are used to validate data for a specific Field object. They are intended to be assigned to a Field object's `validators` property to perform the specific validation against that field.

### autoloader.inc

The autoloader script for the REST API package. This file can be included in your .php or .inc file's `require_once` statements to automatically load all RESTAPI classes.

!!! Tip
    The full PHP API reference for this package including documentation for all applicable classes can be
    found [here](https://pfrest.org/php-docs/).

## Style Guidelines

This projects uses opinionated code formatters to ensure a consistent code style across the project, this also ensures
that contributions are easier to review and maintain. The project uses the following code formatters:

- [Prettier](https://prettier.io) with the [PHP plugin](https://github.com/prettier/plugin-php) for PHP files
    - From the project root, run `npm install` to install Prettier and the Prettier-PHP plugin.
    - From the project root, run `./node_modules/.bin/prettier --write .` to format all files in the project.
- [Black](https://black.readthedocs.io/en/stable/) for Python files
    - From the project root, run `pip install -r requirements.txt` to install Black.
    - From the project root, run `black .` to format all Python files in the project.

## Testing

Testing this package poses a number of challenges. Because pfSense is a hardened, security-focused operating system,
it is difficult to install traditional testing tools like PHPUnit. Instead, the package's framework includes a custom
[TestCase class](https://pfrest.org/php-docs/classes/RESTAPI-Core-TestCase.html) that resembles PHPUnit, but is designed 
to work within the pfSense environment without requiring additional dependencies. This project uses a mix of unit tests,
integration tests, and end-to-end tests to ensure the package is stable and reliable. Common test cases include:

- Ensuring the package's framework functions as intended.
- Ensuring Model classes correctly configure and interact with the pfSense system. This includes testing that the
  corresponding backend services are correctly configured after a Model object is created, updated, deleted, etc.
- Ensuring that the package's endpoints correctly handle requests and return the expected responses.

It is not important to differentiate between unit, integration and end-to-end tests when writing tests for this package.
The package's testing suite is designed to run all tests in a single command and will automatically categorize tests.
The most important thing is to ensure that tests are written to sufficiently cover the expected behavior of the package.

### Writing Tests

Writing tests for the package is similar to writing tests for any other PHP project. The package's testing framework
will automatically discover and run tests that extend the package's [TestCase class](https://pfrest.org/php-docs/classes/RESTAPI-Core-TestCase.html)
and are located in the `RESTAPI/Tests/` directory. To write a test, you can create a new PHP file in the `RESTAPI/Tests/`
directory and extend the `TestCase` class. Any method within your TestCase class that begins with `test` will be run
as a test case.

Below is a simple example of a TestCase class:

```php
<?php

namespace RESTAPI\Tests;

require_once('RESTAPI/autoloader.inc');

use RESTAPI\Core\TestCase;

class MyTestCase extends TestCase
{
    public function test_something()
    {
        # Add your test logic here
        $this->assert_is_true(true);
    }
}
```

!!! Important
    The testing framework will make a copy of the pfSense configuration before the tests are run and will restore the
    configuration after the tests complete. This ensures that the tests do not modify the pfSense configuration 
    permanently which could cause issues with the firewall or remaining tests. It is still recommended that any changes
    made to the configuration during a test are removed or reverted at the end of the test.

### Running Tests

To run all tests for the package, you can use the following command:

```bash
pfsense-restapi runtests
```

To run tests that contain a specific keyword, you can use the following command:

```bash
pfsense-restapi runtests <keyword>
```

!!! Danger
    Running tests is a disruptive process and should not be done on a production pfSense instance. It is recommended to
    only run tests on a development or testing instance of pfSense. A virtual machine with snapshots is ideal for this.
