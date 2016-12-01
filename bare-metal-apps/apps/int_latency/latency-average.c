#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>

struct element {
	struct element *forward;
	struct element *backward;
	int *array;
};

#define ARRAYSIZE 3
#define STRSZ 512

static struct element * insert_element(struct element **header, int* array, int size){
	struct element *e;
	static struct element *last = NULL;
	
	e = malloc(sizeof(struct element));
	
	e->array = malloc(ARRAYSIZE*sizeof(int));
	
	memcpy(e->array, array, ARRAYSIZE*sizeof(int));
	
	insque(e, last);
	
	if (*header == NULL){
		*header = e;
	}
	
	last = e;
	
	return *header;
}


int getlinearray3(FILE* arq, int* array, int size){
	int i;
	char str[STRSZ];
	char *pt;
	
	if (fgets(str, sizeof(str), arq) == NULL){
		return 0;
	}
	
	pt = strtok (str,",");
	
	for(i=0; i<10 && pt!=NULL; i++){
		if(i==0){
			array[0] = atoi(pt); 
		}else if(i==7){
			array[1] = atoi(pt); 
		}else if(i==9){
			array[2] = atoi(pt); 
		}
		pt = strtok (NULL, ",");
	}
	
	return size;
	
}

int main(int argc, char**argv){
	FILE* arq;
	int run=1;
	int linearray[ARRAYSIZE];
	int oldarray[ARRAYSIZE] = {0, 0, 0};
	char str[STRSZ];
	int flag = 1;
	int flag2=0;
	unsigned int num_interrupts = 0;
	unsigned int total_delay = 0;
	struct element *header = NULL;
	struct element *p = NULL;
	struct element *aux = NULL;
	unsigned int rate = 0;
	
	
	if (argc < 2){
		fprintf(stderr, "Usage: %s <input file>", argv[0]);
		return 0;
	}
	
	if ( (arq = fopen(argv[1], "r")) == NULL){
		perror("fopen");
		return 1;
	}
	
	/* drop off the first line of the file */
	if (fgets(str, sizeof(str), arq) == NULL){
		fprintf(stderr, "Wrong file format.");
		return 0;
	}
	
	while(!feof(arq)){
		if (getlinearray3(arq, linearray, ARRAYSIZE) == 0){
			break;
		}
		
		if( linearray[1] != oldarray[1] & flag){
			
			insert_element(&header, linearray, ARRAYSIZE);
			
			while(!feof(arq) && linearray[1] != linearray[2]){
				
				memcpy((char*)oldarray, (char*)linearray, ARRAYSIZE*sizeof(int));
				
				if (getlinearray3(arq, linearray, ARRAYSIZE) == 0){
					break;
				}
				
				flag2=1;
			}
			if(flag2){
				insert_element(&header, linearray, ARRAYSIZE);
				flag2=0;
			}
			flag = 0;
			continue;
		}else{
			flag=1;
		}
		
		memcpy((char*)oldarray, (char*)linearray, ARRAYSIZE*sizeof(int));
	}
	
	p = header;
	while(p){
		printf("%d %d %d\n", p->array[0], p->array[1], p->array[2]);
		if(p->array[1] == p->array[2]){
			num_interrupts++;
		}else{
			num_interrupts++;
			aux = p;
			p = p->forward;
			if(!p) break;
			printf("%d %d %d\n", p->array[0], p->array[1], p->array[2]);
			total_delay += p->array[0] - aux->array[0];
		}
		p = p->forward;
		
	}
	
	printf("total interrupts %d, total delay %d, average %fus, rate %d\n", num_interrupts, total_delay, ((float)total_delay/num_interrupts)*(1.0/rate), rate);
	
	
	
}