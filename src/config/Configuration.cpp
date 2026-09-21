/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 15:36:17 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/21 11:43:39 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/config/Configuration.hpp"
#include "../../inc/config/ServerConfig.hpp"

const char* InvalidFileNameException::what() const throw()
{
	return "Invalid file argument. File must have .conf extension.";
}

static void validateFileName(const std::string &fp)
{
	std::string ext = ".conf";
	
	if (ext.size() > fp.size() || fp.compare(fp.size() - ext.size(), ext.size(), ext) != 0)
		throw InvalidFileNameException();
}

Configuration::Configuration(const std::string &fp)
{
	validateFileName(fp);
	std::vector<t_token> tokens = Lexer::tokenize(fp);
	_servers = ConfigParser::parse(tokens);
}

Configuration::~Configuration() {}

const std::vector<ServerConfig>& Configuration::getServers() const { return _servers; }
