/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 16:01:39 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 11:01:53 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include "ConfigTypes.hpp"
#include <set>

struct t_cgi
{
    std::string extension;
    std::string path;
};

struct t_return
{
    int code;
    std::string url;
};

class LocationConfig
{
    public:
        LocationConfig();
        ~LocationConfig();

        const std::string& getURI() const;
        const std::set<allowed_methods>& getAllowedMethods() const;
        const std::string& getRoot() const;
        const std::vector<std::string>& getIndex() const;
        bool getAutoindex() const;
        const std::string& getUploadStore() const;
        const std::vector<t_cgi>& getCgi() const;
        const t_return& getReturn() const;
        bool has_allowed_methods_set() const;
        bool has_root_set() const;
        bool has_index_set() const;
        bool has_autoindex_set() const;
        bool has_upload_store_set() const;
        bool has_cgi_set() const;
        bool has_return_set() const;

        void setURI(const std::string &uri);
        void addAllowedMethods(const allowed_methods &method);
        void setRoot(const std::string &root);
        void addIndex(const std::string &index);
        void setAutoindex(bool autoindex);
        void setUploadStore(const std::string &upload_store);
        void addCgi(const t_cgi &cgi);
        void setReturn(const t_return &ret);

        bool validateReturnDirective() const;
        bool validatePost() const;

    private:
        std::string _uri;
        std::set<allowed_methods> _allowed_methods;
        std::string _root;
        std::vector<std::string> _index;
        bool _autoindex;
        std::string _upload_store;
        std::vector<t_cgi> _cgi;
        t_return _return;
        bool _has_allowed_methods_set;
        bool _has_root_set;
        bool _has_index_set;
        bool _has_autoindex_set;
        bool _has_upload_store_set;
        bool _has_cgi_set;
        bool _has_return_set;
};

std::ostream&operator<<(std::ostream &os, const LocationConfig &config);

#endif
