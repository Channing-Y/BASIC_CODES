/*
 * Translates user virtual to physical address
 * Codes refer to stressapptest-1.0.7_autoconf 
 *
 * 20151022: create
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PAGEMAP_PATH "/proc/self/pagemap"

typedef int64_t off64_t;

// Translates user virtual to physical address.
uint64_t virtualToPhysical(void *vaddr) {
	uint64_t frame, shift;
	off64_t off = ((uintptr_t)vaddr) / sysconf(_SC_PAGESIZE) * 8;
  
	// /proc/self/pagemap is available in kernel >= 2.6.25
	int fd = open(PAGEMAP_PATH, O_RDONLY);
	if (fd < 0) {
		perror("open pagemap failed");
		return 0;
	}

	if (lseek64(fd, off, SEEK_SET) != off 
		|| read(fd, &frame, 8) != 8) {
		perror("access pagemap failed");
		close(fd);
    		return 0;
	}

	close(fd);

	if (!(frame & (1LL << 63)) || (frame & (1LL << 62)))
		return 0;

	//The page frame number is in bits 0-54
	shift = (frame >> 55) & 0x3f;
	frame = (frame & 0x007fffffffffffffLL) << shift;

	return frame | ((uintptr_t)vaddr & ((1LL << shift) - 1));
}

int main(int argc, char **argv)
{
	void *buffer = NULL;
	size_t buf_size = 32;	
	uint64_t phy_addr = 0;

	buffer = malloc(buf_size);	
	if (buffer == NULL) {
		perror("malloc failed");
		return -1;
	}

/*
	if (mlock(buffer, buf_size) < 0) {
		perror("mlock failed");
	}
*/
	printf("virt addr: %#lx\n", (uint64_t)buffer);

	phy_addr = virtualToPhysical(buffer);

	printf("phy addr: %#lx\n", phy_addr);

	free(buffer);
	return 0;
}
