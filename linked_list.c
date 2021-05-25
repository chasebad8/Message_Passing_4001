#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "shared_items_def.h"

/**
 * Chase Badalato
 *
 * An implementation of a linked list for SYSC 4001
 * Assignment 3
 *
 * 2021-03-27
 *
**/

/*
 * Function: append_llist
 * --------------------
 *
 * Append data to the linked list.  The list must be fully
 * traversed first to append it to the END of the list (required
 * for the search and remove function to work properly)
 *
 * char *data : The data to be added to the new node
 * struct node** head : the reference to the linked list
 *
 * return -> struct ack_struct : An ack struct, which will have an ack of 1 on
 * succcess and an ack of 0 on failure
 *
 */
struct ack_struct append_llist (char *data, struct node** head) {
	struct ack_struct returnStruct; //the struct to return
	struct node* current = *head; //local copy of the linked list head node

	//allocate a new node pointer to memory
    struct node *link = (struct node*) malloc(sizeof(struct node));
	if(link == NULL) {
		returnStruct.ack = 0;
		return returnStruct;
	}

	//if the list is currently empty add right away
	if(current == NULL) {
		strncpy(link->data, data, MAX_CHARACTERS);
	    link->next = *head;
	    *head = link;		
	}
	else {
		//iterate through until the end of the list
		while(current->next != NULL) {
			current = current->next;
		}
		//copy the values required into the new node
		strncpy(link->data, data, MAX_CHARACTERS);
    	current->next = link;
	}
	returnStruct.ack = 1;
	return returnStruct;
}

/*
 * Function: search_llist
 * --------------------
 *
 * Search through the linked list for a sentence that contains a given work
 * If found, return that sentence in the ack packet, and return
 *
 * char *data : The word to be searched for
 * struct node** head : the reference to the linked list
 *
 * return -> struct ack_struct : An ack struct, which will have an ack of 1 on
 * succcess and an ack of 0 on failure
 *
 */
struct ack_struct search_llist (char* data, struct node** head) {
	struct ack_struct returnStruct; //the return struct
    struct node* current = *head; //a copy of the head of the linked list

    //If the list is empty, search failed
    if(*head == NULL) {
		returnStruct.ack = 0;
    	return returnStruct;
    }

    //while the current value is not null
    while(current != NULL) {

    	//check is there is a word withing the current nodes data (char array)
		char *tmp = strstr(current->data, data);

		//if the word was found then return success!
		if(tmp != NULL) {
			strncpy(returnStruct.response, current->data, MAX_CHARACTERS);
			returnStruct.ack = 1;
			return returnStruct;
		}
        current = current->next;
    }
    //if is reaches the end of the list and it is not found, return failure
	returnStruct.ack = 0;
	return returnStruct;      
}

/*
 * Function: delete_llist
 * --------------------
 *
 * Search through the entire list and delete all instances of the word. 
 * Ex List = {hello, help, hey}, Word = he, newList = {llo, lp, y} 
 *
 * char *data : The word to be deleted
 * struct node** head : the reference to the linked list
 *
 * return -> struct ack_struct : An ack struct, which will have an ack of 1 on
 * succcess and an ack of 0 on failure
 *
 */
struct ack_struct delete_llist (char* data, struct node** head) {
	struct ack_struct returnStruct; //the return ack struct
	struct node* current = *head; //a copy of the head of the linked list
    int found = 0; //the number of times that this value was found
	
	//if the head is empy then fail right away
    if(*head == NULL) {
		returnStruct.ack = 0;
		return returnStruct;
    }

	while(current != NULL) {
		//If the word is found in the data, then remove the data
	    size_t len = strlen(data);
	    if (len > 0 && strstr(current->data, data) != NULL) {
	    	found ++;
	        char *p = current->data;
	        while ((p = strstr(p, data)) != NULL) {
	            memmove(p, p + len, strlen(p + len) + 1);
	        }
	    }
	    //either way, continue through the list
        current = current->next;
    }
    //if at the end at least 1 value was found, return success 
    //with the number of successful values deleted
    if(found > 0) {
		returnStruct.ack = 1;
		returnStruct.response[0] = (char) (found + '0');
		return returnStruct;
	}
	//else return a faliure ack
	else {
		returnStruct.ack = 0;
		return returnStruct;		
	}
}

/*
 * Function: remove_llist
 * --------------------
 *
 * Iterate through the list and remove the first instance of an exact sentence
 *
 * char *data : The sentence to be removed
 * struct node** head : the reference to the linked list
 *
 * return -> struct ack_struct : An ack struct, which will have an ack of 1 on
 * succcess and an ack of 0 on failure
 *
 */
struct ack_struct remove_llist (char* data, struct node** head) {
	struct ack_struct returnStruct; //the return ack struct
	struct node* current = *head; //a copy of the head of the linked list
    struct node* previous = NULL; //the value of the previous node (needed for deleting)
	
	//if the linked list is size 0 return failure
    if(*head == NULL) {
		returnStruct.ack = 0;
		return returnStruct;
    }
	while(strcmp(current->data, data) != 0) {
		//if the next node is 0 then return a failure
        if(current->next == NULL) {
			returnStruct.ack = 0;
			return returnStruct;
        } 
        else {
        	previous = current;
        	current = current->next;
        }
    }

    if(current == *head) {
    	*head = (*head)->next;
    } 
    else {
    	previous->next = current->next;
    }  
	returnStruct.ack = 1;
	return returnStruct; 
}

/*
 * Function: printList
 * --------------------
 *
 * Print the linked list
 *
 * struct node** head : the reference to the linked list
 *
 *
 */
void printList(struct node** head) {
   struct node *ptr = *head;
   printf("\n[ ");
	
   while(ptr != NULL) {
      printf("(%s) ",ptr->data);
      ptr = ptr->next;
   }
	
   printf(" ]\n");
}