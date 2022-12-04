#include <stdio.h>
#include <stdlib.h>

#define	MAXINDEX 9
int swaps = 0;
int	verbose = 0;

typedef struct date{
    int day;
    int month;
    int year;
}Date;

typedef struct sorter{
    Date d;
    int num;
}container;

int numdays(Date d){
    return 365*d.year + 30*d.month + d.day;
}

void swap (container *a, int g, int h)
{
    container  t = a[g];		// temporary to hold one value being swapped
    if (verbose)
        printf ("Swap elements at %d (%d) and %d (%d)\n",g,a[g].num,h,a[h].num);
    a[g] = a[h];
    a[h] = t;
    swaps++;
} //swap()

int partition (container  *a,  int left, int right)
{
    int	ll, rr, pivotval;
    ll = left+1;
    rr = right;
    // choose pivot to be leftmost location
    pivotval = a[left].num;

    while (ll < rr)	{
        // if one on left less than pivot, leave it alone
        if(a[ll].num <= pivotval)	{ ll++; continue;}
        // if one on right greater than pivot, leave it
        if(a[rr].num > pivotval)	{ rr--; continue;}
        // both left and right on wrong side - swap them
        swap(a, ll, rr);
    }//endwhile
    // we stop when rr and ll collide. Place pivot value such that everything
    // to left is less and everything to right is same or greater.
    if (a[ll].num < pivotval)	{
        swap(a, ll, left);
    } else	{
        swap(a, --ll, left);
    }
    return ll;
}//partition()

void printarray(char *msg, container *a, int min, int max, int numperline)
{
    // Check first for dumbness
    if ((a != NULL) && (numperline > 0))	{
        int		i, num2print;
        if (msg != NULL) {
            printf("%s\n", msg);
        }
        // Compute total number of data items to print
        num2print = 1 + max-min;
        for (i=0; i<num2print; i++)	{
            // See if we have to finish current line and start next line.
            if (i && ((i % numperline) == 0)) {
                printf("\n");
            }
            // Print number - remember counter goes from 0 but items to
            // print start at min.
            printf(" %4d", a[i+min].num);
        }
        // Close off last line
        putchar ('\n');
    } else	{
        fprintf (stderr, "Error in printarray() parameters\n");
    }
}//printarray()

int quicksort (container *a,  int left, int right)
{
    if (left < right)    {
        int  pivotndx = partition (a, left, right);

        if (verbose) printarray(NULL, a, 0, MAXINDEX, 1000);

        quicksort (a, left, pivotndx-1);
        quicksort (a, pivotndx+1, right);
    }
    return swaps;
}//quicksort()

int main(){

    int verbose = 1;
    int	rslt, max;
    int	size;

    Date arr[11] = {{20, 1, 2014}, {25, 3, 2010}, {3, 12, 1676}, {18, 11, 1982}, {19, 4, 2015},{ 9, 7, 2015}, {12, 8, 2010}, {30, 8, 2010}, {21, 1, 2014}, {20, 7, 2014}, {18, 11, 2020}};
    container containers[11];

    container *pa = containers;

    for (int i = 0;i < 11;i++){
        Date d = arr[i];
        container b = {d, numdays(d)};
        containers[i] = b;
    }
    printf ("Quicksort\n");

    printf("Before Sort:\n");
    for (int i = 0;i < 11;i++){
        printf("%d, %d, %d\n",containers[i].d.day,containers[i].d.month,containers[i].d.year);
    }

    size = 10;
    rslt = quicksort (pa, 0, size-1);

    printf("After Sort:\n");
    for (int i = 0;i < 11;i++){
        printf("%d, %d, %d\n",containers[i].d.day,containers[i].d.month,containers[i].d.year);
    }


}