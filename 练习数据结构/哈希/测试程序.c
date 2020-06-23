#include <stdio.h>
#include "哈希.h"

int main(void){
	char 串[] = "名字";
	printf("sdbm: %s --> %lld\n",串,sdbm(串));
	printf("djb2: %s --> %lld\n",串,djb2(串));
	printf("异或8: %s --> %i \n",串, 异或8(串));
	printf("adler32: %s --> %i \n",串,adler32(串));
	printf("crc32: %s --> %i \n",串,crc32(串));
	return 0;
}
