## Configuration File

A configuration file is a text file used to describe how a program should operate without requiring those settings to be hard coded into the program itself. Instead of recompiling the server whenever a port, directory, route, or other behaviour changes, the server reads a configuration file at startup and uses the information contained in it to build its runtime configuration.

For Webserv, the configuration file is the description of how our HTTP server should behave, telling the server which network interfaces and ports it should listen on, where files are located, which HTTP methods are accepted for particular routes, which error pages should be used, among other things. It is thus an important part of the project architecture, sitting between the human administrator and the server implementation.

### Configuration File Grammar

We used **NGINX** as the main inspiration for our configuration file syntax. NGINX is a web server that uses a hierarchical configuration system based on **directives** and **blocks**. Directives define individual settings, while blocks group related directives together. This makes the configuration easily readable and naturally represents the relationship between a server and the different routes it serves.

Our configuration follows the same idea, excluding some blocks and directives that are unnecessary for this project, such as the http and events blocks. Based on the Webserv subject, our configuration can thus be simplified to a top-level collection of server blocks with location blocks and directives inside them:

```ebnf
<configuration> ::= <server-block>+

<server-block> ::= "server" "{"
                       <server-item>*
                   "}"

<server-item> ::= <server-directive> | <location-block>

<location-block> ::= "location" <URI> "{"
                         <location-directive>*
                     "}"
```

This gives us an NGINX-inspired structure while keeping only the functionality required by Webserv. The subject specifically requires configuration for listening interfaces and ports, error pages, request-body limits, route-specific methods, redirections, filesystem roots, directory listings, index files, uploads, and CGI execution.

In terms of syntax, this configuration file respects the following rules:
 - The file accepts comments, which are comprised with any string of words preceded by a '#' character. These comments are ignored when executing the program;
 - There is no enforcement for ordering of directives and inner blocks within blocks.
 - Directives and their arguments must be separated by whitespace characters (defined by ```std::isspace```). Sequences of multiple whitespace characters are ignored and counted as one single delimiter.
 - Every directive must be terminated by a semicolon. Multi-line contexts, defined as blocks, such as the server or location blocks defined in the ebnf, must be enclosed in curly braces.
 - All directive keywords, which will be defined below, are strictly case sensitive.
 - Folder paths must follow standard Unix formatting.
 - File extensions for CGI must include the leading dot.

### Accepted Directives Description and Rules

Although Nginx accepts several directives and can be modified to accept external directives, our parsing only supports the following directives within the respective blocks:

**Server Block**

Defines the configuration for one server, including its listening endpoints and associated locations.

```markdown

listen <IP:PORT | PORT>: Defines the IP address and port on which the server listens.
    If only PORT is specified, the address defaults to 127.0.0.1 (localhost).
    PORT must be between 1 and 65535.
    Mandatory: each server must define at least one listen directive.
    Unique within a server: the same endpoint may not be defined more than once within servers.
    Global endpoint collision: two different servers may not bind the same endpoint.

root <PATH>: Sets the root directory for requests for the server.
    Relative paths are resolved relative to the server's current working directory.
    Absolute and relative paths are validated and stored as configured.
    Optional: if absent, each content-serving location must provide its own root.
    Unique: a server may only have one root directive.

client_max_body_size <SIZE>: Sets the maximum allowed size of the client request body (in bytes).
    Setting size to 0 disables checking of client request body size.
    Adding 'M' in the end of the value represents a value in Megabytes.
    Default: 1M = 1048576
    Max value: SIZE_MAX
    Unique: a server must have only one client_max_body_size directive.

error_page <CODE>... <URI>: Defines the URI that will be shown for the specified errors.
    One or more status codes are provided as separate whitespace-delimited arguments, followed by the target URI.
    If a status code is repeated, it will return an exception.
    Status code: 400-599 (HTTP error responses)
    Multiple directive.

```

**Location Block**

```markdown

location <URI>: Sets configuration for requests whose URI begins with the specified URI. Directives within the block override corresponding server-level settings where the directive supports location-level override.
    URI must begin with '/'.
    URIs cannot be duplicated within a server.
    The location matching will follow the longest matching URI path-component prefix rule.
    Mandatory: each server must define at least one location block.
    Multiple locations may be defined within a server.

allow_methods <METHOD>...: Defines the HTTP methods accepted by the location.
    Multiple methods may be specified and must be separated by whitespace.
    Repeated methods will be ignored.
    Acceptable methods: <GET | POST | DELETE>. Methods not within this list will not be accepted and return an error.
    Default: GET.
    Multiple directive, if there is more than one allow_methods directive within a location, the methods will be combined.

root <PATH>: Sets the root directory for requests for this location.
    Relative paths are resolved relative to the server's current working directory.
    Absolute and relative paths are validated and stored as configured.
    Optional directive: if not defined, inherits from server; if server root directive is also empty, an error is thrown.
    Unique: a location may contain at most one root directive.
    A location-level root overrides the server-level root.

index <FILE>...: Defines the default file(s) to serve when the requested URI refers to a directory.
    Multiple files may be specified and are stored in the configured order. For each directory request, the server checks them in order and serves the first readable regular file it finds. If none exists, autoindex determines whether a directory listing is generated.
    Files must be relative paths / names.
    Default: index.html. When the directive is provided, the default is replaced.
    Multiple directives are allowed; files from subsequent directives are appended in order.

autoindex <ON | OFF>: Enables or disables the directory listing output. 
    Default: OFF.
    Unique directive: a location may contain at most one autoindex directive.
    Optional directive.

upload_store <PATH>: Specifies a directory to which output files will be saved to via a relative path.
    Mandatory if POST is an allowed method.
    The path must refer to an already existing writable directory.
    Unique: a location may contain at most one upload_store directive.

cgi <EXTENSION> <PATH>: Enables CGI execution for files with the specified extension within this location.
    Extension must be a valid file extension supported by this project. Accepted extensions are .py and .php.
    Path must be absolute and refer to a valid executable CGI program with the required permissions.
    Multiple directive: various cgi directives with different extensions can be declared.
    The same extension may not be defined more than once within a location.

return <CODE> <URL>: Stops processing and returns the specified code to a client, redirecting to the specified URL.
    Acceptable codes: 301, 302, 303, 307, and 308.
    Unique: a location may contain at most one return directive.
    A location block that contains a return directive may not contain root, index, autoindex, cgi, or upload_store directives.

```


Bibliography:
 - https://nginx.org/en/docs/beginners_guide.html
 - https://nginx.org/en/docs/http/ngx_http_core_module.html
 - https://github.com/fdintino/nginx-upload-module
 - https://nginx.org/en/docs/http/ngx_http_rewrite_module.html