/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 16:29:58 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/17 18:09:40 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <vector>
#include "Lexer.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

class ConfigParser
{
    public:
        static std::vector<ServerConfig> parse(const std::vector<t_token> &tokens);

    private:
};

#endif
