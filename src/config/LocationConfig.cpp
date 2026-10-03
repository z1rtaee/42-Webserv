/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 16:01:58 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 11:01:58 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/config/LocationConfig.hpp"

LocationConfig::LocationConfig() :
	_autoindex(false),
	_return(),
	_has_allowed_methods_set(false),
	_has_root_set(false),
	_has_index_set(false),
	_has_autoindex_set(false),
	_has_upload_store_set(false),
	_has_cgi_set(false),
	_has_return_set(false)
{
	_allowed_methods.insert(GET);
	_index.push_back("index.html");
}

LocationConfig::~LocationConfig() {}

const std::string& LocationConfig::getURI() const { return _uri; }

const std::set<allowed_methods>& LocationConfig::getAllowedMethods() const { return _allowed_methods; }

const std::string& LocationConfig::getRoot() const { return _root; }

const std::vector<std::string>& LocationConfig::getIndex() const { return _index; }

bool LocationConfig::getAutoindex() const { return _autoindex; }

const std::string& LocationConfig::getUploadStore() const { return _upload_store; }

const std::vector<t_cgi>& LocationConfig::getCgi() const { return _cgi; }

const t_return& LocationConfig::getReturn() const { return _return; }

bool LocationConfig::has_allowed_methods_set() const { return _has_allowed_methods_set; }

bool LocationConfig::has_root_set() const { return _has_root_set; }

bool LocationConfig::has_index_set() const { return _has_index_set; }

bool LocationConfig::has_autoindex_set() const { return _has_autoindex_set; }

bool LocationConfig::has_upload_store_set() const { return _has_upload_store_set; }

bool LocationConfig::has_cgi_set() const { return _has_cgi_set; }

bool LocationConfig::has_return_set() const { return _has_return_set; }

void LocationConfig::setURI(const std::string &uri) { _uri = uri; }

void LocationConfig::addAllowedMethods(const allowed_methods &method) {
	if (!_has_allowed_methods_set) {
		_has_allowed_methods_set = true;
		_allowed_methods.clear();
	}
	_allowed_methods.insert(method);
}

void LocationConfig::setRoot(const std::string &root) {
	if (_has_root_set)
		throw std::runtime_error("Duplicate 'root' directive in location block");
	_root = root;
	_has_root_set = true;
}

void LocationConfig::addIndex(const std::string &index)
{
	if (!_has_index_set) {
		_has_index_set = true;
		_index.clear();
	}
	_index.push_back(index);
}

void LocationConfig::setAutoindex(bool autoindex)
{
	if (_has_autoindex_set)
		throw std::runtime_error("Duplicate 'autoindex' directive in location block");
	_autoindex = autoindex;
	_has_autoindex_set = true;
}

void LocationConfig::setUploadStore(const std::string &upload_store)
{
	if (_has_upload_store_set)
		throw std::runtime_error("Duplicate 'upload_store' directive in location block");
	_upload_store = upload_store;
	_has_upload_store_set = true;
}

void LocationConfig::addCgi(const t_cgi &cgi)
{
	for (std::vector<t_cgi>::const_iterator it = _cgi.begin(); it != _cgi.end(); ++it)
	{
		if (it->extension == cgi.extension)
			throw std::runtime_error("Duplicate CGI extension: " + cgi.extension);
	}
	_has_cgi_set = true;
	_cgi.push_back(cgi);
}

void LocationConfig::setReturn(const t_return &ret)
{
	if (_has_return_set)
		throw std::runtime_error("Duplicate 'return' directive in location block");
	_return = ret;
	_has_return_set = true;
}

bool LocationConfig::validateReturnDirective() const
{
	if (!_has_return_set)
		return true;

	if (_has_root_set || _has_index_set || _has_autoindex_set || _has_cgi_set || _has_upload_store_set)
		return false;

	return true;
}

bool LocationConfig::validatePost() const
{
	if (_allowed_methods.find(POST) != _allowed_methods.end() && !_has_upload_store_set)
		return false;
	return true;
}

std::ostream&operator<<(std::ostream &os, const LocationConfig &config)
{
	os << "LocationConfig:" << std::endl;
	os << "  URI: " << config.getURI() << std::endl;
	os << "  Allowed Methods: ";
	
	for (std::set<allowed_methods>::const_iterator it = config.getAllowedMethods().begin(); it != config.getAllowedMethods().end(); ++it)
	{
		switch (*it)
		{
			case GET:
				os << "GET ";
				break;
			case POST:
				os << "POST ";
				break;
			case DELETE:
				os << "DELETE ";
				break;
		}
	}
	os << std::endl;
	
	os << "  Root: " << config.getRoot() << std::endl;
	os << "  Index: ";
	
	for (std::vector<std::string>::const_iterator it = config.getIndex().begin(); it != config.getIndex().end(); ++it)
		os << *it << " ";
	os << std::endl;
	
	os << "  Autoindex: " << (config.getAutoindex() ? "ON" : "OFF") << std::endl;
	os << "  Upload Store: " << config.getUploadStore() << std::endl;
	os << "  CGI: ";
	
	for (std::vector<t_cgi>::const_iterator it = config.getCgi().begin(); it != config.getCgi().end(); ++it)
		os << "(" << it->extension << ", " << it->path << ") ";
	os << std::endl;
	
	if (config.has_return_set())
		os << "  Return: (" << config.getReturn().code << ", " << config.getReturn().url << ")" << std::endl;
	else
		os << "  Return: Not set" << std::endl;

	return os;
}
