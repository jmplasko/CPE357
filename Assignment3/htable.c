#include "ncode.h"

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

int cmpnew(const void* a, const void* b){
	parentToHuff *huffA = (parentToHuff *)a;
	parentToHuff *huffB = (parentToHuff *)b;

	if(huffA->hold_ascii < huffB->hold_ascii){
		return -1;
	}
	if(huffA->hold_ascii > huffB->hold_ascii){
		return 1;
	}
	return 0;	
}

parentToHuff* sorting_alg(parentToHuff* new_huff, parentToHuff* old_list){
	parentToHuff* holder;
	parentToHuff* remember;
	if(((new_huff->total_of_children) 
		<= (old_list->amount_char)) || 
		((new_huff->total_of_children) 
		<= (old_list->total_of_children))){
		new_huff->next = old_list;
		return new_huff;
	}
	
	holder = old_list;
	while(old_list != NULL){
		if(old_list->amount_char != 0){
			if(new_huff->total_of_children 
			<= old_list->amount_char){
				new_huff->next = old_list;
				remember->next = new_huff;
				return holder;
			}
		}else{
			if(new_huff->total_of_children 
			<= old_list->total_of_children){
					new_huff->next = old_list;
					remember->next = new_huff;
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
	parentToHuff* head;
	parentToHuff* temp;
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
			parentToHuff* newhigh = 
			safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			newhigh->total_of_children = 
			temp->amount_char 
			+ temp->next->amount_char;
			newhigh->rightchild = temp->next;
			
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			newhigh->hold_ascii = 0;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
			
		}else if((head->amount_char == 0) 
			&& (head->next->amount_char != 0)){
			
			parentToHuff* newhigh = 
			safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			
			newhigh->total_of_children = 
			temp->total_of_children 
			+ temp->next->amount_char;
			
			newhigh->rightchild = temp->next;
			
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			newhigh->hold_ascii = 0;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}else if((head->amount_char != 0) 
			&& (head->next->amount_char == 0)){
			
			parentToHuff* newhigh = 
			safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			newhigh->total_of_children = 
			temp->amount_char 
			+ temp->next->total_of_children;
			newhigh->rightchild = temp->next;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			newhigh->hold_ascii = 0;
			
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}else{
			
			parentToHuff* newhigh = 
			safe_malloc(sizeof(parentToHuff));
			temp = head;
			head = head->next->next;
			
			newhigh->total_of_children = 
			temp->total_of_children 
			+ temp->next->total_of_children;
			newhigh->rightchild = temp->next;
			temp->next->next = NULL;
			newhigh->leftchild = temp;
			temp->next = NULL;
			newhigh->next = NULL;
			newhigh->hold_ascii = 0;
			if(head == NULL){
				head = newhigh;
			}else{
				head = sorting_alg(newhigh, head);
			}
			
			
		}
	}
	return head;
}

unsigned int find_code
(unsigned char findthis, parentToHuff* tree, char string[]){
		int len;
		len = strlen(string);
		if(tree == NULL){
			return 0;
		}
		if(tree->hold_ascii == findthis 
		&& (tree->leftchild == NULL) && (tree->rightchild == NULL)){
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