#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef struct parentToHuff{
	int total_of_children;
	int amount_char;
	struct parentToHuff *next;
	struct parentToHuff *leftchild;
	struct parentToHuff *rightchild;
	char hold_ascii;
} parentToHuff;

void *safe_malloc(size_t size){
	void *new;
	/* malloc returns a NULL pointer upon failure
		This checks for failure. */
	if (NULL == (new=malloc(size))){
		perror("safe_malloc: malloc failure");
		/* Exit program */
		exit(EXIT_FAILURE);
	}
	/* Returns a pointer with no associated data type */
	return new;
}

int cmpfunc(const void* a, const void* b){
	parentToHuff *huffA = (parentToHuff *)a;
	parentToHuff *huffB = (parentToHuff *)b;

	if ((huffA)->amount_char == (huffB)->amount_char){
		if(huffA->hold_ascii < huffB->hold_ascii){
			return -1;
		}
		if(huffA->hold_ascii > huffB->hold_ascii){
			return 1;
		}
		return 0;
	}
	else if (huffA->amount_char > huffB->amount_char){
		return 1;
	}
	return -1;
}

parentToHuff* sorting_alg(parentToHuff* new_huff, parentToHuff* old_list){
	parentToHuff* holder = safe_malloc(sizeof(parentToHuff));
	parentToHuff* remember = safe_malloc(sizeof(parentToHuff));
	if(((new_huff->total_of_children) <= (old_list->amount_char)) || 
		((new_huff->total_of_children) <= (old_list->total_of_children))){
		new_huff->next = old_list;
		//free(holder);
		//free(remember);
		return new_huff;
	}
	
	holder = old_list;
	while(old_list != NULL){
		if(old_list->amount_char != 0){
			if(new_huff->total_of_children <= old_list->amount_char){
				new_huff->next = old_list;
				remember->next = new_huff;
				//free(remember);
				return holder;
			}
		}else{
			if(new_huff->total_of_children <= old_list->total_of_children){
					new_huff->next = old_list;
					remember->next = new_huff;
					//free(remember);
					return holder;
			}
		}
		remember = old_list;
		old_list = old_list->next;
	}
	
	
	remember->next = new_huff;
	return holder;
}

parentToHuff* create_tree(parentToHuff* arr){
	parentToHuff* head = safe_malloc(sizeof(parentToHuff)); 
	parentToHuff* temp = safe_malloc(sizeof(parentToHuff));
	int i; 
	int count = 0;
	for(i = 0; i < 256; i++){
		if(arr[i].amount_char != 0){
			if(count == 0){
				head = &arr[i];
				temp = head;
				count = 1;
			}
			temp->next = &arr[i];
			temp = &arr[i];
		}
	}
	
	while(head->next != NULL){
		if((head->amount_char != 0) && (head->next->amount_char != 0)){
			parentToHuff* newhigh = safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			newhigh->total_of_children = temp->amount_char + temp->next->amount_char;
			newhigh->rightchild = temp->next;
			//newhigh->rightchild->amount_char = temp->next->amount_char;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			//newhigh->leftchild->amount_char = temp->amount_char;
			temp->next = NULL;
			newhigh->next = NULL;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
			
		}else if((head->amount_char == 0) && (head->next->amount_char != 0)){
			
			parentToHuff* newhigh = safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			
			newhigh->total_of_children = temp->total_of_children 
										+ temp->next->amount_char;
			
			newhigh->rightchild = temp->next;
			//newhigh->rightchild->amount_char = temp->next->amount_char;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			//newhigh->leftchild-> = temp->amount_char;
			temp->next = NULL;
			newhigh->next = NULL;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}else if((head->amount_char != 0) && (head->next->amount_char == 0)){
			
			parentToHuff* newhigh = safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			newhigh->total_of_children = temp->amount_char+ temp->next->total_of_children;
			newhigh->rightchild = temp->next;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}else{
			
			parentToHuff* newhigh = safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			
			newhigh->total_of_children = temp->total_of_children + temp->next->total_of_children;
			newhigh->rightchild = temp->next;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}
	}
	return head;
}

int find_code(char findthis, parentToHuff* tree, char string[]){
		int len;
		len = strlen(string);
		if(tree == NULL){
			return 0;
		}
		if(tree->hold_ascii == findthis){
			return 1;
		}
		
		if(find_code(findthis, tree->leftchild, string) == 1){
			len = strlen(string);
			string[len] = '0';
			return 1;
		}			
			
		if(find_code(findthis, tree->rightchild, string) == 1){
			len = strlen(string);
			string[len] = '1';
			return 1;
		}
		return 0;
}	

char *strrev(char *str){
    char c, *front, *back;

    if(!str || !*str)
        return str;
    for(front=str,back=str+strlen(str)-1;front < back;front++,back--){
        c=*front;*front=*back;*back=c;
    }
    return str;
}

int main(int argc, char* argv[]){
	FILE* f;
	int c;
	parentToHuff* final;
	char string[500];

	
	parentToHuff *abc = safe_malloc(256*sizeof(parentToHuff));
	
	
	f = fopen(argv[1], "r");
			/* Check to see if there are any file arguments after the number */
	if (f == NULL){
		/*If file doesn't exist or we don't have permission to read */
		printf("%s : %s\n", argv[1], strerror(errno));
		return 0;
	}
	/* Continue as normal if we do */
	else{
	/* Call read_words(f) */
		while((c = getc(f)) != EOF){
				//count_chars[c] += 1;
				abc[c].hold_ascii = c;
				abc[c].amount_char += 1;
			}
			fclose(f);
	}
	
	
	qsort(abc, 256, sizeof(parentToHuff), cmpfunc);

	final = create_tree(abc);
	
	parentToHuff* head = safe_malloc(sizeof(parentToHuff)); 
	parentToHuff* temp = safe_malloc(sizeof(parentToHuff));
	int i; 
	int count = 0;
	for(i = 0; i < 256; i++){
		if(abc[i].amount_char != 0){
			if(count == 0){
				head = &abc[i];
				temp = head;
				count = 1;
			}
			temp->next = &abc[i];
			temp = &abc[i];
		}
	}
	
	
	while(head != NULL){
		if(head->hold_ascii < 16){
			printf("0x0%x: ", head->hold_ascii);
		}else{
			printf("0x%x: ", head->hold_ascii);
		}
		find_code(head->hold_ascii, final, string);
		strrev(string);
		for(i = 0; i < strlen(string); i++){
			printf("%c", string[i]);
		}
		for(i=0; i<500; i++){
			string[i] = 0;
		}
		printf("\n");
		head = head->next;
	}
	
	return 0;
}