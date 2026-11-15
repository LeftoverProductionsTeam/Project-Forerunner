/* Real native filesystem operations, confined by the caller to disposable paths. */
#include <stdio.h>
#include <string.h>
#include "posix.h"

int main(int argc, char **argv)
{
	unsigned char buffer[65536];
	char lock_path[1100];
	posix_ulong size = 0;
	int lock, result;
	FILE *input;
	size_t length;

	if (argc < 3 || snprintf(lock_path, sizeof(lock_path), "%s.lock", argv[2]) >= (int)sizeof(lock_path))
		return 5;
	lock = posix_gear_lock(lock_path);
	if (lock < 0)
		return 2;
	if (!strcmp(argv[1], "hold"))
	{
		puts("locked");
		fflush(stdout);
		getchar();
		result = 0;
	}
	else if (!strcmp(argv[1], "try"))
		result = 0;
	else if (!strcmp(argv[1], "read") || !strcmp(argv[1], "read-small"))
	{
		result = posix_gear_read(argv[2], buffer, !strcmp(argv[1], "read-small") ? 2 : sizeof(buffer), &size);
		if (result == 1)
		{
			fwrite(buffer, 1, size, stdout);
			result = 0;
		}
		else
			result = result == 0 ? 3 : 4;
	}
	else if (argc == 4 && !strcmp(argv[1], "write"))
	{
		input = fopen(argv[3], "rb");
		if (!input)
			return 5;
		length = fread(buffer, 1, sizeof(buffer), input);
		if (ferror(input) || !feof(input))
			return 5;
		fclose(input);
		result = posix_gear_replace(argv[2], buffer, (posix_ulong)length) ? 0 : 4;
	}
	else
		result = 5;
	posix_gear_unlock(lock);
	return result;
}
