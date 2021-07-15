#include "hls.h"

int error(general_t info, char *filename)
{
	char *buff;
	char *text[] = {"cannot access", "cannot open directory"};
	char *command_name = info.argv[0];
	int index;

	switch (errno)
	{
		case ENOENT:
			index = 0;
			break;
		case EACCES:
			index = 1;
			break;
	}

	buff = malloc(_strlen(command_name) +
			_strlen(text[index]) +
			_strlen(filename) +
			6);
	sprintf(buff, "%s: %s '%s'", command_name, text[index], filename);

	perror(buff);
	free(buff);

	return (errno);
}
