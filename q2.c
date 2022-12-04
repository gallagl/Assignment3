#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Definition of node in linked list. Note that Node contains pointer to
// what is expected to be a C-string. (Which, for you viola players, is not
// the string on the far left!)

typedef struct _node {
    char			*value;
    struct _node	*next;
    struct _node	*prev;
} Node, *pNode;


pNode initNode (char *pstr)
// Fetch memory for new node and make copy of string passed as
// parameter.
{
    pNode	newNode = NULL;		// will point to our new node
    if (pstr != NULL)	{
        // Confirmed that we got a string parameter - make a copy
        char	*pcopy = strdup (pstr);
        if (pcopy)	{
            // Everything OK so far ...
            newNode = (pNode)malloc(sizeof(Node));
            if (newNode == NULL)	{
                // failed to allocate node memory - give up our string copy
                free (pcopy);
            } else	{
                // OK - got copy of string and a node. Initailize the node.
                newNode->value = pcopy;
                newNode->next = NULL;
                newNode->prev = NULL;
            }
        }//endif got string copy
    }//endif valid parameter
    return newNode;
} //initNode()


void destroyNode (pNode p)
// Get rid of all the memory allocated to hold this node.
{
    if (p)	{
        free (p->value);	// get rid of string
        free (p);			// get rid of node
    }
} //destroyNode()


// ============================================================================

// Definition of a linked list of Nodes. Linked list keeps track of both
// the start (head) of the list and the end (tail) of the list.
typedef struct LinkedList {
    pNode	head;			// pointer to first node
    pNode	tail;			// pointer to last node
    int		size;			// number of nodes in list
} LinkedList, *pLinkedList;


pLinkedList	init(void)
// Create and initialize a new linked list descriptor.
// Returns NULL if memory allocation failed; if successful, returns pointer
// to initialized linked list descriptor.
{
    pLinkedList	pll = (pLinkedList) malloc(sizeof(LinkedList));
    if (pll)	{
        // Initialize fields if we got memory
        pll->size = 0;	pll->head = NULL;	pll->tail = NULL;
    }
    return pll;
}//init()


int isEmptyList (pLinkedList pll)
{
    int		rslt = 1;		// default is empty
    if ((pll != NULL) && (pll->size > 0))	rslt = 0;
    return rslt;
}//isEmpty()


void printList (pLinkedList pll)
// Print all the strings found in the nodes in the linked list
{
    if (pll != NULL)	{
        // There are nodes to print
        pNode	ptr = pll->head;
        while (ptr != NULL) {
            printf ("%s -", ptr->value);
            ptr = ptr->next;
        }//endwhile
    } else	{
        // List was empty
        printf ("--EMPTY--");
    }
    putchar('\n');
}//printList()


int find(pLinkedList pll, char *x)
// Find string x in one of the nodes stored in the linked list pll. Returns
// 1 if found, 0 if not found.
{
    int		rslt = 0;			// default is not found
    if (pll != NULL)	{
        if (pll->size > 0)	{
            // valid list pointer and list not empty. Point to first item
            // on list.
            pNode ptr = pll->head;
            // Now march through all Nodes in list looking for match
            while ((ptr != NULL) && (rslt==0))	{
                if (strcmp(ptr->value, x) == 0)	rslt = 1;
                ptr = ptr->next;
            }//endwhile
        }//endif not empty list
    }//endif valid list pointer
    return rslt;
}//find()


int addFront(pLinkedList pll, char *x)
// Add string to front of linked list. Create a node to hold the string and
// then link that node at the front of the list. Returns 1 in node was
// created and linked; returns 0 if something went wrong
{
    int		rslt = 0;				// default is error return
    pNode	newNode = initNode (x);
    printf ("        Inserting value %s at front\n", x);
    if (newNode)	{
        // successful creation of node - add it into list. Actual action
        // depends on whether this is first node.
        if (pll->size == 0) {
            // First one. Both head and tail have to be updated
            pll->head = newNode;
            pll->tail = newNode;
            // Now we count on the fresh node having two NULL pointers
        } else {
            pNode	oldFront = pll->head;// save current head pointer
            newNode->next = oldFront;	// new node points where head pointed
            oldFront->prev = newNode;	// previous front node points back to
            // newnode
            pll->head = newNode;		// list head points to new node
        }
        // In either case, list grew by 1 and the action was a success
        pll->size++;
        rslt = 1;
    }
    return rslt;
} //addFront()


