#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct node
{
	int data;
	struct node* next;
}Node;

typedef struct linkedlist
{
	int nNode;
	struct node* head; 
}LinkedList;

void init_ll(LinkedList* ll, int n)
{
	int i;
	Node* temp;

	ll->nNode = n;
	ll->head = NULL;

	for(i = n; i >= 1; i--)
	{
		temp = (Node*)malloc(sizeof(Node));
	
		if(!(ll->head))
		{
			temp->data = i;
			temp->next = NULL;
			ll->head = temp;
			continue;
		}

		temp->data = i;
		temp->next = ll->head;
		ll->head = temp;
	}
}

int rm_node_from_ll(LinkedList* ll, int idx)
{
	int i, retval;
	Node* follower = NULL;
	Node* iter = ll->head;
	
	if(ll->nNode)
	{
		ll->nNode--;
		for(i = 1; i < idx; i++)
		{
			follower = iter;
			iter = iter->next;
		}
	
		retval = iter->data;

		if(!follower)
		{
			ll->head = iter->next;
			free(iter);
			return retval;
		}
	
		follower->next = iter->next;
		free(iter);
		return retval;
	}
}

int shuffle(int n, int* order, int nColumn)
{
	int i;
	LinkedList ll;

	if(n > nColumn) return -1;

	memset(order, 0x00, sizeof(int) * nColumn);
	init_ll(&ll, n);

	for(i = 0; i < n; i++)
		order[i] = rm_node_from_ll(&ll, (rand()%ll.nNode)+1);

	return 0;
}

int parse(FILE* fp, char* (*arr)[2], int nColumn)
{
	char buff1[20], buff2[20];
	int n = 0, nBuff1, nBuff2;

	memset(arr, 0x00, sizeof(char*) * nColumn * 2);

	while(1)
	{
		memset(buff1, 0x00, sizeof(buff1));
		memset(buff2, 0x00, sizeof(buff2));

		if(fscanf(fp, "%s %s\n", buff1, buff2)==EOF)
			return n;

		if(n > nColumn)
			return -1;

		nBuff1 = strlen(buff1) + 1;
		nBuff2 = strlen(buff2) + 1;
		
		arr[n][0] = (char*)malloc(sizeof(char) * nBuff1);
		arr[n][1] = (char*)malloc(sizeof(char) * nBuff2);
		
		memset(arr[n][0], 0x00, nBuff1);
		memset(arr[n][1], 0x00, nBuff2);

		strncpy(arr[n][0], buff1, nBuff1);
		strncpy(arr[n][1], buff2, nBuff2);

		n++;
	}
}

int main(int argc, char** argv)
{
	int   i,j, nStd;
	char  filename[20];
	char* list[20][2];
	int	  order[20];
	FILE* fp;
	
	if(!(argc == 2 || argc == 3))
	{
		fprintf(stderr, "Usage : prg a.txt b.txt\n");
		exit(-1);
	}
	
	srand((unsigned)time(NULL));

	for(i = 1; i < argc; i++)
	{
		//open text file.
		memset(filename, 0x00, sizeof(filename));
		strncpy(filename, argv[i], strlen(argv[i]));
		fprintf(stdout, "[%s]\n", filename);
		if((fp = fopen(filename, "rt")) == NULL)
		{
			fprintf(stderr, "file '%s' open error\n", filename);
			exit(-1);
		}

		//initiat list.
		if((nStd = parse(fp, list, 20)) < 0)
		{
			fprintf(stderr, "file '%s' too much data\n", filename);
			fclose(fp);
			exit(-1);
		}
		fclose(fp);

		//shuffle.
		if(shuffle(nStd, order, 20) < 0)
		{
			fprintf(stderr, "file '%s' too much data\n", filename);
			exit(-1);
		}

		//print out.
		for(j = 0; j < nStd; j++)
		{
			fprintf(stdout, "%d %s %s\n", order[j], list[j][0], list[j][1]);
			if(j == nStd-1) fprintf(stdout, "\n");

			free(list[j][0]);
			free(list[j][1]);
		}
	}

	return 0;
}
