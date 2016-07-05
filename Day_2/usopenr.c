#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(){

int fd, k=1;
char inbu[100];

if ( (fd = open("/dev/mydev", O_RDWR))==-1 ) printf("Open err\n");


	k=read(fd,inbu,2);
	if (k<0) {perror("read err\n");}
	printf( "read %2d: %s \n", k, inbu);
	
close(fd);
}
