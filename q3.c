#include <stdio.h>
#include <stdlib.h>

int		verbose = 0;

typedef int	Que_data;		// create queue filled with ints
#define MAXVERTEX 7				// Set maximum number of vertices

#ifndef MAXVERTEX
#error Must define MAXVERTEX before including this file
#endif

#ifndef _BOOLEAN_INC
#define	_BOOLEAN_INC

enum _bool {FALSE=0, TRUE};
typedef enum _bool	BOOL;

#endif

// Symbolic names for states vertex can be in while search is underway
typedef enum vrtx_state {GREEN, GREY, RED}	VertexState;

// Graph vertex contains both vertex label and flag if visited.
typedef struct {
    char		label;		// only single character label for vertex
    VertexState	visited;	// state while search underway
} Vertex, *pVertex;


// graph variables. Note that all arrays are constant size based on whatever
// MAXVERTEX manifest is.

struct queue  {
    int			front;		// index of front element
    int			quecount;	// items in queue
    int			qarrsz;		// maximum size of array
    Que_data	*que;		// pointer to array of Que_data items
};

typedef struct queue	Queue, *pQueue;


Queue *create_queue (int size)
// Create queue.
{
    Queue	*q = NULL;
    // First, check for idiocy
    if (size > 0)	{
        Que_data	*pqd = (Que_data *)malloc(size * sizeof(Que_data));
        if (pqd != NULL)	{
            // Memory for actual queue obtained; get memory for descriptor
            q = (pQueue) malloc (sizeof (Queue));
            if (q != NULL)	{
                // Got descriptor
                q->front = 0;		// initially empty
                q->quecount = 0;	// initially empty
                q->qarrsz = size;	// array size
                q->que = pqd;		// array to store queue items
            }
        }
    }
    return q;		// either pointer to queue descriptor or NULL
}//create_queue()


void destroy_queue (Queue **ppq)
// Destroy queue.  Pass address of queue pointer returned from create(); this
// way we can ensure pointer is cleared as well.
{
    // always apply PAI rule.
    if (ppq != NULL)	{
        Queue *pq = *ppq;		// get queue descriptor
        free (pq->que);			// free queue array
        free (pq);				// free queue descriptor
        *ppq = NULL;			// zero queue descriptor tag
    }
    return;
}//destroy_queue()


BOOL isQueueEmpty (pQueue q)
// Determine if queue currently empty
{
    BOOL	rslt = FALSE;
    if (q != NULL)	{
        rslt = q->quecount == 0;
    }
    return rslt;
}//isQueueEmpty()


BOOL isQueueFull (pQueue q)
// Determine if queue is full
{
    BOOL	rslt = FALSE;
    if (q != NULL)	{
        rslt = q->quecount >= (q->qarrsz);
    }
    return rslt;
}//isStackFull()


int removefront (pQueue q, Que_data *px)
// If possible, remove front element from queue pointed to by q.
// If successful, location at px will be updated and 1 will be returned;
// otherwise 0 will be returned and que_data item at px will not be changed.
{
    int		rslt = 0;
    if (q != NULL)	{
        // Anything to remove?
        if (!isQueueEmpty (q))	{
            *px = q->que[q->front++];
            q->quecount--;
            // wrap-around?
            if (q->front >= q->qarrsz)	q->front = 0;
            rslt = 1;
        }
    }
    return rslt;
}


int insert (pQueue q, Que_data x)
// If possible, insert new data element at end of queue.  If successful,
// return 1; otherwise return 0.
{
    int		rslt = 0;
    if (q != NULL)	{
        // Room for another item?
        if (!isQueueFull(q))	{
            // there is room.  Compute index of end position
            int   rear = q->front+q->quecount;
            if (rear >= q->qarrsz)	rear = rear % q->qarrsz;
            q->que [rear] = x;
            q->quecount++;
            rslt = 1;
        }
    }
    return rslt;
}

Vertex	*Vertices[MAXVERTEX];		// array of pointers to vertices
int		adjMatrix[MAXVERTEX][MAXVERTEX];// contains edge information for all possible edges
int		vertexCount = 0;	// actual vertex count - updated as vertices added


// Basic graph functions

void resetGraph (void)
// Initialize adjacency matrix to have no edges.
// Initialize vertexCount to 0, effectively discarding any vertex information.
{
    int		row;
    for(row=0; row<MAXVERTEX; row++)	{
        int		col;
        for(col=0; col<MAXVERTEX; col++)	adjMatrix[row][col] = 0;
    }
    vertexCount = 0;
}//initGraph()


void printAdjMatrix (void)
// print simple version of adjacency matrix
{
    int		row;
    for(row=0; row<vertexCount; row++)	{
        int		col;
        for(col=0; col<vertexCount; col++)
            printf (" %5d", adjMatrix[row][col]);
        putchar ('\n');
    }
    putchar ('\n');
}//printAdjMatrix (void)


void resetVisitFlags (void)
// Reset visited flag in all vertices
{
    int	i;
    for (i=0; i<vertexCount; i++)	Vertices[i]->visited = GREEN;
}//resetVisitFlags()


void addVertex(char label)
// Add vertex with value label to vertex list
{
    Vertex	*vertex = (Vertex*) malloc(sizeof(Vertex));
    vertex->label = label;
    vertex->visited = GREEN;
    if (verbose)	printf ("  Add vertex %c index %d\n", label, vertexCount);
    Vertices[vertexCount++] = vertex;
}//addVertex()


