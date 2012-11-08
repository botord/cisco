#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define HOST_BITS 28
#define HOST_SIZE (1UL << HOST_BITS)

static unsigned int hash_table[HOST_SIZE];

unsigned int ELFHash (char *str)
{
	unsigned int hash = 0;
	unsigned int x = 0;

	while(*str) {
		hash = (hash << 4) + (*str++);

		if((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}

	return (hash & 0x7ffffff);
}

int scan(void)
{
	char data[100];
	char host1[50], host2[50];
	unsigned int hash;
	unsigned int first = 0;
	unsigned int second = 0;
	char first_host[50];
	char second_host[50];

	FILE *fp = fopen("vpn_topo.log", "r");

	assert(fp != NULL);
	
	while (fgets(data, 100, fp) != NULL) {
		hash = ELFHash(data);
		//printf("%s\r", data);
		sscanf(data, "%[a-z, A-Z,_,0-9]->%s", host1, host2);
		if(!strcmp(host1, host2)) {	//host1 host2 are the same
			printf("%s are the same with %s\n", host1, host2);
			continue;
		}	
		if (hash_table[hash] == 0) {
			hash_table[hash] += 1;
			
			hash = ELFHash(host1);
			++hash_table[hash]; 
			if (hash_table[hash] > first) {
				if(!strcmp(host1, second_host)) { 	//the second one win
					second = first;
					first = hash_table[hash];
					strcpy(second_host, first_host);
					strcpy(first_host, host1);
				} else {			//the first one got larger
					//strcpy(first_host, host1);
					//first = hash_table[hash];
					first ++ ;
				}
			} else if (hash_table[hash] > second) {
				strcpy(second_host, host1);
				second = hash_table[hash];
			}

			hash = ELFHash(host2);
			++hash_table[hash];
			if (hash_table[hash] > first) {
				if(!strcmp(host2, second_host)) { 	//the second one win
					second = first;
					first = hash_table[hash];
					strcpy(second_host, first_host);
					strcpy(first_host, host2);
				} else {			//the first one got larger
					//strcpy(first_host, host2);
					//first = hash_table[hash];
					first ++ ;
				}
			} else if (hash_table[hash] > second) {
				strcpy(second_host, host2);
				second = hash_table[hash];
			}
				
		} else {				//already exist
			printf("exist\n");
			continue;
		}
	}

	fclose(fp);
	printf("first :\n%d\t%s\n", first, first_host);
	printf("second:\n%d\t%s\n", second, second_host);

	return 0;
}

int main()
{
	scan();
	return 0;
}
