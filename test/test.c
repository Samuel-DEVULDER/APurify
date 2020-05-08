/*
 * test.c - a small test program
 */

#include <exec/memory.h>
#include <stdio.h>
#include <stdlib.h>

extern void *AllocMem(int, int);
extern void FreeMem(void *, int);

#define NUMINT 40

/* some PASCAL-like macros */
#define NEW(p)          ((p)=(void*)AllocMem(sizeof(*(p)),MEMF_CLEAR))
#define NEWTAB(p,n)     ((p)=(void*)AllocMem(sizeof(*(p))*(n),MEMF_CLEAR))
#define DISPOSE(p)      {FreeMem(p,sizeof(*p));p=NULL;}
#define DISPOSETAB(p,n) {FreeMem(p,sizeof(*p)*(n));p=NULL;}

/* simple decision (binary) tree */
struct tree {
    struct tree *right;
    struct tree *left;
    int num;
    int depth;
};

int numint      = NUMINT;
int *tabint     = NULL;
struct tree *tr = NULL;

/* init an array of integer */
void init_tab(int **tab, int num)
{
    int i;

    NEWTAB(*tab,num);
    for(i=0;i<num;++i) (*tab)[i] = i*i ^ (i+1)*(i+2);
}

/* opposite of the above procedure */
void free_tab(int **tab, int num)
{
    DISPOSETAB(*tab, num);
}

/* sum-1 or someone ? that is the question :) */
void sum1(int *tab, int num) 
{
    int i;

    printf("\nWe'll cross array bounds:\n");
    for(i=0;i<num;++i) {
        tab[i] += tab[i+1]*2 - tab[i-1];
                 /*   ^^^          ^^^    */
                 /* array bounds crossing */
        if(i==0 || i==num-1) tab[i]=i+1;
    }
}

/* delete the decision tree */
void del_tree(struct tree *t)
{
    if(t != NULL) {
        del_tree(t->left);
        del_tree(t->right);
        DISPOSE(t);
    }
}

/* add a number to the tree */
void insert_tree(struct tree **ptree, int depth, int num)
{
    if(*ptree == NULL) {
        NEW(*ptree);
        (*ptree)->right = NULL;
        (*ptree)->left  = NULL;
        (*ptree)->depth = depth;
        (*ptree)->num   = num;
        return;
    } else if((*ptree)->num < num) {
        insert_tree(&(*ptree)->right, ++depth, num);
    } else {
        insert_tree(&(*ptree)->left,  ++depth, num);
    }
}

/* build a decistion tree from an array */
void build_tree(struct tree **t, int *tab, int num)
{
    int i;

    for(i=0;i<num;++i) {
        insert_tree(t, 0, tab[i]);
    }
}

/* find a number in the array using the tree. */
/* returns NULL if the number is not found */
struct tree *find_num(struct tree *t, int num)
{
    if(t == NULL || t->num == num) {
        if(t == NULL) printf("\nWe'll reference the NULL pointer:\n");
        return t;
    } else if(t->num < num) {
        return find_num(t->right, num);
    } else {
        return find_num(t->left,  num);
    }
}

/* fill-up a structure corresponding to a */
/* missing number.                        */
struct tree *build_fake(struct tree *t, int num)
{
    t->right = NULL;
    t->left  = NULL;
    t->num   = num;
    t->depth = -1;

    return t;
}

/* find a number in the array using the tree. */
/* returns a temporary node with depth == -1  */
/* if the number is not found */
struct tree *find_num_safe(struct tree *t, int num)
{
    if(t == NULL) {
        struct tree fake_node;
        printf("\nWe'll reference data left on the stack:\n");
        return build_fake(&fake_node, num);
    } else if(t->num == num) {
        return t;
    } else if(t->num > num) {
        return find_num_safe(t->left, num);
    } else {
        return find_num_safe(t->right, num);
    }
}

/* output the depth of a number in the tree */
void output_depth(
#ifndef __PDC__
                  struct tree *(*find_proc)(struct tree *, int),
#else
                  struct tree *(*find_proc)(),
#endif
                  struct tree *t, int num)
{
    struct tree *node;

    node = (*find_proc)(t, num);

    printf("The depth of %d is: %d\n", num, node->depth);
                           /*               ^^^^^^^^^^^      */
                           /* what happens if node == NULL ? */
}

/* find the minimum node */
struct tree *min_node(struct tree *t)
{
    if(t == NULL) {
        return t;
    } else if(t->left) return min_node(t->left);
    else return t;
}

/* extract the minimum node from the tree */
void print_min_depth(struct tree *t)
{
    struct tree *min, *n;

    NEW(min);           /* typical memory leak */
    n   = min_node(t);
    min = n;
    DISPOSE(n);         /* bug: the tree is not updated */

    printf("\nWe'll access freed memory:\n");
    /* typical beginner error */
    printf("minimum node depth %d\n",min->depth);
}

int main(void)
{
    init_tab(&tabint, numint);

    sum1(tabint, numint);

    tr = NULL;
    build_tree(&tr, tabint, numint);

    output_depth(find_num, tr, tabint[numint/2]);
    output_depth(find_num, tr, 17);

    output_depth(find_num_safe, tr, tabint[numint/2]);
    output_depth(find_num_safe, tr, 17);

    print_min_depth(tr);

    printf("\nWe'll access and free some memory twice:\n");
    del_tree(tr);tr = NULL;

    printf("\nWe'll have a mis-aligment problem:\n");
    build_tree(&tr,(void*)((ULONG)tabint+2),numint);
    del_tree(tr);tr = NULL;

    free_tab(&tabint, numint);

    printf("\nWe'll have a memory leak:\n");
}
