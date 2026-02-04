#include<stdio.h>
#include<stdlib.h>
#define T 9  /* Value that defines the puzzle size */
int count=0; /* Variable that counts the number of values inserted in matrix "S" */
int contradiction=0; /* Flag indicating contradiction during propagation */

            /*#############*/
/*========================================
 Data STRUCTURE for each node in the lists.
 This structure has a variable that will
 receive a number between 1 and "T" and also
 contains a pointer to the next node
 ========================================*/
typedef struct dados
{
    int n;
    struct dados *next;
}data;
/*=======================================*/

typedef data *nodePtr; /* Declaration of data type "nodePtr" which is a pointer */
                       /* to a structure of the type previously declared        */

data S[T][T];  /* declaration of a "T" by "T" matrix                 */


                    /*///////////////////////////////////*/
/*==============================================================================
 Function that INITIALIZES a list for each index of the matrix. This list
 will have "T" nodes where each node holds a number between 1 and "T".
 =============================================================================*/
void init()
{
    nodePtr ap,tmp;
    int l,c,i;
    for (l=0;l<T;l++)          /* traverses each row until the end             */
        for (c=0;c<T;c++)      /* all matrix indices are traversed             */
        {
            S[l][c].n=0;       /* the value 0 represents a blank space         */
            S[l][c].next=(data*)malloc(sizeof(data));/* Creation of the first node */
            tmp=S[l][c].next;
            for (i=1;i<T;i++)
            {
                ap=(data*)malloc(sizeof(data)); /* Here the remaining nodes   */
                (*tmp).next=ap;                 /* are created, where each    */
                (*tmp).n=i;                     /* node receives a number     */
                tmp=ap;
            }
            (*tmp).n=T;
            (*tmp).next=NULL;    /* The last node will point to "NULL"          */
        }
}
/*==== #ini ==================================================================*/





                    /*///////////////////////////////////*/
/*==============================================================================
 Function that frees all nodes associated with each cell of S
 =============================================================================*/
void freeAll()
{
    nodePtr tmp2,tmp;
    int l,c,i;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
        {
            tmp=S[l][c].next;
            tmp2=tmp;
            while(tmp!=NULL)
            {
                tmp=(*tmp2).next;
                free(tmp2);
                tmp2=tmp;
            }
            if (tmp2!=NULL)
                free (tmp2);
            S[l][c].next=NULL;
        }
}
/*==== #lib ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 Function that removes the value "val" from the list associated with cell [l][c]
 of matrix S.
 =============================================================================*/
int rem(int l,int c,int val)
{
    nodePtr tmp,tmp2;
    if (contradiction) return 0;
    tmp=&S[l][c];
    tmp2=(*tmp).next;
    /*printf("[%i][%i] - %i\n",l+1,c+1,val);*/
    if (tmp2==NULL)
        return 0;
    while ((*tmp2).next!=NULL && (*tmp2).n!=val)
    {
        tmp=(*tmp).next;
        tmp2=(*tmp2).next;
    }
    if (tmp2==NULL)
    {
        (*tmp).next=(*tmp2).next;
        free(tmp2);
    }
    else
        if ((*tmp2).n==val)
        {
            (*tmp).next=(*tmp2).next;
            free(tmp2);
        }
    tmp=S[l][c].next;
    if (tmp==NULL) { contradiction=1; return 0; }
    tmp2=(*tmp).next;
    if(tmp2==NULL)
    {
        S[l][c].n=(*tmp).n;
        count++;             /* the counter is incremented */
        S[l][c].next=NULL;
        free(tmp);
        tmp=NULL;
        return S[l][c].n;
    }
    else
        return 0;
}
/*==== #rem ==================================================================*/




                    /*///////////////////////////////////*/
/*==============================================================================
 Function that traverses row "l", column "c" and the subgrid associated with [l][c]
 =============================================================================*/
void check (int l,int c,int val)
{
    int k,x,i;
    if (contradiction) return;
    for (k=0;k<T;k++)/* traverses the columns */
    {
        x=rem(l,k,val);    /* When "val" is removed and it is the only element  */
        if (x)             /* in the list, that value is placed in the          */
        {                   /* corresponding cell of the matrix, this function   */
                           /* is called again, with the value to remove being   */
            check(l,k,x);  /* what the function returns  (A)                    */
        }
    }

    if (contradiction) return;
    for (k=0;k<T;k++)/* traverses the rows   */
    {
        x=rem(k,c,val);
        if (x)             /*       (A)     */
            check(k,c,x);
    }

    if (contradiction) return;
    while (l!=6 && l!=3 && l!=0)
        l--;
    while (c!=6 && c!=3 && c!=0)
        c--;
    for (k=l;k<l+3;k++)      /* In these two "for" loops   */
        for (i=c;i<c+3;i++)  /* the subgrids are traversed */
        {
            x=rem(k,i,val);
            if (x)          /*       (A)     */
                check(k,i,x);
        }
}
/*==== #chk ==================================================================*/



                    /*///////////////////////////////////*/
