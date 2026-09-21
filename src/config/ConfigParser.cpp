/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 16:30:07 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 11:42:56 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/config/ConfigParser.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <limits>
#include <set>

typedef std::vector<t_token>::const_iterator token_iterator;

typedef void (*server_directive_handler)(token_iterator &, const token_iterator &, ServerConfig &);
typedef void (*location_directive_handler)(token_iterator &, const token_iterator &, LocationConfig &);

struct t_server_directive
{
    const char *name;
    server_directive_handler handler;
};

struct t_location_directive
{
    const char *name;
    location_directive_handler handler;
};

static void parseErrorPageDirective(token_iterator &, const token_iterator &, ServerConfig &);
static void parseClientMaxBodySizeDirective(token_iterator &, const token_iterator &, ServerConfig &);
static void parseListenDirective(token_iterator &, const token_iterator &, ServerConfig &);
static void parseLocationBlock(token_iterator &, const token_iterator &, ServerConfig &);
static void parseRootDirective(token_iterator &, const token_iterator &, ServerConfig &);
static void parseAllowMethodsDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseRootDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseIndexDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseAutoindexDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseUploadStoreDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseCgiDirective(token_iterator &, const token_iterator &, LocationConfig &);
static void parseReturnDirective(token_iterator &, const token_iterator &, LocationConfig &);

static const t_server_directive SERVER_DIRECTIVES[] =
{
    {"listen", parseListenDirective},
    {"root", parseRootDirective},
    {"client_max_body_size", parseClientMaxBodySizeDirective},
    {"error_page", parseErrorPageDirective},
    {"location", parseLocationBlock}
};

static const t_location_directive LOCATION_DIRECTIVES[] =
{
    {"allow_methods", parseAllowMethodsDirective},
    {"root", parseRootDirective},
    {"index", parseIndexDirective},
    {"autoindex", parseAutoindexDirective},
    {"upload_store", parseUploadStoreDirective},
    {"cgi", parseCgiDirective},
    {"return", parseReturnDirective}
};

static location_directive_handler findLocationDirective(const std::string &name)
{
    const size_t directive_count = sizeof(LOCATION_DIRECTIVES) / sizeof(LOCATION_DIRECTIVES[0]);
    
    for (size_t i = 0; i < directive_count; ++i)
    {
        if (name == LOCATION_DIRECTIVES[i].name)
            return LOCATION_DIRECTIVES[i].handler;
    }
    return NULL;
}

static server_directive_handler findServerDirective(const std::string &name)
{
    const size_t directive_count = sizeof(SERVER_DIRECTIVES) / sizeof(SERVER_DIRECTIVES[0]);
    
    for (size_t i = 0; i < directive_count; ++i)
    {
        if (name == SERVER_DIRECTIVES[i].name)
            return SERVER_DIRECTIVES[i].handler;
    }
    return NULL;
}

static void validateUniqueServerEndpoints(const std::vector<ServerConfig> &servers)
{
    std::set<t_endpoint> unique_endpoints;

    for (std::vector<ServerConfig>::const_iterator server = servers.begin();
        server != servers.end(); ++server)
    {
        const std::vector<t_endpoint> &endpoints = server->getListen();
        for (std::vector<t_endpoint>::const_iterator endpoint = endpoints.begin();
            endpoint != endpoints.end(); ++endpoint)
        {
            if (!unique_endpoints.insert(*endpoint).second)
                throw std::runtime_error(
                    "Duplicate listen endpoint between server blocks");
        }
    }
}

// Validate error code, ensuring it is a three-digit number between 400 and 599.

static bool isErrorCode(const std::string &value)
{
    if (value.size() != 3)
        return false;

    for (std::string::size_type i = 0; i < value.size(); ++i)
    {
        if (value[i] < '0' || value[i] > '9')
            return false;
    }
    return true;
}

static void validateReturnCode(const std::string &value)
{
    static const char *const allowed_return_codes[] = {"301", "302", "303", "307", "308"};
    const size_t code_count = sizeof(allowed_return_codes)
        / sizeof(allowed_return_codes[0]);

    for (size_t i = 0; i < code_count; ++i)
    {
        if (value == allowed_return_codes[i])
            return;
    }
    throw std::runtime_error("Invalid return code: " + value);
}

static void validateErrorStatusCode(const std::string &value)
{
    int error_code;
    std::istringstream stream(value);
    stream >> error_code;
    if (error_code < 400 || error_code > 599)
        throw std::runtime_error("Invalid error code: " + value);
}

