/*
Simple cp command implementation
for Operating Systems Bonus 4
University of Electro-Communications
Jesse Jennings
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

main(int ac,char *args[])
{
	int source = -1 ;
	int dest = -1 ;
	caddr_t source_mmap,dest_mmap;
	off_t i;
	off_t size;
	struct stat sb; //used to determine file size
	//check command syntax
	if(ac!=3)  {
		fprintf(stderr,"Usage: %s source_file dest_file\n",args[0]);
		exit(0);
   	}

	if((source=open(args[1],O_RDONLY,0666))==0)  {
		fprintf(stderr,"can't open %s\n",args[1]);
		perror(NULL);
	}
	if( fstat(source, &sb)) {
		fprintf(stderr,"can't fstat %s\n",args[1]);
	}
	i=0;
	size = sb.st_size;
	source_mmap = mmap(NULL,size,PROT_READ,MAP_PRIVATE,source,(off_t)0);
	if(source_mmap==(caddr_t)-1) {
		fprintf(stderr,"Can't mmap source\n");
		perror(NULL);
		exit(0);
	}
	if((dest=open(args[2],O_CREAT|O_RDWR,0666))==0)  {
		fprintf(stderr,"can't open %s\n",args[2]);
		perror(NULL);		
	}
	// check for empty source file	
	if(size==0) 
	{
		exit(1);
	}
	lseek(dest, size-sizeof(int),  0L);
	write(dest,(&i),sizeof(int));
	dest_mmap = mmap(NULL,size,PROT_WRITE|PROT_READ,MAP_SHARED,dest,(off_t)0);
	if(dest_mmap==(caddr_t)-1) {
		fprintf(stderr,"Can't mmap dest\n");
		perror(NULL);
		exit(0);
	}
	memcpy(dest_mmap,source_mmap,size);
	ftruncate(dest, size);

	return 1;
}
