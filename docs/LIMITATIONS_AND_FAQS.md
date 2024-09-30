# Limitations and FAQs

## Limitations

Keep the following limitations in mind when using this API:

- pfSense's XML configuration was not designed for large scale concurrency. It may be necessary to delay API calls to
  prevent unexpected behavior, such as configuration overwrites.
- While the REST API contains controls that should help improve scalability and resource management in comparison to 
  the webConfigurator, it is still recommended to use the API with caution on large datasets.

## Frequently Asked Questions

### Why is this package not an official pfSense package?

I'd love for it to be an official package! A few years back, Netgate opened a friendly dialogue about the
possibility of making this package official. There was some back and forth about the direction of the package and pfSense
itself. Since then, I have been working on addressing most of the items that were brought up during the discussions but
it seems it is no longer in Netgate's interest to make this package official. I am still open to the idea of making this
package official, but sadly with the announcement of pfSense Plus's [multi-instance management](https://www.netgate.com/multi-instance-management-pfsense-plus)
it seems highly unlikely that this package will ever be made official.

There are still some benefits to this package being unofficial. It allows for more rapid development and
more flexibility in the direction of the package. It also allows for more community involvement in the development of the
package. It also ensures the package remains free and open-source.

### Why are the v1 and v2 APIs separate packages?

There are three primary reasons it was decided to create a new package for the v2 API:

1. **Backwards Compatibility**: It was realized early on that the v2 API would require a significant amount of changes to
the underlying codebase. This would have made it difficult to maintain backwards compatibility with the v1 API with both 
codebases in one package. Instead, it was decided to create a new package for the v2 API to allow for a clean break from
the v1 API and still allow both packages to be installed concurrently for users who still require v1.
2. **Namespaces**: v1 notably lacked the use of namespaces for the underlying framework which led to a number of conflicts
when trying to implement v2. v2 was designed from the ground up with namespaces in mind to prevent these conflicts and
allow major revisions to be included in the same package going forward.
3. **Accuracy**: The v1 codebase was a brute force attempt at creating an API for pfSense. Initially it was not 
designed to comply with RESTful principles and was only meant to be REST-like. The v2 API was designed from the ground
up with RESTful principles in mind and is a much more accurate representation of a RESTful API, therefor it was decided 
to create a new package to reflect this change while also differentiating it from Netgate's supposed RESTCONF plans.

### Why can I not see passwords/keys/hashes in API responses?

By default, sensitive fields such as passwords, keys and hashes are **not** included in API responses. This is an important
security measure to help prevent critical information from being leaked. All though it is highly advised you keep these
sensitive fields hidden, you can override this behavior by enabling the 'Expose Sensitive Fields' option in 
System > REST API > Settings, or by setting the 'expose_sensitive_fields' option in a `PATCH` request to 
[/api/v2/system/restapi/settings](https://pfrest.org/api-docs/#/SYSTEM/patchSystemRESTAPISettingsEndpoint).
If you do choose to expose sensitive fields, it's recommended you only do so temporarily and only when necessary.