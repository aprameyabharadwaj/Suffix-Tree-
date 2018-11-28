#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_CHAR 256


#define FREQ 2.60e9

void heapify(int arr[], int n, int i);
void heapSort(int arr[], int n);
struct SuffixTreeNode *newNode(int start, int *end);
int edgeLength(struct SuffixTreeNode *n);
int walkDown(struct SuffixTreeNode *currNode);
void extendSuffixTree(int pos);
void setSuffixIndexByDFS(struct SuffixTreeNode *n, int labelHeight);
void buildSuffixTree();
struct SuffixTreeNode* traversal(char* s);
void recursiveFindIndexes(struct SuffixTreeNode * node, int* indexes, int * index_count);

void heapify(int arr[], int n, int i)
{
    int largest = i;
    int l = 2*i + 1;
    int r = 2*i + 2;
    int temp;


    if (l < n && arr[l] > arr[largest])
        largest = l;

    if (r < n && arr[r] > arr[largest])
        largest = r;


    if (largest != i)
    {
        temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest);
    }
}


void heapSort(int arr[], int n)
{

    int temp;

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);


    for (int i=n-1; i>=0; i--)
    {

        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        heapify(arr, i, 0);
    }
}



unsigned long long rdtsc()
{
  unsigned long long int x;
  unsigned a, d;

  __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

  return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}

struct SuffixTreeNode {
    struct SuffixTreeNode *children[MAX_CHAR];


    struct SuffixTreeNode *suffixLink;


    int start;
    int *end;


    int suffixIndex;
};

char text[200000];
struct SuffixTreeNode *root = NULL;


struct SuffixTreeNode *lastNewNode = NULL;
struct SuffixTreeNode *activeNode = NULL;


int activeEdge = -1;
int activeLength = 0;


int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1;




struct SuffixTreeNode *newNode(int start, int *end)
{
    struct SuffixTreeNode * node =(struct SuffixTreeNode *) malloc(sizeof(struct SuffixTreeNode));
    int i;
    for (i = 0; i < MAX_CHAR; i++)
          node->children[i] = NULL;


    node->suffixLink = root;
    node->start = start;
    node->end = end;


    node->suffixIndex = -1;
    return node;
}



int edgeLength(struct SuffixTreeNode *n) {
    return *(n->end) - (n->start) + 1;
}

int walkDown(struct SuffixTreeNode *currNode)
{

    if (activeLength >= edgeLength(currNode))
    {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void buildSuffixTree()
{
    size = strlen(text);
    int i;
    rootEnd = (int*) malloc(sizeof(int));
    *rootEnd = - 1;


    root = newNode(-1, rootEnd);

    activeNode = root;
    for (i=0; i<size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);

}

void extendSuffixTree(int pos)
{

    leafEnd = pos;


    remainingSuffixCount++;


    lastNewNode = NULL;


    while(remainingSuffixCount > 0) {

        if (activeLength == 0)
            activeEdge = pos;


          if (activeNode->children[text[activeEdge]] == NULL)
        {

            activeNode->children[text[activeEdge]] = newNode(pos, &leafEnd);


            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }

        else
        {

            struct SuffixTreeNode *next = activeNode->children[text[activeEdge]];
            if (walkDown(next))
            {

                continue;
            }

            if (text[next->start + activeLength] == text[pos])
            {

                if(lastNewNode != NULL && activeNode != root)
                {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }


                activeLength++;

                break;
            }


            splitEnd = (int*) malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;

            struct SuffixTreeNode *split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;


            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;


            if (lastNewNode != NULL)
            {

                lastNewNode->suffixLink = split;
            }


            lastNewNode = split;
        }


        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0)
        {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        }
        else if (activeNode != root)
        {
            activeNode = activeNode->suffixLink;
        }
    }
}

void setSuffixIndexByDFS(struct SuffixTreeNode *n, int labelHeight)
{
    if (n == NULL)  return;


    int leaf = 1;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {

            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight + edgeLength(n->children[i]));
        }
    }
    if (leaf == 1)
    {
        n->suffixIndex = size - labelHeight;

    }
}


void recursiveFindIndexes(struct SuffixTreeNode * node, int* indexes, int * index_count){
    if(node==NULL) return;
    int flag_child = 0;
    int i;
    for(i = 0; i < MAX_CHAR; i++){
        if((node -> children)[i] != NULL){
            flag_child = 1;
            recursiveFindIndexes((node -> children)[i], indexes, index_count);
        }
    }
    if(flag_child)
        return;
    indexes[*index_count] = node -> suffixIndex;
    (*index_count)++;
}

struct SuffixTreeNode* traversal(char* s)
{
  struct SuffixTreeNode* temp = root;
  for(int i=0;i<strlen(s);i++)
  {
    temp = temp->children[s[i]];
    if(temp==NULL) return NULL;
    for(int j=temp->start; j<(*temp->end) && i<strlen(s);j++)
    {
      if(text[j]!=s[i])
      {
        return NULL;
      }
      i += 1;
    }
  }
  return temp;
}

int main(int argc,char* argv[])
{
    unsigned long long t0,t1;
    int count = 0;
    int t;
    FILE* fp;

    if(argc<2)
    {
        printf("File name not input\n");
        return 0;
    }

    fp = fopen(argv[1],"r");

    char c;
    char* temp_text = malloc(300000*sizeof(char));

    int j = 0;
    int i = 0;
    int tc = 0;
    while ((c=fgetc(fp))!=EOF)
    {
      temp_text[i++]=c;
    }
    fclose(fp);


    scanf("%d",&t);
    strcpy(text, temp_text);
    free(temp_text);
    t0=rdtsc();
    buildSuffixTree();
    t1=rdtsc();

    printf("%lf\n", (t1-t0)/FREQ);


    for(j=0;j<t;j++){
        char* pattern = (char*)malloc(sizeof(char)*200);
        scanf(" %[^\n]",pattern);
        count=0;
        int* indexes= malloc(20000*sizeof(char));
        t0=rdtsc();
        if(strchr(pattern,'@')==NULL){
            struct SuffixTreeNode* temp = traversal(pattern);
            recursiveFindIndexes(temp,indexes,&count);
            //mergeSort(indexes,0,count-1);
            int n = sizeof(indexes)/sizeof(indexes[0]);
            heapSort(indexes,n);
            if(count!=0){
                for(int i=0;i<count;i++)
                {
                printf("%d,", indexes[i]);
                }
            }

            else
                printf("-1\n");
            free(temp);
        }

        else
            printf("-1\n");
        t1=rdtsc();
        printf("%lf\n", (t1-t0)/FREQ);
        free(pattern);
        free(indexes);
    }

  return 0;
}
