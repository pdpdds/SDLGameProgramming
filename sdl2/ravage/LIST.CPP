#include "LIST.HPP"

#include <stdio.h>

#ifdef MEM_TRACK
#include "../Memory.h"
#endif

void list_Stack(struct list **head,void *data,int info)
{
	struct list *listNew = NULL;
	struct list *listStart = *head;
	struct list *listCurrentEnd = NULL;

    #ifdef MEM_TRACK
    listNew = (struct list *)mem_Malloc(sizeof(struct list),__LINE__,__FILE__);
    #else
	listNew = (struct list *)malloc(sizeof(struct list));
	#endif

	if(*head == NULL)
	{
		*head = listNew;
		(*head)->previous = NULL;
	}
	else
	{
		listCurrentEnd = (*head)->previous;

		(*head)->previous = listNew;

		if(listCurrentEnd == NULL)
		{
		    (*head)->next = listNew;
            listNew->previous = *head;
		}
		else
        {
            listCurrentEnd->next = listNew;
            listNew->previous = listCurrentEnd;
        }
	}

	listNew->info = info;
	listNew->data = data;
	listNew->next = NULL;

	return;
}

void list_Stack_Node(struct list **head, struct list *node, void *data, int info)
{
	struct list *listNew = node;
	struct list *listStart = *head;
	struct list *listCurrentEnd = NULL;

	if(*head == NULL)
	{
		*head = listNew;
		(*head)->previous = NULL;
	}
	else
	{
		listCurrentEnd = (*head)->previous;

		(*head)->previous = listNew;

		if(listCurrentEnd == NULL)
		{
		    (*head)->next = listNew;
            listNew->previous = *head;
		}
		else
        {
            listCurrentEnd->next = listNew;
            listNew->previous = listCurrentEnd;
        }
	}

	listNew->info = info;
	listNew->data = data;
	listNew->next = NULL;

	return;
}

/*Push the node into the front of the list, making it the head*/
void list_Push(struct list **head,void *data,int info)
{
	struct list *listNew = NULL;

    #ifdef MEM_TRACK
    listNew = (struct list *)mem_Malloc(sizeof(struct list),__LINE__,__FILE__);
    #else
	listNew = (struct list *)malloc(sizeof(struct list));
	#endif

    if(*head == NULL)
    {
        listNew->next = NULL;
        listNew->previous = NULL;
    }
    else
    {
        if((*head)->previous == NULL) /*First catch*/
            listNew->previous = *head; /*The new head->previous now points to the end of the list*/
        else
            listNew->previous = (*head)->previous; /*Same here*/
        (*head)->previous = listNew; /*Moving the previous head forward*/
        listNew->next = *head; /*Linking them together*/
    }


	listNew->info = info;
	listNew->data = data;

	*head = listNew;

	return;
}

void list_Push_SortL(struct list **head, void *data, int info)
{
	struct list *listCurrent = *head;
	struct list *listNew = NULL;
	int *sortCompare[2];

    #ifdef MEM_TRACK
    listNew = (struct list *)mem_Malloc(sizeof(struct list),__LINE__,__FILE__);
    #else
	listNew = (struct list *)malloc(sizeof(struct list));
	#endif

    if(*head == NULL)
    {
        listNew->next = NULL;
        listNew->previous = NULL;
        *head = listNew;
    }
    else
    {
        sortCompare[0] = (int *)data;
        while(listCurrent != NULL)
        {
            sortCompare[1] = (int *)listCurrent->data;
            if(*sortCompare[0] <= *sortCompare[1])
            {
                listNew->previous = listCurrent->previous;

                if(listCurrent->previous != NULL)
                    listCurrent->previous->next = listNew;

                listCurrent->previous = listNew;
                listNew->next = listCurrent;

                if(listCurrent == *head)
                    *head = listNew;

                listCurrent = NULL;
            }
            else
            {
                if(listCurrent->next == NULL)
                {
                    (*head)->previous = listNew; /*Correct the list*/
                    listCurrent->next = listNew;
                    listNew->previous = listCurrent;
                    listNew->next = NULL;

                    listCurrent = NULL;
                }
                else
                    listCurrent = listCurrent->next;
            }
        }
    }

	listNew->info = info;
	listNew->data = data;

	return;
}

