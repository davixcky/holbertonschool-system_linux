#include "hls.h"

/**
 * error - Print the errors with the app name and the filename
 *
 * @info: General information and state about the application
 * @filename: Name of the file who cause the error
 * Return: Errno code
 **/
int error(general_t info, char *filename)
{
	char *buff;
	char *text[] = {"cannot access", "cannot open directory"};
	char *command_name = info.argv[0];
	int index;

	errno = info.errno_value;
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
	sprintf(buff, "%s: %s %s", command_name, text[index], filename);

	perror(buff);
	free(buff);

	errno = 0;
	return (errno);
}
