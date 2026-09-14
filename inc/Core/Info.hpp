#ifndef JJ_INFO_HPP
# define JJ_INFO_HPP

#include "Core/structs.hpp"

class Info
{
private:
	virtual int func(void) = 0;
public:
	const e_type type;
		Info(e_type type);
	~Info();
};

#endif