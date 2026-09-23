# include "Webserv.hpp"
# include <signal.h>

void FunctionalEnv(void);

int	G_STOP_VAR = 1;

void modify (int var)
{
	var = 0;
	G_STOP_VAR = var;
}

void FunctionalEnv(void)
{
	signal(SIGINT, modify);
}
