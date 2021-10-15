#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct list
{
	int info;
	void *data;

	struct list *next; /*This will always be NULL on the end node*/
	struct list *previous; /*If the node is the head, then this points to the end node*/
};

void list_Stack(struct list **head,void *data,int info);

void list_Stack_Node(struct list **head,struct list *node,void *data,int info);

void list_Push(struct list **head,void *data,int info);

void list_Push_SortL(struct list **head, void *data, int info);

void list_Push_Sort(struct list **head, void *data, int info);

void list_Push_Node(struct list **head,struct list *node,void *data,int info);

void list_Pop(struct list **head);

void list_Delete_Node(struct list **head,struct list *dList);

unsigned int list_Delete_NodeFromData(struct list **head, void *data);

void list_Insert(struct list **head, struct list *insertBefore, void *data, int info);

void list_Link_Node(struct list **head, struct list *pointNode, struct list *node);

void list_Unlink_Node(struct list **head, struct list *uList);

void list_SwapNode(struct list *nodeA, struct list *nodeB);

void *list_Iterate(struct list **list);

struct list *list_MatchCheck(struct list *head, void *data);

void list_Concatenate(struct list **listA, struct list *listB);

void list_Clear(struct list **listHead);

void list_ClearAll(struct list **listHead);

struct list *list_Copy(struct list *src);

void list_Report(struct list *head);

int list_Size(struct list *head);

#endif