/*==============================================================================
 Function that traverses the elements of row "l", except the elements associated
 with the subgrid containing the element at index [l][j].
 =============================================================================*/
elemRow(int l,int val,int j)
{
    int x,i,tmp;
    tmp=j;
    while (j!=6 && j!=3 && j!=0)
        j--;
    for (i=0;i<T;i++)/* traverses the columns */
        if (i!=j && i!=j+1 && i!=j+2 && S[i][tmp].n==0)
        {
            x=rem(l,i,val);
            if (x)
                check(l,i,x);
        }
}
/*==== #ell ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 Function that traverses the elements of column "c", except the elements associated
 with the subgrid containing the element at index [k][c].
 =============================================================================*/
elemCol(int c,int val,int k)
{
    int i,x,tmp;
    tmp=k;
    while (k!=6 && k!=3 && k!=0)
        k--;
    for (i=0;i<T;i++)/* traverses the rows   */
        if (i!=k && i!=k+1 && i!=k+2 && S[tmp][i].n==0)
        {
            x=rem(i,c,val);
            if (x)            /*       (A)     */
                check(i,c,x);
        }
}
/*==== #elc ==================================================================*/




                    /*///////////////////////////////////*/
/*==============================================================================
 Two functions that print the matrix values
 =============================================================================*/
numbers(int l)
{
    int c;
    for (c=0;c<T;c=c+3)
    {
        if (S[l][c].n==0)
            printf("#   ",S[l][c].n);
        else
            printf("# %i ",S[l][c].n);
        if (S[l][c+1].n==0)
            printf("|   |",S[l][c+1].n);
        else
            printf("| %i |",S[l][c+1].n);
        if (S[l][c+2].n==0)
            printf("   ",S[l][c+2].n);
        else
            printf(" %i ",S[l][c+2].n);
    }
    printf("#\n");
}

print()
{
    printf("\n\t#####################################\n\t");
    numbers(0);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(1);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(2);
    printf("\t#####################################\n\t");
    numbers(3);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(4);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(5);
    printf("\t#####################################\n\t");
    numbers(6);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(7);
    printf("\t#-----------#-----------#-----------#\n\t");
    numbers(8);
    printf("\t#####################################\n\n");
}
/*==== #prt ==================================================================*/




                    /*///////////////////////////////////*/
/*==============================================================================
 Group of functions that traverse the subgrids, places a value in a cell
 if it is the only possibility for that subgrid, and eliminates
 possibilities from the row or column if they are in the same row
 or column.
 =============================================================================*/
final(int row,int num,int col,int k,int j)
{
    if (contradiction) return 0;
    if (row!=-1 && col !=-1 && S[row][col].next!=NULL && S[row][col].n==0)
    {
        /*printf("[%i][%i]=%i\n",row+1,col+1,num);*/
        S[row][col].n=num;
        count++;
        S[row][col].next=NULL;
        check(row,col,num);

    }
    else
    {
        if (row!=-1 && col==-1)
        {
            /*printf("l=%i;val=%i\n",row+1,num);*/
            elemRow(row,num,j);
        }
        if (col!=-1 && row==-1)
        {
            /*printf("c=%i;val=%i\n",col+1,num);*/
            elemCol(col,num,k);
        }
    }
}

scanSubgrid2(int l,int c,int k,int j,nodePtr ap,int num)
{
    nodePtr ap2,tmp2;
    int n,m,i,row,col;
    row=k;
    col=j;
    for (n=l;n<l+3;n++)
        for (m=c;m<c+3;m++)
        {
            ap2=S[n][m].next;
            tmp2=S[n][m].next;
            while (ap!=ap2 && tmp2!=NULL && (*tmp2).n<=num)
            {
                if((*tmp2).n==num)
                {
                    if (row!=n)
                        row=-1;
                    if (col!=m)
                        col=-1;
                }
                tmp2=(*tmp2).next;
            }
        }
    final(row,num,col,k,j);
        /*checkL(row);*/
}


