# include "Webserv.hpp"
# include <signal.h>

void TestingEnvironment(void);

int	G_STOP_VAR = 1;

void modify (int var)
{
	var = 0;
	G_STOP_VAR = var;
}

void TestingEnvironment(void)
{
	signal(SIGINT, modify);
}