static void validateFilePath(const std::string &path, const std::string &permissions)
{
    if (path.empty())
        throw std::runtime_error("File path cannot be empty");

    if (path[0] != '/')
        throw std::runtime_error("File path must start with '/'");

    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        throw std::runtime_error("File path does not exist: " + path);

    if (!S_ISREG(info.st_mode))
        throw std::runtime_error("File path is not a file: " + path);

    if (permissions.find('R') != std::string::npos && access(path.c_str(), R_OK) != 0)
        throw std::runtime_error("File path is not readable: " + path);

    if (permissions.find('W') != std::string::npos && access(path.c_str(), W_OK) != 0)
        throw std::runtime_error("File path is not writable: " + path);

    if (permissions.find('X') != std::string::npos && access(path.c_str(), X_OK) != 0)
        throw std::runtime_error("File path is not executable: " + path);
}

static void parseErrorPageDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, ServerConfig &server)
{
    ++it;
    std::vector<int> error_codes;
    while (it != end && it->type == WORD && isErrorCode(it->content))
    {
        validateErrorStatusCode(it->content);
        std::istringstream stream(it->content);
        int error_code;
        stream >> error_code;
        error_codes.push_back(error_code);
        ++it;
    }

    if (error_codes.empty())
        throw std::runtime_error("Expected error code after 'error_page'");

    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected error page uri after error code");

    std::string error_page_uri = it->content;
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after error page uri");

    server.addErrorPage(error_codes, error_page_uri);
    ++it;
}

// Validate and parse client_max_body_size directive, ensuring the value is a valid size in bytes and accepting optional 'M' suffix for megabytes.

static size_t parseClientMaxBodySize(const std::string &size_str)
{
    std::string number = size_str;
    bool megabytes = false;

    if (number.empty())
        throw std::runtime_error(
            "Invalid client_max_body_size value: " + size_str);
            
    if (number[number.size() - 1] == 'M')
    {
        megabytes = true;
        number.erase(number.size() - 1);
    }

    for (std::string::size_type i = 0; i < number.size(); ++i)
    {
        if (number[i] < '0' || number[i] > '9')
            throw std::runtime_error(
                "Invalid client_max_body_size value: " + size_str);
    }

    size_t size;
    char extra;
    std::istringstream stream(number);

    if (!(stream >> size) || (stream >> extra))
        throw std::runtime_error(
            "Invalid client_max_body_size value: " + size_str);

    if (megabytes)
    {
        if (size > std::numeric_limits<size_t>::max() / (1024 * 1024))
            throw std::runtime_error(
                "client_max_body_size value out of range: " + size_str);

        size *= 1024 * 1024;
    }

    return size;
}

static void parseClientMaxBodySizeDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, ServerConfig &server)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected size after 'client_max_body_size'");

    std::string size_str = it->content;
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after client_max_body_size value");

    size_t size = parseClientMaxBodySize(size_str);
    server.setClientMaxBodySize(size);
    ++it;
}

// Validate root directive, ensuring the path is valid and accessible

static void validateDirectoryPath(const std::string &path, const std::string &permissions)
{
    if (path.empty())
        throw std::runtime_error("Directory path cannot be empty");

    struct stat info;
    
    if (stat(path.c_str(), &info) != 0)
        throw std::runtime_error("Directory path does not exist: " + path);
        
    if (!S_ISDIR(info.st_mode))
        throw std::runtime_error("Directory path is not a directory: " + path);
    
    if (permissions.find('R') != std::string::npos && access(path.c_str(), R_OK) != 0)
        throw std::runtime_error("Directory path is not readable: " + path);
        
    if (permissions.find('W') != std::string::npos && access(path.c_str(), W_OK) != 0)
        throw std::runtime_error("Directory path is not writable: " + path);
        
    if (permissions.find('X') != std::string::npos && access(path.c_str(), X_OK) != 0)
        throw std::runtime_error("Directory path is not executable: " + path);
}

static std::string joinRootAndPath(const std::string &root, const std::string &path)
{
    if (path.empty())
        return root;
    return root + "/" + path;
}

static bool hasParentDirectoryComponent(const std::string &path)
{
    std::string::size_type start = 0;
    while (start <= path.size())
    {
        std::string::size_type end = path.find('/', start);
        std::string component = path.substr(start, end - start);
        if (component == "..")
            return true;
        if (end == std::string::npos)
            break;
        start = end + 1;
    }
    return false;
}

static void validateLocationPaths(const ServerConfig &server)
{
    const std::vector<LocationConfig> &locations = server.getLocations();

    for (std::vector<LocationConfig>::const_iterator location = locations.begin();
        location != locations.end(); ++location)
    {
        if (!location->has_return_set() && location->has_upload_store_set())
        {
            std::string upload_store_path = joinRootAndPath(
                location->getRoot(), location->getUploadStore());
            validateDirectoryPath(upload_store_path, "WX");
        } 
    }
}

static void parseRootDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, ServerConfig &server)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected root path after 'root'");

    validateDirectoryPath(it->content, "RX");
    server.setRoot(it->content);
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after root path");

    ++it;
}