scanSubgrid(int l,int c)
{
    nodePtr tmp,ap;
    int k,j,num;
    int nums[T], nn, idx;
    for (k=l;k<l+3;k++)
        for (j=c;j<c+3;j++)
        {
            nn=0;
            tmp=S[k][j].next;
            while (tmp!=NULL) { nums[nn++]=(*tmp).n; tmp=(*tmp).next; }
            ap=S[k][j].next;
            for (idx=0;idx<nn;idx++)
            {
                if (contradiction) return 0;
                if (S[k][j].next==NULL) break;
                scanSubgrid2(l,c,k,j,ap,nums[idx]);
            }
        }
}


solve()
{
    int l,c;
    /*l=0;
    c=0;*/
        for (l=0;l<T;l=l+3)
            for (c=0;c<T;c=c+3)
            {
                scanSubgrid(l,c);
                /*print();*/
                /*getchar();*/
            }
}
/*==== #sol ==================================================================*/

                    /*///////////////////////////////////*/
/*==============================================================================
 Function that checks if the current puzzle state is valid (no duplicates)
 =============================================================================*/
int isValid()
{
    int l,c,k,i,val,bl,bc;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
        {
            val=S[l][c].n;
            if (val==0) continue;
            for (k=c+1;k<T;k++)
                if (S[l][k].n==val) return 0;
            for (k=l+1;k<T;k++)
                if (S[k][c].n==val) return 0;
            bl=l-l%3; bc=c-c%3;
            for (k=bl;k<bl+3;k++)
                for (i=bc;i<bc+3;i++)
                    if ((k!=l || i!=c) && S[k][i].n==val) return 0;
        }
    return 1;
}
/*==== #val ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 BACKTRACK function - tries to solve the puzzle by trial and error using the
 MRV (Minimum Remaining Values) heuristic
 =============================================================================*/
int backtrack()
{
    int save[T][T];
    int save_count, i, l, c, best_l, best_c, min_cands, ncands, num_cands;
    int cands[T];
    nodePtr tmp;

    if (count>=81) return 1;

    /* Save state */
    save_count=count;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
            save[l][c]=S[l][c].n;

    /* Find cell with fewest candidates (MRV) */
    min_cands=T+1;
    best_l=-1;
    best_c=-1;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
            if (S[l][c].n==0)
            {
                ncands=0;
                tmp=S[l][c].next;
                while (tmp!=NULL) { ncands++; tmp=(*tmp).next; }
                if (ncands==0) return 0;
                if (ncands<min_cands) { min_cands=ncands; best_l=l; best_c=c; }
            }

    if (best_l==-1) return 0;

    /* Save candidates in an array */
    num_cands=0;
    tmp=S[best_l][best_c].next;
    while (tmp!=NULL) { cands[num_cands++]=(*tmp).n; tmp=(*tmp).next; }

    /* Try each candidate */
    for (i=0;i<num_cands;i++)
    {
        /* Restore saved state */
        freeAll();
        init();
        count=0;
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
            {
                S[l][c].n=save[l][c];
                if (save[l][c]!=0) count++;
            }

        /* Place the tentative value */
        S[best_l][best_c].n=cands[i];
        count++;
        contradiction=0;

        /* Re-propagate constraints: remove candidates for already filled cells */
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
                if (save[l][c]!=0 || (l==best_l && c==best_c))
                {
                    S[l][c].next=NULL;
                }
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
                if ((save[l][c]!=0 || (l==best_l && c==best_c)) && !contradiction)
                {
                    check(l,c,S[l][c].n);
                }

        if (contradiction) continue;

        /* Run solve iterations */
        { int j=0; while (count<81 && j!=20 && !contradiction) { solve(); j++; } }

        if (contradiction || !isValid()) continue;
        if (count>=81) return 1;
        if (backtrack()) return 1;
    }

    /* All candidates failed - restore state for the caller */
    freeAll();
    init();
    count=0;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
        {
            S[l][c].n=save[l][c];
            if (save[l][c]!=0) count++;
        }
    contradiction=0;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
            if (S[l][c].n!=0)
            {
                S[l][c].next=NULL;
                check(l,c,S[l][c].n);
            }

    return 0;
}
/*==== #bkt ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 START function
 =============================================================================*/
start()
{
    int l,c,i=0;
    char ch='p';
    contradiction=0;
    for (l=0;l<9;l++)
        for (c=0;c<9;c++)
            if (S[l][c].n!=0)
            {
                S[l][c].next=NULL;
                check(l,c,S[l][c].n);
            }
    while (count<81 && i!=20 && !contradiction)
    {
        solve();
        i++;
        /*ch=getchar();*/
    }
    if (count<81 && !contradiction)
    {
        if (backtrack())
            printf("\7");
        else
            printf("Unable to solve sudoku!!!\n\n");
    }
    else if (count>=81)
        printf("\7");
    else
        printf("Unable to solve sudoku!!!\n\n");
    print();
    printf("count=%i\n",count);
}
/*==== #str ==================================================================*/



                    /*///////////////////////////////////*/
