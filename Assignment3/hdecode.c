#include "ncode.h"

int main(int argc, char* argv[]){
	int nread;
	int writeop;
	unsigned int* buffer;
	uint32_t litty = 0;
	unsigned int amount_of_uniprints = 0;
	unsigned int total = 0;
	parentToHuff* final;
	parentToHuff* currentplace;
	char string[500];
	unsigned int amount_of_chars = 0;
	parentToHuff *abc = safe_malloc(256*sizeof(parentToHuff));
	parentToHuff *cba = safe_malloc(256*sizeof(parentToHuff));
	char yup[256][256];
	int i,j;
	for(i = 0; i <256; i++){
		for(j = 0; j < 256; j++){
				yup[i][j] = 0;
		}
	}

	buffer = safe_malloc(sizeof(char));
	
	if(argc > 3){
		printf("usage: ./hencode [ ( infile | - ) [ outfile ] ]\n");
		return 0;
	}
	
	if(argv[1] == NULL){
		writeop = 1;
		nread = 1;
	}else if(argv[2] == NULL ){
		if(strcmp(argv[1],"-") == 0){
			writeop = 1;
			nread = 1;
		}else{
			writeop = 1;
			nread = open(argv[1], O_RDONLY);
		}
	}else{
		if(strcmp(argv[1],"-") == 0){
			nread = 1;
			writeop = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0640);
		}else{
			nread = open(argv[1], O_RDONLY);
			writeop = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0640);
		}
	}
	
	
	/* Check to see if there are any file arguments after the number */
	if (nread == -1){
		/*If file doesn't exist or we don't have permission to read */
		printf("%s: No such file or directory\n", argv[1]);
		return 0;
		
	}else { /* Continue as normal if we do */
	/* Call read_words(f) */
		
		//if(buffer[0] == 0 )
			
		read(nread,buffer,4);
		
		amount_of_uniprints = buffer[0];
		if(amount_of_uniprints == 0){
			close(nread);
			close(writeop);
			return 0;
		}
		buffer[0] = 0;
		
		if(amount_of_uniprints == 1){
			read(nread,buffer,1);
			int special = buffer[0];			
			read(nread,buffer,4);
			int specialtot = buffer[0];			
			
			for(i = 0; i < specialtot; i++){
				write(writeop,&special,1);
			}
			close(nread);
			close(writeop);
			return 0;
		}
		
		while(amount_of_uniprints > 0){
			read(nread,buffer,1);
			
			abc[buffer[0]].hold_ascii = buffer[0];
			cba[buffer[0]].hold_ascii = buffer[0];
			
			read(nread,&litty,4);
			abc[buffer[0]].amount_char = litty;
			cba[buffer[0]].amount_char = litty;
			
			total += litty;
			amount_of_uniprints --;
		}
	}
	
	
	qsort(abc, 256, sizeof(parentToHuff), cmpfunc);
	
	final = create_tree(abc);
	
	qsort(cba, 256, sizeof(parentToHuff), cmpnew);
	
	
	parentToHuff* head = safe_malloc(sizeof(parentToHuff)); 
	parentToHuff* temp = safe_malloc(sizeof(parentToHuff));
	
	int count = 0;
	for(i = 0; i < 256; i++){
		if(cba[i].amount_char != 0){
			if(count == 0){
				head = &cba[i];
				temp = head;
				count = 1;
			}
			amount_of_chars ++;
			temp->next = &cba[i];
			temp = &cba[i];
		}
	}
		
	parentToHuff* restart = head;
		
	while(head != NULL){
		find_code(head->hold_ascii, final, string);
		strrev(string);
		strcpy(yup[head->hold_ascii], string);
		for(i=0; i<500; i++){
			string[i] = '\0';
		}
		head = head->next;
	}
	head = restart;
	
	
		
	char eight_bits[17] = {0};
	uint16_t yuh;
	i = 0;
		
	currentplace = final;
	while(total != 0){
		read(nread,&yuh, 1);

		eight_bits[0] = ((yuh >> 7) + 48);
		eight_bits[1] = (((yuh & 0x40) >> 6) + 48);
		eight_bits[2] = (((yuh & 0x20) >> 5) + 48);
		eight_bits[3] = (((yuh & 0x10) >> 4) + 48);
		eight_bits[4] = (((yuh & 0x08) >> 3) + 48);
		eight_bits[5] = (((yuh & 0x04) >> 2) + 48);
		eight_bits[6] = (((yuh & 0x02) >> 1) + 48);
		eight_bits[7] = ((yuh & 0x01) + 48);
					
					
		for(i = 0; i < 8; i++){
			
			if(eight_bits[i] == '0'){
				currentplace = currentplace->leftchild;
			}else{
				currentplace = currentplace->rightchild;
			}
			
			if((currentplace->hold_ascii != 0) || (
			(currentplace->hold_ascii == 0) 
			&& (currentplace->leftchild == NULL) 
			&& (currentplace->rightchild == NULL))){
				write(writeop,&(currentplace->hold_ascii),1);
				currentplace = final;
				total --;
				
				if(total == 0){
					close(writeop);
					close(nread);
					return 0;
				}
			}
		}		
	}

		close(writeop);
		close(nread);
		return 0;
}