// Validate listen directive, parsing IP and port, and adding to server configuration

static int parsePort(const std::string &port_str)
{
    if (port_str.empty())
        throw std::runtime_error("Invalid port number: " + port_str);
        
    for (std::string::size_type i = 0; i < port_str.size(); ++i)
    {
        if (port_str[i] < '0' || port_str[i] > '9')
            throw std::runtime_error("Invalid port number: " + port_str);
    }

    int port;
    char extra;
    std::istringstream stream(port_str);
    if (!(stream >> port) || (stream >> extra))
        throw std::runtime_error("Invalid port number: " + port_str);
    if (port < 1 || port > 65535)
        throw std::runtime_error("Port number out of range: " + port_str);
    return port;
}

static void validateIP(const std::string &ip)
{
	size_t start = 0;

	for (int i = 0; i < 4; ++i)
	{
		size_t end = ip.find('.', start);
		if (i < 3 && end == std::string::npos)
			throw std::runtime_error("Invalid IP address: " + ip);
		if (i == 3 && end != std::string::npos)
			throw std::runtime_error("Invalid IP address: " + ip);

		std::string octet = ip.substr(start, end - start);

		if (octet.empty())
			throw std::runtime_error("Invalid IP address: " + ip);

		if (octet.length() > 1 && octet[0] == '0')
			throw std::runtime_error("Invalid IP address: " + ip);

		for (size_t j = 0; j < octet.length(); ++j)
		{
			if (!std::isdigit(static_cast<unsigned char>(octet[j])))
				throw std::runtime_error("Invalid IP address: " + ip);
		}

		std::istringstream stream(octet);
		int value;
		if (!(stream >> value))
            throw std::runtime_error("Invalid IP address: " + ip);

		if (value > 255)
			throw std::runtime_error("IP address octet out of range: " + ip);

		start = end + 1;
	}
    
    /*if (ip == "0.0.0.0")
    {
        throw std::runtime_error("Will not listen on all interfaces: " + ip);
    }*/
}

static void parseListenDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, ServerConfig &server)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected listen address after 'listen'");

    std::string listen_address = it->content;
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after listen address");

    if (listen_address.find(':') != std::string::npos)
    {
        size_t colon_pos = listen_address.find(':');
        std::string ip = listen_address.substr(0, colon_pos);
        validateIP(ip);
        int port = parsePort(listen_address.substr(colon_pos + 1));
        server.addListen(ip, port);
    }
    else
    {
        server.addListen("127.0.0.1", parsePort(listen_address));
    }
    ++it;
}

// Validate and parse location URI, ensuring it starts with '/' and is not empty.
static void parseLocationURI(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected location URI");

    std::string uri = it->content;
    if (uri.empty() || uri[0] != '/')
        throw std::runtime_error("Invalid location URI: " + uri);

    location.setURI(uri);
}

// Validate and parse allow methods directive, ensuring each method is valid and not duplicated.

static allowed_methods parseAllowMethod(const std::string &method)
{
    if (method == "GET")
        return GET;
    if (method == "POST")
        return POST;
    if (method == "DELETE")
        return DELETE;
    throw std::runtime_error("Invalid allow method: " + method);
}

static void parseAllowMethodsDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected allowed methods after 'allow_methods'");

    while (it != end && it->type == WORD)
    {
        location.addAllowedMethods(parseAllowMethod(it->content));
        ++it;
    }

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after allowed methods");

    ++it;
}

// Validate and parse root directive in location block.

static void parseRootDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected root path after 'root'");

    validateDirectoryPath(it->content, "RX");
    location.setRoot(it->content);
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after root path");

    ++it;
}

// Parse ordered index candidates. Files are checked at request time for each requested directory.

static void parseIndexDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected index files after 'index'");

    while (it != end && it->type == WORD)
    {
        if (!it->content.empty() && it->content[0] == '/')
            throw std::runtime_error("Index path must be relative: " + it->content);
        if (hasParentDirectoryComponent(it->content))
            throw std::runtime_error("Index path cannot contain '..': " + it->content);
        location.addIndex(it->content);
        ++it;
    }

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after index files");

    ++it;
}

// Validate and parse autoindex directive in location block, ensuring the value is either 'ON' or 'OFF'.

static void parseAutoindexDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected 'ON' or 'OFF' after 'autoindex'");

    if (it->content == "ON")
        location.setAutoindex(true);
    else if (it->content == "OFF")
        location.setAutoindex(false);
    else
        throw std::runtime_error("Invalid value for autoindex: " + it->content);

    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after autoindex value");

    ++it;
}

// Validate and parse upload_store directive in location block, ensuring the path is valid and accessible. The path will be relative to the location's root.