char *removeEnd (pLinkedList pll)
// Remove last node from linked list, returning pointer to value stored in
// that node.
{
    char	*str = NULL;		// default is no string found
    if (pll->size > 0)	{
        // List not empty, so there is a last element
        pNode last = pll->tail;
        // Now remove it from list
        if (pll->head == last)	{
            // Both head and tail pointed to it, so it was only node in list.
            pll->head = pll->tail = NULL;
            pll->size = 0;
        } else	{
            // Wasn't only node - so just detach it. Node that pointed to it
            // has to be adjusted.
            last->prev->next = NULL;	// disconnect
            pll->size--;				// one fewer in list
        }
        // At this point, node has been extracted. Get pointer to its string
        // contents.
        str = last->value;
        // Now the node can be discarded
        free (last);
    } //endif non-empty list
    return str;
}//removeEnd()


char *removeFront (pLinkedList pll)
// Remove first node from linked list, returning pointer to value stored in
// that node.
{
    char	*str = NULL;		// default is no string found
    if (pll->size > 0)	{
        // List not empty, so there is a first element
        pNode first = pll->head;
        // Now remove it from list
        if (pll->tail == first)	{
            // Both head and tail pointed to it, so it was only node in list.
            pll->head = pll->tail = NULL;
            pll->size = 0;
        } else	{
            // wasn't only node, so just detach it
            first->next->prev = NULL;
            pll->head = first->next;
            pll->size--;
        }
        // At this point, node has been extracted. Get pointer to its string
        // contents.
        str = first->value;
        // Now the node can be discarded
        free (first);
    } //endif non-empty list
    return str;
}//removeFront()



// Definition of data structure for chained hashtable
typedef struct HashTable {
    pLinkedList	*a;			// point to array of pointers to linked lists
    int			size;		// size of the array (not the table)
    int			population;	// actual number of strings in table
    int			collisions;	// count collisions detected on insert
} HashTable, *pHashTable;


pHashTable initHashTable (int s)
{
    pHashTable	pht = (pHashTable) malloc(sizeof(HashTable));
    if (pht != NULL)	{
        // Created space for hashtable descriptor
        int		i;
        pht->size = s;		// size is the size of the linked-list array
        pht->collisions = 0;
        pht->population = 0;
        // Now set up the harray of linked list pointers for the chains
        pht->a = malloc(sizeof(LinkedList) * s);
        if (pht->a != NULL)	{
            // Initialize memory for all the linked lists
            for(i=0; i<s; i++)	pht->a[i] = init();
        } else	{
            // Didn't get needed memory - give up what we got
            free (pht);
            pht = NULL;
        }
    }
    return pht;
}//initHashTable()


unsigned long int hashcode (char *x)
// This function computes hash code for given string.  Note that hash code
// is unbounded - it needs to be reduced modulo size of hash table (array)
// used to store data. (You will see another function hash() below that
// completes the index calculation.)
{
    unsigned long int	sum = 0;
    int		i = 0;
    int l = strlen(x);
    while(x[i]!='\0'){
        sum += (x[i] * (long)pow(31,l-1-i));
        i++;
    }
    return sum;
}//hashcode()


int hash (char* x, int size)
// Index calculation. Function hashcode() computes general hash for string;
// this function turns hash into valid index, given size of array.
{
    return hashcode(x) % size;
}//hash()


void insert(HashTable *ht, char *x)
// Insert new string into hashtable
{
    int key = hash(x, ht->size);
    printf ("    Word: %s hash index: %d\n", x, key);
    // Is the slot empty?
    if (!isEmptyList(ht->a[key]))	ht->collisions++;
    addFront(ht->a[key],x);
}//insert()


int findInTable(HashTable *ht, char* x)
{
    int key = hash(x, ht->size);
    return (find(ht->a[key],x));
}


void printTable(HashTable *ht)
{
    int i;
    printf("----------\n");
    for (i = 0; i < ht->size; i++)	{
        printf("%d --> ", i);
        printList(ht->a[i]);
    }
    printf("# of Collisions=%d # entries=%d\n", ht->collisions, ht->population);
    //printf("Load Factor %%=%.2f\n----------", loadFactor(ht));
}

#define	TABSIZ	40				// define size of table


int main(){

    int size = 5;
    HashTable* h = initHashTable(size+1);

    char keys[6] = {'1','2','3','4','5','\0'};
    char values[6] = {'10','20','30','40','50','\0'};

    for (int i = 0; i < size; i++){
        insert(keys[i],values[i]);
    }


// Two versions of code to build data. The first generates random strings, the
// second (used in the lab) reads words from a file.
/*
    FILE	*fp;
    char	*p;
    char	buff [132];
    fp = fopen ("WORDLIST.TXT", "r");
    while (!feof (fp))	{
        fgets (buff, 128, fp);
        p = buff;
        while (*p)	 {
            if (*p <= ' ')	{
                *p = '\0';
                break;
            }
            p++;
        }
        printf ("Word inserted: %s\n", buff);
        insert (h, buff);
        printTable (h);
        printf ("Press return key to add another number to table:");
        fflush (stdout);
        getchar();
    }
#endif
    printTable(h);*/



    return 0;
}