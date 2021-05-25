all: Text-Manager User

Text-Manager: Text-Manager.c linked_list.c
	gcc -g -o Text-Manager Text-Manager.c linked_list.c

User: User.c
	gcc -g -o User User.c