/*Smallest first, based on the number given to info*/
void list_Push_Sort(struct list **head, void *data, int info)
{
	struct list *listCurrent = *head;
	struct list *listNew = NULL;

    #ifdef MEM_TRACK
    listNew = (struct list *)mem_Malloc(sizeof(struct list),__LINE__,__FILE__);
    #else
	listNew = (struct list *)malloc(sizeof(struct list));
	#endif

    if(*head == NULL)
    {
        listNew->next = NULL;
        listNew->previous = NULL;
        *head = listNew;
    }
    else
    {
        while(listCurrent != NULL)
        {
            if(info <= listCurrent->info)
            {
                listNew->previous = listCurrent->previous;

                if(listNew->previous != NULL)
                    listNew->previous->next = listNew;

                listCurrent->previous = listNew;
                listNew->next = listCurrent;

                if(listCurrent == *head)
                {
                    *head = listNew;
                    if(listNew->previous != NULL)
                    { /*Linking in a node that will become the new head, so the new end node should always 				point to null*/
                    	listNew->previous->next = NULL; /*Ie what if the second node was the end node? There would be an endless loop if this was the case*/
                    }
                    else
                    {
                        listNew->previous = listCurrent;
                    }
                }

                listCurrent = NULL;
            }
            else
            {
                if(listCurrent->next == NULL) /*We are at the end of the list*/
                {
                    (*head)->previous = listNew; /*Correct the list*/
                    listCurrent->next = listNew;
                    listNew->previous = listCurrent;
                    listNew->next = NULL;

                    listCurrent = NULL;
                }
                else
                    listCurrent = listCurrent->next;
            }
        }
    }

	listNew->info = info;
	listNew->data = data;

	return;
}

void list_Push_Node(struct list **head, struct list *node, void *data, int info)
{
	struct list *listNew = node;

    if(*head == NULL)
    {
        listNew->next = NULL;
        listNew->previous = NULL;
    }
    else
    {
        if((*head)->previous == NULL) /*First catch*/
        {
            listNew->previous = *head; /*The new head->previous now points to the end of the list*/
        }
        else
        {
            listNew->previous = (*head)->previous; /*Same here*/
        }

        (*head)->previous = listNew; /*Moving the previous head forward*/
        listNew->next = *head; /*Linking them together*/
    }

	listNew->info = info;

	listNew->data = data;

	*head = listNew;

    return;
}

void list_Pop(struct list **head)
{
	struct list *listStart = *head;

    if((*head)->next != NULL)
        (*head)->next->previous = (*head)->previous; /*Set the previous to point to the end of the list*/

    *head = (*head)->next;

    #ifdef MEM_TRACK
    mem_Free(listStart);
    #else
	free(listStart);
	#endif

	return;
}

void list_Delete_Node(struct list **head, struct list *dList)
{
    if(dList == *head)
    {
        if((*head)->next != NULL)
            (*head)->next->previous = (*head)->previous; /*Set the previous to point to the end of the list*/
        *head = (*head)->next;
    }
    else
    {
        dList->previous->next = dList->next;
        if(dList->next != NULL)
            dList->next->previous = dList->previous;

        if(dList == (*head)->previous) /*If dList is at the end*/
        {
            if(dList->previous != *head)
                (*head)->previous = dList->previous;
            else
                (*head)->previous = NULL;
        }
    }

    #ifdef MEM_TRACK
    mem_Free(dList);
    #else
	free(dList);
	#endif

    return;
}

unsigned int list_Delete_NodeFromData(struct list **head, void *data)
{
    struct list *listCurrent = *head;
    struct list *listTemp = *head;
    unsigned int numDeleted = 0;

    while(listTemp != NULL)
    {
        listTemp = listCurrent->next;
        if(listCurrent->data == data)
        {
            list_Delete_Node(head, listCurrent);
            numDeleted ++;
        }

        listCurrent = listTemp;
    }

    return numDeleted;
}

void list_Insert(struct list **head, struct list *insertBefore, void *data, int info)
{
	#ifdef MEM_TRACK
    struct list *listNew = (struct list *)mem_Malloc(sizeof(struct list),__LINE__,__FILE__);
    #else
	struct list *listNew = (struct list *)malloc(sizeof(struct list));
	#endif

	if(insertBefore == *head) //Same as placing it at the start
	{
		list_Push_Node(head, listNew, data, info);
		return;
	}
	else if(insertBefore == NULL) //Same as placing it at the end
	{
		list_Stack_Node(head, listNew, data, info);
		return;
	}

	listNew->previous = insertBefore->previous;

	insertBefore->previous->next = listNew;

	insertBefore->previous = listNew;

	listNew->next = insertBefore;

	listNew->data = data;
	listNew->info = info;

	return;
}

