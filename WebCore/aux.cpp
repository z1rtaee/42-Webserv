#include "local_core.hpp"

void	jj_memset(char *str, int size)
{
	for (int ind = 0; ind < size; ind++)
		str[ind] = 0;
}