/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomanuel <jomanuel@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 16:06:38 by jomanuel          #+#    #+#             */
/*   Updated: 2026/09/17 18:06:28 by jomanuel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/config/Lexer.hpp"
#include <cctype>
#include <fstream>

const char* InvalidFileException::what() const throw()
{
	return "Invalid file argument. File must be a non-directory existing with reading permissions.";
}

static bool isSymbol(const char &c)
{
	return (c == '{' || c == '}' || c == ';');
}

static t_token symbolToken(const char &curr, size_t &index, size_t &line_nr)
{
	t_token new_token;

	switch (curr)
	{
		case '{':
			new_token.type = LBRACE;
			break;

		case '}':
			new_token.type = RBRACE;
			break;

		case ';':
			new_token.type = SEMICOLON;
			break;
	}

	new_token.content = curr;
	new_token.line_nr = line_nr;
	new_token.col_nr = index;
	index++;
	return new_token;
}

static t_token wordToken(const std::string &line, size_t &index, size_t &line_nr)
{
	t_token new_token;

	new_token.type = WORD;
	new_token.content = "";
	new_token.line_nr = line_nr;
	new_token.col_nr = index;

	while (index < line.length() && !isSymbol(line[index]) && !std::isspace(static_cast<unsigned char>(line[index])) && line[index] != '#')
	{
		new_token.content += line[index];
		index++;
	}

	return new_token;
}

static std::vector<t_token> tokenizeLine(const std::string &line, size_t &line_nr)
{
	size_t index = 0;
	std::vector<t_token> line_tokens;
	size_t len = line.length();

	while (index < len)
	{
		char curr = line.at(index);

		if (curr == '#')
			break;
		else if (std::isspace(static_cast<unsigned char>(curr)))
			index++;
		else if (isSymbol(curr))
			line_tokens.push_back(symbolToken(curr, index, line_nr));
		else
			line_tokens.push_back(wordToken(line, index, line_nr));
	}
	return line_tokens;
}

std::vector<t_token> Lexer::tokenize(const std::string &fp)
{
	std::vector<t_token> tokens;
	std::string line;
	size_t line_nr = 0;

	std::ifstream f(fp.c_str());
	if (!f.is_open())
		throw InvalidFileException();

	while (std::getline(f, line))
	{
		std::vector<t_token> line_tokens = tokenizeLine(line, line_nr);
		tokens.insert(tokens.end(), line_tokens.begin(), line_tokens.end());
		line_nr++;
	}

	return tokens;
}

static std::string getTokenType(token_type type)
{
	switch (type)
	{
		case WORD: return "WORD";
		case LBRACE: return "LBRACE";
		case RBRACE: return "RBRACE";
		case SEMICOLON: return "SEMICOLON";
		default: return "UNKNOWN";
	}
}

std::ostream &operator<<(std::ostream &out, const std::vector<t_token> &src)
{
	std::vector<t_token>::const_iterator it;
	
	for (it = src.begin(); it != src.end(); it++)
		out << "Line: " << it->line_nr << " [" << getTokenType(it->type) << "] '" << it->content << "'\n";
	return (out);
}
