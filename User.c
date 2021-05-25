#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "shared_items_def.h"

/**
Chase Badalato
101072570

2021-03-017

This program acts as a server, receiving commands
from a client and manipulating a string based on
inputted commands

**/

int main()
{

	int to_manager;							//The ID of the output queue
	int from_manager;						//The ID of the input message queue
	struct data_struct outgoing_data;		//the data that is to be send to the manager
	struct ack_struct incoming_data;		//received from manager
	int my_msg_type = 1;					//message received type is 1

	//Create a message queue to send messages to the text manager
	to_manager = msgget((key_t) KEY_OUTGOING, 0666 | IPC_CREAT);
	if (to_manager == -1)
	{
		fprintf(stderr,"Could not set up a message queue: %d\n",errno);
		exit(EXIT_FAILURE);
	}
	//Create a message queue to receive messages from the text manager
	from_manager = msgget((key_t) KEY_INCOMING, 0666 | IPC_CREAT);
	if (from_manager == -1)
	{
		fprintf(stderr,"Could not set up a message queue: %d\n",errno);
		exit(EXIT_FAILURE);
	}	

	/*************************
			Main Loop
	**************************/
	while(1) {
		//Gather the command the user would like to use
		display_options(&outgoing_data.command);

		//If the user would like to exit the program, delete queues and exit
		if(outgoing_data.command == 6) {
			delete_queues(to_manager, from_manager);
		}

		if (outgoing_data.command != 5)
		{
			get_text(outgoing_data.text);
		}

		//Set the message type to 1 as required with message queue structs;
		outgoing_data.my_msg_type = my_msg_type;

		//send the data to the Text-Manager
		if(msgsnd(to_manager, (void*) &outgoing_data, sizeof(outgoing_data.command) + sizeof(outgoing_data.text), 0) == -1)
		{
			fprintf(stderr,"msgsnd failed.%d\n", errno); 
			exit(EXIT_FAILURE);
		}	
		//receive an acknowledgement from the Text-Manager
		if(msgrcv(from_manager, (void*) &incoming_data, sizeof(incoming_data), 0, 0) == -1)
		{
			fprintf(stderr,"msgrcv failed with error: %d\n",errno);
			exit(EXIT_FAILURE);
		}
		//Check the ack response to see if the command was successful
		unpack_ack(outgoing_data.command, incoming_data);
	}

}

/*
 * Function: display_options
 * --------------------
 *
 * Display the commands that the user can choose.  The value gets
 * saved into the passed pointer to the command variable
 *
 * int *command : Where the command choice will be saved
 * 
 */
void display_options(int *command) {
	printf("\n");
	printf("Please enter a number corresponding to a command\n");
	printf("1) Append\n");
	printf("2) Delete\n");
	printf("3) Remove\n");
	printf("4) Search\n");
	printf("5) Compute Average\n");
	printf("6) Exit\n");

	printf("Choice: ");
	scanf("%d", command);
}

/*
 * Function: get_text
 * --------------------
 *
 * Get the sentence / word that the user wants to use
 *
 * char *text : Where the sentence will be saved in to
 * 
 */
void get_text(char *text) {
	printf("Please enter some text\n");
	printf("Text: ");
	scanf(" %[^'\n']s",text); 
	if(strlen(text) > MAX_CHARACTERS) {
		printf("The length of the string is too long ...\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Function: unpack_ack
 * --------------------
 *
 * Parse through the received acknowledgment packet and 
 * decide what to do with it
 *
 * int command : the command that was sent
 * struct ack_struct ack_s : the ack_struct that was received 
 * 
 */
void unpack_ack (int command, struct ack_struct ack_s) {
	printf("\n");

	if (command == 1) {
		if (ack_s.ack == 1) {
			printf("Sentence appended!\n");
		}
		else {
			printf("Could not append sentence to the data structure\n");
		}
	}
	else if (command == 2) {
		if (ack_s.ack == 1) {
			printf("Deleted %c copies of the word from various sentences\n", ack_s.response[0]);
		}
		else {
			printf("Could not find any copies of the word in any sentences\n");
		}
	}
	else if (command == 3) {
		if (ack_s.ack == 1) {
			printf("Sentence Removed!\n");
		}
		else {
			printf("Could not find this exact sentence in data structure\n");
		}
	}
	else if (command == 4) {
		if (ack_s.ack == 1) {
			printf("First sentence with word in it: %s\n", ack_s.response);
		}
		else {
			printf("Could not find the word in any sentences\n");
		}
	}
	else if (command == 5) {
		if (ack_s.ack == 1) {
			printf("Average command execution time: %s milliseconds\n", ack_s.response);
		}
		else {
			printf("You must complete all 4 commands at least once to get an average\n");
		}
	}
}

/*
 * Function: delete_queues
 * --------------------
 *
 * Delete the message queues when the user is done with them
 * for safety reasons
 *
 * int to_manager : the key that is used to access outgoing data
 * int from_manager : the key that is used to access incoming data
 * 
 */
void delete_queues(int to_manager, int from_manager) {
    if (msgctl(to_manager, IPC_RMID, 0) == -1 ) 
    {
		fprintf(stderr,"msgctl(IPC_RMID) failed.\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);

    if (msgctl(from_manager, IPC_RMID, 0) == -1 ) 
    {
		fprintf(stderr,"msgctl(IPC_RMID) failed.\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}