void list_Link_Node(struct list **head, struct list *pointNode, struct list *node)
{
    struct list *temp = NULL;

    if(pointNode == NULL)
    {
        list_Push_Node(head, node, node->data, node->info);
    }
    else if(pointNode->next == NULL)
    {
        list_Stack_Node(head, node, node->data, node->info);
    }
    else
    {
        temp = pointNode->next;

        pointNode->next = node;
        node->next = temp;
        node->previous = pointNode;

        if(node->next != NULL)
        {
            node->next->previous = node;
        }
    }

    return;
}

void list_Unlink_Node(struct list **head,struct list *uList)
{
    if(uList == *head)
    {
        if((*head)->next != NULL)
        {
            (*head)->next->previous = (*head)->previous; /*Set the previous to point to the end of the list*/
        }
        *head = (*head)->next;
    }
    else
    {
        uList->previous->next = uList->next;

        if(uList->next != NULL)
            uList->next->previous = uList->previous;

        if(uList == (*head)->previous) /*If uList is at the end*/
        {
            if(uList->previous != *head)
                (*head)->previous = uList->previous;
            else
                (*head)->previous = NULL;
        }
    }

    return;
}

void *list_Iterate(struct list **list)
{
	void *returnData = NULL;

	if(*list != NULL)
	{
		returnData = (*list)->data;
		*list = (*list)->next;
	}

	return returnData;
}

void list_SwapNode(struct list *nodeA, struct list *nodeB)
{
    /*Don't change positions just data that it holds*/
    struct list swapTemp;
    swapTemp.data = nodeA->data;
    swapTemp.info = nodeA->info;

    nodeA->data = nodeB->data;
    nodeA->info = nodeB->info;

    nodeB->data = swapTemp.data;
    nodeB->info = swapTemp.info;

    return;
}

void list_Concatenate(struct list **listA, struct list *listB)
{
    struct list *listEnd = *listA;

    if(*listA == NULL)
        *listA = listB;
    else
    {
        if(listEnd->previous != NULL)
            listEnd = listEnd->previous;

        if(listB->previous != NULL)
            (*listA)->previous = listB->previous;
        else
            (*listA)->previous = listB;

        listEnd->next = listB;
        listB->previous = listEnd;
    }

    return;
}

struct list *list_MatchCheck(struct list *head, void *data)
{
    while(head != NULL)
    {
        if(head->data == data)
            return head;

        head = head->next;
    }

    return NULL;
}

void list_Clear(struct list **listHead)
{
    while(*listHead != NULL)
        list_Pop(listHead);

    return;
}

void list_ClearAll(struct list **listHead)
{
    struct list *dataFetch;

    while(*listHead != NULL)
    {
        dataFetch = *listHead;

        #ifdef MEM_TRACK
        mem_Free(dataFetch->data);
        #else
        free(dataFetch->data);
        #endif

        list_Pop(listHead);
    }

    return;
}

struct list *list_Copy(struct list *src)
{
    struct list *cList = NULL;

    while(src != NULL)
    {
        list_Push(&cList, src->data, src->info);

        src = src->next;
    }

    return cList;
}

void list_Report(struct list *head)
{
    int x = 0;
    struct list *temp = NULL;
    printf("List report -\n");
    printf("[#]ListAdd: DataAdd Info    N: NextAdd P: PrevAdd\n");

    while(head != NULL)
    {
        x ++;
        printf("[%d][%p]: %p %d     N: %p P: %p\n", x, head, head->data, head->info, head->next, head->previous);

        temp = head;
        head = head->next;
    }

    head = temp;
    printf("Backwards - \n");
    while(head != NULL)
    {
        printf("[%d][%p]: %p %d     N: %p P: %p\n", x, head, head->data, head->info, head->next, head->previous);

        head = head->previous;
        x --;

        if(head != NULL && head->next == NULL)
            break;
    }
    return;
}

int list_Size(struct list *head)
{
    int x = 0;

    while(head != NULL)
    {
        x++;

        head = head->next;
    }

    return x;
}


