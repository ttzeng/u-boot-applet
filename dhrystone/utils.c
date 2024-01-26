#include <common.h>
#include <exports.h>

long time(long* arg)
{
	return get_timer(0) / 1000;
}

void hang(void)
{
    while (1);
}
