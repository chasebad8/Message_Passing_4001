#ifndef SHARED_ITEMS_DEF_H
#define SHARED_ITEMS_DEF_H
#endif

#define KEY_OUTGOING 2570
#define KEY_INCOMING 2571
#define MAX_CHARACTERS 35
#define MICRO_SEC_IN_SEC 1000000

struct data_struct {
	long int my_msg_type;
	int command;
	char text[MAX_CHARACTERS];
};

struct ack_struct {
	long int my_msg_type;
	int ack;
	char response[MAX_CHARACTERS];
};

struct node {
   char data[35];
   struct node *next;
};

void delete_queues(int to_manager, int from_manager);
void display_options(int *command);
void get_text(char *text);
int isNum(int num);

struct ack_struct delegateCommand(char *text, int command, struct node** head, long int timeList[]);

struct ack_struct append_llist (char *data, struct node** head);
struct ack_struct search_llist (char* data, struct node** head);
struct ack_struct delete_llist (char* data, struct node** head);
struct ack_struct remove_llist (char* data, struct node** head);
void printList(struct node** head);
void unpack_ack (int command, struct ack_struct ack_s);
