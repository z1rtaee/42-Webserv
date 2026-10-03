#include <string>
# include <iostream>
# include <sstream>

int	ft_STOI(std::string str)
{
	std::stringstream	stream;
	int					sep;
	int					ret = 0;

	stream.clear();
	stream.str("");


	stream << str;
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	return (ret);
}

int main()
{
    std::cout << ft_STOI("127.0.0.1") << std::endl;
}