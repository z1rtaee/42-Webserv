#ifndef JJ_INFO_HPP
# define JJ_INFO_HPP

#include "Core/structs.hpp"
#include <ctime>

class Info
{
private:
	virtual int func(void) = 0;
public:
	const e_type	type;
	time_t			timestamp;

		Info(e_type type);
	~Info();
};

#endif