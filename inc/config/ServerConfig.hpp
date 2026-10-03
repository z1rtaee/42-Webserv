/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:59:08 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 10:51:33 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include "LocationConfig.hpp"

struct t_endpoint
{
	std::string ip;
	int port;

    bool operator<(const t_endpoint &other) const
    {
        if (ip != other.ip)
            return ip < other.ip;
        return port < other.port;
    }
};

struct t_error_page
{
    int code;
    std::string uri;
};

class ServerConfig
{
    public:
        ServerConfig();
        ~ServerConfig();

        const std::vector<t_endpoint>& getListen() const;
        const std::string& getRoot() const;
        size_t getClientMaxBodySize() const;
        const std::vector<t_error_page>& getErrorPages() const;
        const std::vector<LocationConfig>& getLocations() const;
        
        bool has_listen_set() const;
        bool has_root_set() const;
        bool has_client_max_body_size_set() const;
        bool has_location_set() const;

        void addListen(const std::string &ip, int port);
        void setRoot(const std::string &root);
        void resolveLocationRoots();
        void setClientMaxBodySize(size_t size);
        void addErrorPage(std::vector<int> codes, const std::string &uri);
        void addLocation(const LocationConfig &location);

    private:
        std::vector<t_endpoint> _listen;
        std::string _root;
        size_t _client_max_body_size;
        std::vector<t_error_page> _error_pages;
        std::vector<LocationConfig> _locations;
        bool _has_listen_set;
        bool _has_root_set;
        bool _has_client_max_body_size_set;
        bool _has_location_set;
};

std::ostream&operator<<(std::ostream &os, const ServerConfig &config);

#endif
