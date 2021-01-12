#include "ncode.h"

int main(int argc, char* argv[]){
	int nread;
	int outread;
	int writeop;
	uint8_t* buffer;
	uint32_t total = 0;
	parentToHuff* final;
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
	
	
	if((argc == 1) || (argc > 3)){
		printf("usage: ./hencode infile [ outfile ]\n");
		return 0;
	}

	buffer = safe_malloc(sizeof(char));


	nread = open(argv[1], O_RDONLY);

	outread = read(nread,buffer, 1);
	
	
	/* Check to see if there are any file arguments after the number */
	if (nread == -1){
		/*If file doesn't exist or we don't have permission to read */
		printf("%s: No such file or directory\n", argv[1]);
		return 0;
		
	}else { /* Continue as normal if we do */
	/* Call read_words(f) */
	
		if(outread == 0){
			if(argv[2] == NULL){
				writeop = 1;
			}else{
				writeop = open(argv[2],O_WRONLY|O_CREAT
								|O_TRUNC,0640);
			}
			write(writeop,&amount_of_chars,4);
			close(nread);
			close(writeop);
			return 0;
			
		}
		while(outread != 0){
				abc[buffer[0]].hold_ascii = buffer[0];
				abc[buffer[0]].amount_char += 1;
				cba[buffer[0]].hold_ascii = buffer[0];
				cba[buffer[0]].amount_char += 1;
				total ++;
				outread = read(nread,buffer, 1);
			}
	}
	
	int special = 0;
	int whatisit = 0;
	for(i = 0; i <256; i++){
		if(abc[i].amount_char != 0){
			special ++;
			whatisit = i;
		}
	}
	if(special == 1){
		if(argv[2] == NULL){
				writeop = 1;
		}else{
			writeop = open(argv[2],O_WRONLY|O_CREAT
						|O_TRUNC,0640);
		}
		write(writeop,&special,4);
		write(writeop,&whatisit,1);
		write(writeop,&(abc[whatisit].amount_char),4);
		return 0;
	}
	
	qsort(abc, 256, sizeof(parentToHuff), cmpfunc);
	
	final = create_tree(abc);
	
	qsort(cba, 256, sizeof(parentToHuff), cmpnew);
	
	lseek(nread,0,0);
	
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
		
	if(argv[2] == NULL){
		writeop = 1;
	}else{
		writeop = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0640);
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
		
	write(writeop,&amount_of_chars,4);
	
	while(head != NULL){
		write(writeop,&(head->hold_ascii),1);
		write(writeop,&(head->amount_char),4);
		head = head->next;
	
	}
	
		
	char* eight_bits = malloc(8*sizeof(char));
	eight_bits[8] = '\0';
	int keeptrack = 0;
	outread = read(nread,buffer, 1);
	while(outread != 0){
		for(i = 0; i < strlen(yup[buffer[0]]); i++){
			eight_bits[keeptrack] = yup[buffer[0]][i];
			keeptrack ++;
			if(keeptrack == 8){
					
					
			unsigned int yuh = strtol(eight_bits, NULL, 2);
				write(writeop,&yuh,1);
				eight_bits[0] = '\0';
				eight_bits[1] = '\0';
				eight_bits[2] = '\0';
				eight_bits[3] = '\0';
				eight_bits[4] = '\0';
				eight_bits[5] = '\0';
				eight_bits[6] = '\0';
				eight_bits[7] = '\0';
				eight_bits[8] = '\0';
				keeptrack = 0;
			}
		}
		outread = read(nread,buffer, 1);
	}
	
	if(keeptrack != 0){
		for(i = keeptrack; i<8; i++){
			eight_bits[i] = '0';
		}
		unsigned int pyuh = strtol(eight_bits, NULL, 2);
		write(writeop,&pyuh,1);
	}
	
		close(nread);
		close(writeop);
		return 0;
}