static void parseUploadStoreDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected upload storage path after 'upload_store'");

    if (!it->content.empty() && it->content[0] == '/')
        throw std::runtime_error("Upload storage path must be relative: " + it->content);
    if (hasParentDirectoryComponent(it->content))
        throw std::runtime_error("Upload storage path cannot contain '..': " + it->content);
    location.setUploadStore(it->content);
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after upload storage path");

    ++it;
}

// Validate and parse cgi directive in location block, ensuring the CGI extension is supported and the CGI executable path is valid and accessible.

static bool isSupportedCgiExtension(const std::string &extension)
{
    static const char *const supported_extensions[] = {".py", ".php"};
    const size_t extension_count = sizeof(supported_extensions)
        / sizeof(supported_extensions[0]);

    for (size_t i = 0; i < extension_count; ++i)
    {
        if (extension == supported_extensions[i])
            return true;
    }
    return false;
}

static void parseCgiDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected CGI extension after 'cgi'");

    std::string cgi_extension = it->content;
    if (!isSupportedCgiExtension(cgi_extension))
        throw std::runtime_error("Unsupported CGI extension: " + cgi_extension);
    ++it;

    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected CGI executable after CGI path");

    std::string cgi_path = it->content;
    validateFilePath(cgi_path, "X");
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after CGI directive");

    t_cgi cgi;
    cgi.path = cgi_path;
    cgi.extension = cgi_extension;
    location.addCgi(cgi);
    ++it;
}

// Validate and parse return directive in location block, ensuring the return code is valid and the return URL is provided.

static void parseReturnDirective(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, LocationConfig &location)
{
    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected return code after 'return'");

    std::string return_code_str = it->content;
    validateReturnCode(return_code_str);
    int return_code;
    std::istringstream stream(return_code_str);
    stream >> return_code;
    ++it;

    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected return URL after return code");

    std::string return_url = it->content;
    ++it;

    if (it == end || it->type != SEMICOLON)
        throw std::runtime_error("Expected ';' after return directive");

    t_return ret;
    ret.code = return_code;
    ret.url = return_url;
    location.setReturn(ret);
    ++it;
}

static void parseLocationBlock(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end, ServerConfig &server)
{
    LocationConfig location;

    ++it;
    if (it == end || it->type != WORD)
        throw std::runtime_error("Expected location URI after 'location'");

    parseLocationURI(it, end, location);
    ++it;

    if (it == end || it->type != LBRACE)
        throw std::runtime_error("Expected '{' after location URI");

    ++it;

    while (it != end && it->type != RBRACE)
    {
        if (it->type != WORD)
            throw std::runtime_error("Unexpected token in location block: " + it->content);

        location_directive_handler handler = findLocationDirective(it->content);
        if (!handler)
            throw std::runtime_error("Unknown directive in location block: " + it->content);
        handler(it, end, location);
    }

    if (it == end || it->type != RBRACE)
        throw std::runtime_error("Expected '}' at the end of location block");

    ++it;

    if (!location.validateReturnDirective())
        throw std::runtime_error(
            "A location with 'return' cannot contain root, index, autoindex, cgi, or upload_store directives");
    if (!location.validatePost())
        throw std::runtime_error(
            "A location with 'POST' in allow_methods must have 'upload_store' directive");

    server.addLocation(location);
}

static ServerConfig parseServerBlock(std::vector<t_token>::const_iterator &it, const std::vector<t_token>::const_iterator &end)
{
    ServerConfig server;

    if (it == end || it->type != LBRACE)
        throw std::runtime_error("Expected '{' after 'server'");

    ++it;

    while (it != end && it->type != RBRACE)
    {
        if (it->type != WORD)
            throw std::runtime_error("Unexpected token in server block: " + it->content);

        server_directive_handler handler = findServerDirective(it->content);
        if (!handler)
            throw std::runtime_error("Unknown directive: " + it->content);
        handler(it, end, server);
    }

    if (it == end || it->type != RBRACE)
        throw std::runtime_error("Expected '}' at the end of server block");

    ++it;

    if (!server.has_listen_set())
        throw std::runtime_error("Missing 'listen' directive in server block");
    if (!server.has_location_set())
        throw std::runtime_error("Missing 'location' block in server block");

    server.resolveLocationRoots();
    validateLocationPaths(server);
    return server;
}

std::vector<ServerConfig> ConfigParser::parse(const std::vector<t_token> &tokens)
{
    std::vector<ServerConfig> servers;
    std::vector<t_token>::const_iterator it = tokens.begin();
    while (it != tokens.end())
    {
        if (it->type == WORD && it->content == "server")
        {
            ++it;
            servers.push_back(parseServerBlock(it, tokens.end()));
        }
        else
            throw std::runtime_error("Expected 'server' keyword");
    }

    if (servers.empty())
        throw std::runtime_error("Configuration must contain at least one server block");

    validateUniqueServerEndpoints(servers);
    return servers;
}