// Code to add edges to graph. Code here can add unweighted or weighted,
// directed or non-directed edge to adjacency matrix.

#define	INVALID_NDX		(-1)

int vertex_index_from_label (char label)
// Given vertex name - its label - find it in vertex list, returning index.
// Returns -1 if label not found.
{
    int		i;
    for (i=0; i<vertexCount; i++)
        if (Vertices[i]->label == label)	return i;
    return INVALID_NDX;
}//vertex_index_from_label()


void addWeightedDirectedEdge (int start, int end, int weight)
// Add directed edge from start to end in edge array
{
    // verify indices within bounds
    if (start >= 0 && start < vertexCount &&
        end >= 0 && end <= vertexCount)	{
        adjMatrix[start][end] = weight;
        if (verbose)	{
            printf ("--Add edge between %c (%d) and %c (%d) with weight %d\n",
                    Vertices[start]->label, start,
                    Vertices[end]->label, end,
                    weight);
        }
    }
}//addWeightedDirectedEdge()


void addWeightedDirectedEdgeByLabel (char startlabel, char endlabel, int wgt)
// Add directed edge from startlabel to endlabel. Function does lookup to get
// indices for labels, and then calls routine above. (Note that our checking
// here is unnecessary, because the routine above does verification.)
{
    int		start, end;
    start = vertex_index_from_label (startlabel);
    end = vertex_index_from_label (endlabel);
    if (start == INVALID_NDX || end == INVALID_NDX)	return;
    addWeightedDirectedEdge (start, end, wgt);
}//addWeightedDirectedEdgeByLabel()


void addWeightedEdge (int start, int end, int weight)
// Add edge to edge array with weight
{
    addWeightedDirectedEdge (start, end, weight);
    addWeightedDirectedEdge (end, start, weight);
}//addWeightedEdge ()


void addEdge(int start, int end)
// Add edge to edge array. This is for unweighted graphs. Calls routine that
// adds weighted directed edge with weight of 1, in both directions.
{
    addWeightedEdge (start, end, 1);
}// addEdge()


void addEdgeByLabel (char startlbl, char endlbl)
{
    int		start, end;
    start = vertex_index_from_label (startlbl);
    end = vertex_index_from_label (endlbl);
    if (start == INVALID_NDX || end == INVALID_NDX)	return;
    addEdge (start, end);
}//addEdgeByLabel ()


void addDirectedEdge (int start, int end)
// Add directed edge from start to end in edge array
{
    addWeightedDirectedEdge (start, end, 1);
}//addDirectedEdge


void displayVertex (int vertexIndex)
// Display one vertex label with trailing blank.
{
    printf("%c ",Vertices[vertexIndex]->label);
}// displayVertex()


void addVertexLabels (char *p)
// Add multiple single character labels to vertex list, from string of labels.
{
    if (p)	{while (*p)	addVertex (*p++);}
}//addVertexLabels()


int getAdjUnvisitedVertex(int vertexIndex)
// Get adjacent unvisted vertex.
// Go through adjacency matrix vertexIndex row to find first neighbour that has
// not yet been visited.
{
    int i;
    for (i=0; i<vertexCount; i++)	{
        if	(adjMatrix[vertexIndex][i] > 0 && (Vertices[i]->visited == GREEN))	{
            return i;
        }
    }//endfor
    return INVALID_NDX;
}// getAdjUnvisitedVertex()

void breadthFirstSearch (int start)
// Do breadth-first search of global graph
{
    int		i;
    int		unvisitedVertex;
    Queue	*ToVisit = create_queue (MAXVERTEX+1);

    //mark first node as visited
    Vertices[start]->visited = GREY;

    //display vertex
    displayVertex (start);

    //insert vertex index in queue
    insert (ToVisit, start);

    while(!isQueueEmpty(ToVisit)) {
        //get vertex from front of unvisited queue
        int	tempVertex;
        removefront (ToVisit, &tempVertex);

        //no adjacent vertex found
        while((unvisitedVertex = getAdjUnvisitedVertex(tempVertex)) != -1) {
            Vertices[unvisitedVertex]->visited = GREY;
            displayVertex(unvisitedVertex);
            insert(ToVisit, unvisitedVertex);
        }
    }

    putchar ('\n');
    printf ("List of vertices that were not visited\n");
    for (i=0; i<vertexCount; i++)
        if (Vertices[i]->visited == GREEN)	displayVertex (i);
    putchar ('\n');

    //queue is empty, search is complete, reset all visited flag
    resetVisitFlags ();
    destroy_queue (&ToVisit);
}//BreadthFirstSearch()

// bfs at every node
// output number of nodes not reached
// if number of nodes not reached = 0 then it is a parent

int Mothvex(int start){
    int v = 0;
    for (int i = 0; i < V; i++) {
        if (visited[i] == false) {
            DFSUtil(i, visited);
            v = i;
        }
    }
}

int main (void) {
    int v;

    verbose = 0;            // Set to be talkative/not talkative during build

    resetGraph();
    addVertexLabels("0123456");

    addEdgeByLabel('0', '1');
    addEdgeByLabel('1', '3');
    addEdgeByLabel('1', '4');
    addEdgeByLabel('0', '2');
    addEdgeByLabel('2', '5');
    addEdgeByLabel('5', '6');
    addEdgeByLabel('6', '4');
    addEdgeByLabel('6', '0');

    verbose = 1;            // now be talkative

    if (verbose) {
        // Display resulting adjacency matrix
        printf("Adjacency Matrix:\n");
        printAdjMatrix();
        putchar('\n');
    }
}
