/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 16:02:01 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 10:52:03 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/config/ServerConfig.hpp"
#include "../../inc/config/LocationConfig.hpp"
#include <sstream>

static std::string intToString(int value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

ServerConfig::ServerConfig() : 
    _client_max_body_size(1048576),
    _has_listen_set(false), 
    _has_root_set(false), 
    _has_client_max_body_size_set(false), 
    _has_location_set(false) {}

ServerConfig::~ServerConfig() {}

const std::vector<t_endpoint>& ServerConfig::getListen() const { return _listen; }

const std::string& ServerConfig::getRoot() const { return _root; }

size_t ServerConfig::getClientMaxBodySize() const { return _client_max_body_size; }

const std::vector<t_error_page>& ServerConfig::getErrorPages() const { return _error_pages; }

const std::vector<LocationConfig>& ServerConfig::getLocations() const { return _locations; }

bool ServerConfig::has_listen_set() const { return _has_listen_set; }

bool ServerConfig::has_root_set() const { return _has_root_set; }

bool ServerConfig::has_client_max_body_size_set() const { return _has_client_max_body_size_set; }

bool ServerConfig::has_location_set() const { return _has_location_set; }

static void checkDuplicateListen(const std::vector<t_endpoint>& listen, const t_endpoint& new_endpoint)
{
    for (std::vector<t_endpoint>::const_iterator it = listen.begin(); it != listen.end(); ++it)
    {
        if (it->ip == new_endpoint.ip && it->port == new_endpoint.port)
            throw std::runtime_error("Duplicate listen directive: " + new_endpoint.ip + ":" + intToString(new_endpoint.port));
    }
}

void ServerConfig::addListen(const std::string &ip, int port)
{
    t_endpoint endpoint;
    endpoint.ip = ip;
    endpoint.port = port;
    checkDuplicateListen(_listen, endpoint);
    _listen.push_back(endpoint);
    _has_listen_set = true;
}

void ServerConfig::setRoot(const std::string &root)
{
	if (_has_root_set)
		throw std::runtime_error("Duplicate 'root' directive in server block");
    _root = root;
    _has_root_set = true;
}

void ServerConfig::resolveLocationRoots()
{
    for (std::vector<LocationConfig>::iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
        if (!it->has_root_set() && !it->has_return_set())
        {
            if (_root.empty())
                throw std::runtime_error("Location block missing 'root' directive and server block has no 'root' directive");
            it->setRoot(_root);
        }
    }
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
	if (_has_client_max_body_size_set)
		throw std::runtime_error("Duplicate 'client_max_body_size' directive in server block");
    _client_max_body_size = size;
    _has_client_max_body_size_set = true;
}

static void checkDuplicateErrorPage(const std::vector<t_error_page>& error_pages, const t_error_page& new_error_page)
{
    for (std::vector<t_error_page>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
    {
        if (it->code == new_error_page.code)
            throw std::runtime_error("Duplicate error_page directive for code: " + intToString(new_error_page.code));
    }
}

void ServerConfig::addErrorPage(std::vector<int> codes, const std::string &uri)
{    
    for (std::vector<int>::const_iterator it = codes.begin(); it != codes.end(); ++it)
    {
        t_error_page error_page;
        error_page.code = *it;
        error_page.uri = uri;
        checkDuplicateErrorPage(_error_pages, error_page);
        _error_pages.push_back(error_page);
    }
}

static void checkDuplicateLocation(const std::vector<LocationConfig>& locations, const LocationConfig& new_location)
{
    for (std::vector<LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (it->getURI() == new_location.getURI())
            throw std::runtime_error("Duplicate location directive: " + new_location.getURI());
    }
}

void ServerConfig::addLocation(const LocationConfig &location)
{
    checkDuplicateLocation(_locations, location);
    _locations.push_back(location);
    _has_location_set = true;
}

std::ostream&operator<<(std::ostream &os, const ServerConfig &config)
{
    os << "ServerConfig:" << std::endl;
    os << "  Listen: ";
    for (std::vector<t_endpoint>::const_iterator it = config.getListen().begin(); it != config.getListen().end(); ++it)
    {
        os << it->ip << ":" << it->port;
        if (it + 1 != config.getListen().end())
            os << ", ";
    }
    os << std::endl;
    os << "  Root: " << config.getRoot() << std::endl;
    os << "  Client Max Body Size: " << config.getClientMaxBodySize() << std::endl;
    os << "  Error Pages: ";
    for (std::vector<t_error_page>::const_iterator it = config.getErrorPages().begin(); it != config.getErrorPages().end(); ++it)
    {
        os << it->code << " -> " << it->uri;
        if (it + 1 != config.getErrorPages().end())
            os << ", ";
    }
    os << std::endl;
    os << "  Locations: ";
    for (std::vector<LocationConfig>::const_iterator it = config.getLocations().begin(); it != config.getLocations().end(); ++it)
    {
        os << it->getURI();
        if (it + 1 != config.getLocations().end())
            os << ", ";
    }
    os << std::endl;
    return os;
}
