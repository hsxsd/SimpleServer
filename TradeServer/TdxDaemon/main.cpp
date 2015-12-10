#include "handle.h"
#include "communication.h"

int main()
{
	if(!trader_init())
		return 0;
	handle_init_command_string();

	tcper_start();

	trader_exit();

	return 0;
}

