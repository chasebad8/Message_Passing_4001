#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/time.h>

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
	int to_user;						//The ID of the output queue
	int from_user;						//The ID of the input message queue
	struct ack_struct outgoing_data;	//the data that is to be send to the manager
	struct data_struct incoming_data;	//received from manager
	int my_msg_type = 1;				//my_msg_type is 1 for the struct
	struct ack_struct returnSuccess;	//the acknowledgement struct 
	struct node *head = NULL;			//the head of the linked list that stores the data
	long int averageTime; 				//the average time it takes to execute all 4 commands

	long int timeList[4] = {-1, -1, -1, -1};  //an array that has the time for each command


	outgoing_data.my_msg_type = my_msg_type;
	outgoing_data.ack = 1;

	//Check if there is already a message queue ... if so delete it
	if(msgget(KEY_INCOMING, 0666) == 1) {
		printf("Incoming queue already exists ... resetting\n");
		if (msgctl(to_user, IPC_RMID, NULL) == -1) {
			fprintf(stderr, "Message queue could not be deleted.\n");
			exit(EXIT_FAILURE);
		}		
	}

	//Check if there is already a message queue ... if so delete it
	if(msgget(KEY_OUTGOING, 0666) == 1) {
		printf("Outgoing queue already exists ... resetting\n");
		if (msgctl(from_user, IPC_RMID, NULL) == -1) {
			fprintf(stderr, "Message queue could not be deleted.\n");
			exit(EXIT_FAILURE);
		}		
	}

	//Create / connect a message queue and retreive its ID
	to_user = msgget((key_t) KEY_INCOMING, 0666 | IPC_CREAT);
	if (to_user == -1)
	{
		fprintf(stderr,"Could not set up a message queue: %d\n",errno);
		exit(EXIT_FAILURE);
	}

	//Create / connect a message queue and retreive its ID
	from_user = msgget((key_t) KEY_OUTGOING, 0666 | IPC_CREAT);
	if (from_user == -1)
	{
		fprintf(stderr,"Could not set up a message queue: %d\n",errno);
		exit(EXIT_FAILURE);
	}	

	/*************************
			Main Loop
	**************************/
	while(1) {
		printf("\nWaiting to receive.\n");

		//Wait until a message is received and place it into the incoming_data struct
		if(msgrcv(from_user, (void*) &incoming_data, BUFSIZ, 0, 0) == -1)
		{
			//If the reason for error is errno 43 (value does not exist)
			//this means that the user closed the queues, so exit
			if(errno == 43) {
				printf("The Message Queues have been deleted.\n");
				exit(EXIT_SUCCESS);
			}
			fprintf(stderr,"msgrcv failed with error: %d\n",errno);
			exit(EXIT_FAILURE);
		}
		printf("Message Received!\n");

		//parse through the incoming data and deal with it accordingly in the function
		returnSuccess = delegateCommand(incoming_data.text, incoming_data.command, &head, timeList);

		//copy the returnSuccess ack_struct into the outgoing_data struct
		outgoing_data.ack = returnSuccess.ack;
		strncpy(outgoing_data.response, returnSuccess.response, MAX_CHARACTERS);

		//Send this back to the User so it can know if it was a success or failure
		if(msgsnd(to_user, (void*) &outgoing_data, sizeof(outgoing_data), 0) == -1)
		{
			fprintf(stderr,"msgsnd failed.%d\n", errno); 
			exit(EXIT_FAILURE);
		}	
		//print the linked list after every iteration
		printList(&head);
	}
}

/*
 * Function: delegateCommand
 * --------------------
 *
 * Based on what the command entered is, send the linked list to different
 * functions that will manipulate it ... (linked_list.c)
 *hat is being used
 * struct node** head : the head of the linked list, which will be manipulated
 * 
 * return -> struct ack_struct : the ack_struct to send to the User based on the results
 */
 * char *data : Where the command choice will be saved
 * int command : the command t
struct ack_struct delegateCommand(char *data, int command, struct node** head, long int timeList[]) {
	struct timeval start, end; 			//variables for calculating execution time 
	struct ack_struct returnSuccess;
	printf("\n");
	

	//Append Data
	if 		(command == 1) {
		printf("Appending ...\n");

		gettimeofday(&start, NULL);

		returnSuccess = append_llist(data, head);

		gettimeofday(&end, NULL);
		timeList[0] = ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) 
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
	}
	//Delete Data
	else if (command == 2) {
		printf("Deleting ...\n");
		gettimeofday(&start, NULL);

		returnSuccess = delete_llist(data, head);

		gettimeofday(&end, NULL);
		timeList[1] = ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) 
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
	}
	//Remove Data
	else if (command == 3) {
		printf("Removing ...\n");
		gettimeofday(&start, NULL);

		returnSuccess = remove_llist(data, head);

		gettimeofday(&end, NULL);
		timeList[2] = ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) 
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
	}
	//Search for Data
	else if (command == 4) {
		printf("Searching ...\n");
		gettimeofday(&start, NULL);

		returnSuccess = search_llist(data, head);

		gettimeofday(&end, NULL);
		timeList[3] = ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) 
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
	}

	else if (command == 5) {
		float average;
		int found;
		for (int i = 0; i < 4; ++i)
		{	
			average += timeList[i];
			if(timeList[i] >= 0) {
				found ++;
			}
		}

		if (found != 4)
		{
			returnSuccess.ack = 0;
			printf("Need more values\n");
			returnSuccess.ack = 0;
			return returnSuccess;
		}

		average = average / 4;
		//char tmp = (average + '0');
		char tmp[35];
		sprintf(tmp, "%f", average);

		returnSuccess.ack = 1;
		strncpy(returnSuccess.response, tmp, MAX_CHARACTERS);
		printf("%s\n", returnSuccess.response);

	}

	return returnSuccess;
}	

