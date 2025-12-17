# Securing API Access

In the age of network automation, APIs on network appliances unlocks incredible efficiency and potential, but it also 
can also create a high-stakes attack vector. Unlike standard web applications, a compromised firewall doesn't just leak 
data; it can grant attackers administrative control over your network's perimeter, allowing them to rewrite traffic rules 
and bypass your defenses entirely. The REST API package includes several built-in security features to help protect
API access and ensure that only authorized users and systems can interact with your pfSense instances.

## Step 1: Follow Netgate's Best Practices for Remote Access

If you need access to the pfSense REST API from outside your local network, it is critical that you follow Netgate's
[Allowing Remote Access to the GUI¶](https://docs.netgate.com/pfsense/en/latest/recipes/remote-firewall-administration.html) 
guide to ensure that your pfSense instance is properly secured against unauthorized access. This includes:

- Enabling HTTPS for the web GUI to encrypt traffic between clients and the pfSense instance.
- Using a VPN to connect to your pfSense instance remotely, rather than exposing the web GUI directly to the internet.
- Configuring strong firewall rules to restrict access to the webConfigurator.

## Step 2: Choose an Appropriate Authentication Method

The authentication method you choose for API access will depend on your specific use case and security requirements
for your environment. The pfSense REST API package supports several different authentication methods, and allows
multiple methods to be enabled simultaneously. The three main authentication methods supported are:

### Basic Authentication (Local Database)

[Basic authentication](AUTHENTICATION_AND_AUTHORIZATION.md#basic-authentication-local-database) allows you to authenticate
with the same username and password you use to log into the pfSense web GUI. This method is the default authentication
method for the REST API package as it allows for out-of-the-box functionality without any additional configuration. 
However, basic authentication is less secure than other methods and should only be used in trusted environments and
over secure connections (e.g., HTTPS or VPN).

**Pros**:

- Easy to set up and use.
- No additional configuration required.

**Cons**:

- Less secure than other methods.
- Credentials are sent with each request, increasing the risk of interception, especially if not using HTTPS.
- Credentials may also allow web GUI and/or SSH access, which may not be desirable for API-only users.

### JWT Authentication

[JWT authentication](AUTHENTICATION_AND_AUTHORIZATION.md#json-web-token-jwt-authentication) allows you to authenticate 
using JSON Web Tokens. These are short-lived, digitially signed tokens that can be used to authenticate API requests without
sending a username and password with each request. JWT authentication is more secure than basic authentication and is
recommended for production environments who need session-based or short-lived authentication. This is especially useful
for front-end applications or scripts that need to make multiple API requests over a short period of time.

**Pros**:

- More secure than basic authentication.
- Tokens can be short-lived, reducing the risk of compromise.
- Tokens do not expose pfSense user credentials with each request.

**Cons**:

- Requires additional configuration to set up.
- Tokens need to be refreshed before they expire.

### Key Authentication

[Key authentication](AUTHENTICATION_AND_AUTHORIZATION.md#api-key-authentication) allows you to authenticate using
dedicated API keys. These keys are created specifically for API access and never require a username or password to 
be sent with requests. Key authentication is the most secure method and is recommended for production environments
where security is a top priority. This method is especially useful for automated systems or services that need to
make API requests without human intervention.

**Pros**:

- Most secure authentication method.
- API keys can be easily revoked or rotated without affecting user accounts.
- Does not expose pfSense user credentials with requests.
- Supports configurable key-lengths and hashing algorithms for purpose-specific security needs.

**Cons**:
 
- Requires additional configuration to set up.
- API keys need to be securely stored and managed.

## Step 4: Use API-specific user accounts

Regardless of the authentication method you choose, the REST API package uses pfSense's built-in privilege system to
control access to API endpoints. This means that all credentials used for API access must belong to a pfSense user account
who has been granted the appropriate API privileges. Each endpoint has its own privileges for each HTTP method supported
by that endpoint. It is highly recommended that you create dedicated user accounts specifically for API access, rather 
than using existing user accounts that may have broader access. This helps to limit the potential impact of a 
compromised account and allows for better tracking of API activity. It is also highly recommended to follow the principle
of least privilege when assigning API privileges to user accounts. Only grant the minimum privileges necessary for the
intended use case.

!!! Warning
    The `page-all` privilege grants unrestricted access to all API endpoints and methods. Avoid assigning this privilege
    to user accounts unless absolutely necessary, as it significantly increases the risk of unauthorized access and 
    potential misuse of the API.

## Step 4: Restrict API access to specific interfaces

By default, the pfSense REST API package allows requests received on any interface IP to respond to API requests. However,
you can restrict the API to only respond to requests received on specific interfaces if desired. This can help limit the
exposure of the API to only trusted networks or systems beyond just setting firewall rules. To configure which 
interfaces the API will respond to, navigate to `System` > `REST API` > `Settings` > `Allowed Interfaces` and select 
the desired interfaces.

!!! Warning
    This setting is not a replacement for proper firewall rules. This setting should be used in addition to firewall rules
    to provide a layered approach to security. Ensure that you have proper firewall rules in place to restrict access to the API
    to only trusted networks or systems, then ensure the API is configured to only respond on those same interfaces.

## Step 5: Configure API access lists

The REST API package includes an API access list feature that allows you to restrict API access based on source IP,
network, time-of-day, and/or user. This provides an additional layer of security by allowing you to define specific rules for who 
can access the API, when they can access it, and from where. To configure API access lists, navigate to `System` > `REST API` >
`Access Lists` and create the desired access list rules. When designing your access list rules, consider the following best practices:

- Only allow IPs you trust and have a legitimate use case for accessing the API.
- Only allow the relevant users to access the API from their respective IPs or networks.
- If possible, configure and apply a schedule to apply to the access list rules to limit access to only when necessary.

!!! Warning
    This access control list is not a replacement for proper firewall rules. This setting should be used in addition to 
    firewall rules to provide a layered approach to security. Ensure that you have proper firewall rules in place to 
    restrict access to the API to only trusted networks or systems, then use the access list to further restrict access 
    based on your specific requirements.

## Step 6: Update Regularly

Ensure that you are running the latest version of the pfSense REST API package to benefit from the latest security
patches and features. Regularly check for updates and apply them as soon as possible to minimize the risk of vulnerabilities.

!!! Tip
    If you are using Prometheus for monitoring in your environment, consider using the [official pfREST Prometheus exporter](https://github.com/pfrest/pfsense_exporter)
    to monitor for outdated pfSense REST API package versions across your fleet of pfSense instances!