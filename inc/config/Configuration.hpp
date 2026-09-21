/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 15:36:57 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/18 16:54:36 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <exception>
#include <string>
#include <vector>
#include "Lexer.hpp"
#include "ConfigParser.hpp"

class InvalidFileNameException : public std::exception
{
	public:
		virtual const char *what() const throw();
};

class Configuration
{
	public:
		Configuration(const std::string &fp);
		~Configuration();

		const std::vector<ServerConfig>& getServers() const;

	private:
		std::vector<ServerConfig> _servers;
};

#endif