/*==============================================================================
 INSERT function
 =============================================================================*/
void insert()
{
    int l,c,v=1;
    freeAll();
    init();
    count=0;
    printf("Enter the Row, Column and respective Value.\n");
    printf("To finish insertion enter ZERO for the Row,\nthe Column and the Value.\n");
    printf("To clear a cell enter ZERO for the Value.\n");
    while (l && c)
    {
        printf("\nRow   : "); scanf(" %d",&l);
        printf("Column: "); scanf(" %d",&c);
        printf("Value : "); scanf(" %d",&v);
        if ((l>=0 && l<10) && (c>=0 && c<10))
        {
            if (l && c && v)
            {
                if (S[l-1][c-1].n==0)
                    count++;
                S[l-1][c-1].n=v;
            }
            else
                if (l && c)
                {
                    S[l-1][c-1].n=v;
                    if (count!=0)
                        count--;
                }
        }
        else
            printf("Values are out of range!!!\n");
        print();
        printf("count=%i\n",count);
        /*printf("\nl=%d ; c=%d ; v=%d\n",l,c,v);*/
    }



    printf("\n");
    /*print(); */
    /*tab();*/
    getchar();
    getchar();

}
/*==== #ins ==================================================================*/




                    /*///////////////////////////////////*/
/*==============================================================================
 Function that imports a puzzle from the file "sdk.txt"
 =============================================================================*/
void import()
{
    FILE *f;
    int l,c,v;
    char line[12];
    char filename[256];
    freeAll();
    init();
    count=0;
    printf("File (ENTER for sdk.txt): ");
    while ((v=getchar())!='\n' && v!=EOF); /* clear buffer */
    fgets(filename,sizeof(filename),stdin);
    /* remove newline */
    for (l=0;filename[l]!='\0';l++)
        if (filename[l]=='\n') { filename[l]='\0'; break; }
    if (filename[0]=='\0')
        sprintf(filename,"sdk.txt");
    f=fopen(filename,"r");
    if (f==NULL)
    {
        printf("Error opening file %s!\n",filename);
        return;
    }
    for (l=0;l<T;l++)
    {
        if (fgets(line,sizeof(line),f)==NULL)
            break;
        for (c=0;c<T;c++)
        {
            v=line[c]-'0';
            if (v>=1 && v<=9)
            {
                S[l][c].n=v;
                count++;
            }
        }
    }
    fclose(f);
    print();
    printf("count=%i\n",count);
}
/*==== #imp ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 Menu function
 =============================================================================*/
void menu()
{
       char option;
       do
       {
           printf("\n");
           printf("\t\tMENU\n");
           printf("\n");
           printf("\t##########################\n");
           printf("\t#                        #\n");
           printf("\t# 1 - New Puzzle         #\n");
           printf("\t# 2 - Solve Puzzle       #\n");
           printf("\t# 3 - Import Puzzle      #\n");
           printf("\t# 0 - Exit               #\n");
           printf("\t#                        #\n");
           printf("\t##########################\n");
           printf("\n");
           printf("\t\t\tOption:");
           scanf(" %c",&option);
           fflush(stdin);/* Clear the keyboard buffer */
           switch(option)
           {
                case '1':
                          	insert();
                          	break;
                case '2':
				start();
                          	break;
                case '3':
                          	import();
                          	break;
                case '0':
                          printf("Do you really want to exit? ( y / n )\n");
                          scanf(" %c",&option);
                          fflush(stdin);
                          /* two options in case uppercase is entered */
                          if(option=='y' || option=='Y')
                          {
                                    option='0';
                                    freeAll();
                          }
                          break;

                default:
                         printf("Invalid option!\n");
                         break;

           }
       }
    while(option!='0');
}
/*==== #menu =================================================================*/


/*==============================================================================
  MAIN
  ============================================================================*/
main(int argc, char *argv[])
{
    if (argc>1)
    {
        FILE *f;
        int l,c,v;
        char line[12];
        init();
        count=0;
        f=fopen(argv[1],"r");
        if (f==NULL) { printf("Error opening %s\n",argv[1]); return 1; }
        for (l=0;l<T;l++)
        {
            if (fgets(line,sizeof(line),f)==NULL) break;
            for (c=0;c<T;c++)
            {
                v=line[c]-'0';
                if (v>=1 && v<=9) { S[l][c].n=v; count++; }
            }
        }
        fclose(f);
        printf("Loaded %d clues\n",count);
        start();
        return 0;
    }
    init();
    menu();
    return 0;
}
/*==== #main =================================================================*/
