/*
 * 类似wget的进度条的实现，实际就是转移符\r的使用，\r的作用是返回至行首而不换行
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
	size_t len = 100;
	char *bar = NULL;
	size_t bar_size = sizeof(char) * (len + 1);
	int i;

	bar = (char*)malloc(bar_size);
	if (bar == NULL) {
		perror("malloc failed");		
		return -1;
	}

	memset(bar, '=', bar_size - 2);
	bar[bar_size - 2] = '>';
	bar[bar_size - 1] = '\0';

	printf("%d%% [%s]\r", 0, "");
	for (i = 0; i < len; i++) {
		printf("%d%% [%s]\r", i+1, bar + len - i);
		fflush(stdout);
		usleep(200000);
	}

	printf("\n");
	free(bar);
	return 0;
}
