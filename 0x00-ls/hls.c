#include "hls.h"

int main(int argc, char **argv)
{
	general_t *info;
	int status_code;

	info = malloc(sizeof(general_t));
	if (info == NULL)
		exit(1);

	info->argc = argc;
	info->argv = argv;
	info->head_parent = NULL;

	status_code = execute_hls(info);
	free(info);

	return (status_code);
}
