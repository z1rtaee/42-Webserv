/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 16:06:30 by jomanuel          #+#    #+#             */
/*   Updated: 2026/08/27 17:32:55 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <exception>

enum token_type {
	WORD,
	LBRACE,
	RBRACE,
	SEMICOLON
};

typedef struct s_token {
	token_type type;
	std::string content;
	size_t line_nr;
	size_t col_nr;
} t_token;

class InvalidFileException : public std::exception
{
	public:
		virtual const char *what() const throw();
};

class Lexer
{
	public:
		static std::vector<t_token> tokenize(const std::string &fp);

	private:
};

std::ostream &operator<<(std::ostream &out, const std::vector<t_token> &src);

#endif
