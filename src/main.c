#include "cache.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	Cache *cache = cache_create(10);
	if (!cache)
	{
		fprintf(stderr, "Failed to create cache\n");
		return 1;
	}
	network_start_server(cache);
	

	return 0;
}