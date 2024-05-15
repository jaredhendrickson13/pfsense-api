# Glossary

This page contains a list of common terms and definitions used throughout the pfSense REST API package and its documentation.

## API

An Application Programming Interface (API) is a set of rules and protocols that allow one software application to interact with another. The pfSense REST API package provides a set of endpoints that allow you to interact with your pfSense firewall programmatically.

## API Key

An API Key is a unique identifier that is used to authenticate API requests. API Keys are used to identify the source of an API request and are often used in place of traditional username and password authentication.

## Cache

A Cache is a temporary storage location that is used to store data that is frequently accessed. Caching is used to improve the performance of an application by reducing the amount of time it takes to retrieve data from the original source. In the context of the pfSense REST API package, the Cache class is used to store large sets of data locally to reduce the amount of time it takes to retrieve the data from external sources.

## Dispatcher

In the context of the pfSense REST API package, a Dispatcher is a class that is responsible for applying changes to the pfSense configuration and allow the changes to applied in the background. Dispatchers also control the number of apply processes that can run concurrently and enforce timeouts to prevent stuck processes.

## Endpoint

An Endpoint is a specific URL that is used to interact with a specific resource or set of resources. Endpoints are used to perform actions such as reading, creating, updating, and deleting data. In the context of the pfSense REST API package, the Endpoint class defines the URL, HTTP methods, privileges, and other information for a specific API endpoint. The Endpoint class is also responsible for dynamically generating a PHP page in the pfSense web root that handles the API request.

## Field

A Field is a single piece of data that is part of a Model. Fields are used to define the structure of the data that is associated with a Model. Fields can have various attributes such as type, default value, and validation rules.

## Filter

A Filter is a type of Query parameter that is used to limit the data returned by an API endpoint based on specific criteria. Filters are used to narrow down the results to only the data that matches the filter criteria.

## HATEOAS

Hypermedia As The Engine Of Application State (HATEOAS) is a constraint of the REST application architecture that allows clients to navigate the API by following links provided by the server. The pfSense REST API package supports HATEOAS driven development by providing links to related resources in the response data. This allows clients to easily navigate the API and discover available actions and resources related to the current API response.

## Model

A Model is a representation of a data object that is used to define the structure of the data. Models are used in the pfSense REST API package to define the structure of pfSense configuration objects, along with the process needed to apply the configurations associated with the models. The pfSense REST API's Model class is responsible for defining the structure of the data via Fields, validating the data using Validators, and applying the data to the pfSense configuration using Dispatchers.

## OpenAPI

OpenAPI is a specification for building APIs that allows you to define the structure of your API, including endpoints, parameters, request and response bodies, and more. The pfSense REST API package was designed around the OpenAPI specification and is built to automatically document its components as OpenAPI schemas.

## Pagination

Pagination is a technique used to break up large sets of data into smaller, more manageable chunks. Pagination is often used in APIs to limit the amount of data returned in a single response. The pfSense REST API package supports pagination for endpoints that return large sets of data.

## Query

A Query is a set of parameters that are used to filter, sort, and limit the data returned by an API endpoint. Queries are used to control the behavior of an API request and can be used to narrow down the results to only the data that is needed.

## REST

Representational State Transfer (REST) is an architectural style for building APIs that uses standard HTTP methods (GET, POST, PUT, DELETE) to perform actions on resources. REST APIs are stateless, meaning that each request from a client contains all the information needed to process the request.

## Schema

A Schema is a formal definition of the structure of data that is used to validate and describe the data. In the context of the pfSense REST API package, Schemas are used to define the structure of API requests and responses.

## Swagger

Swagger is a set of tools for building, documenting, and consuming REST APIs. Swagger provides a way to define the structure of an API using a JSON or YAML file, which can then be used to generate interactive documentation, client libraries, and more. The pfSense REST API package includes built-in Swagger documentation that provides full documentation for all API endpoints and their associated parameters.

## Validator

A Validator is a class that is used to validate the data associated with a Field. Validators are used to ensure that the Field's value meets certain criteria before it is applied to the pfSense configuration. Validators can check for things like data type, length,
and format.