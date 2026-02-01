#include<stdio.h>
#include<stdlib.h>
#define T 9  /* Valor que define o Tamanho do puzzle */
int count=0; /* Variavel que conta o numero de valores introduzidos na matriz "S" */
int contradiction=0; /* Flag que indica contradição durante propagação */

            /*#############*/
/*========================================
 ESTRUTURA de dados que cada nó das listas
 terá. Esta estrutura tem uma variável que
 receberá um numero entre 1 e "T" e contem
 também um apontador para o nó seguinte
 ========================================*/
typedef struct dados
{
    int n;
    struct dados *NoS;
}data;
/*=======================================*/

typedef data *apL; /* Declaração do tipo de dados "apL" que é um apontador */
                   /* para uma estrutura do tipo é anteriormente declarada */

data S[T][T];  /* declaracao de uma matriz "T" por "T"                 */


                    /*///////////////////////////////////*/
/*==============================================================================
 Função que INICIALIZA uma lista por cada indice da matriz, essa lista
 ficará com "T" nós em que cada nó terá um numero, entre 1 e "T".
 =============================================================================*/
void inic()
{
    apL ap,tmp;
    int l,c,i;
    for (l=0;l<T;l++)          /* percorre cada linha até ao final            */
        for (c=0;c<T;c++)      /* todos os índices da matriz são percorridos  */
        {
            S[l][c].n=0;       /* o valor 0 corresponde a um espaço em branco */
            S[l][c].NoS=(data*)malloc(sizeof(data));/* Criação do primeiro nó */
            tmp=S[l][c].NoS;
            for (i=1;i<T;i++)
            {
                ap=(data*)malloc(sizeof(data)); /* Aqui são criados os  nós   */
                (*tmp).NoS=ap;                  /* restantes, em que cada nó  */
                (*tmp).n=i;                     /* recebera um numero         */
                tmp=ap;
            }
            (*tmp).n=T;
            (*tmp).NoS=NULL;     /* O ultimo nó ficará a apontar para "NULL"  */
        }
}
/*==== #ini ==================================================================*/






                    /*///////////////////////////////////*/
/*==============================================================================
 Função que permite libertar todos os nós associados a cada uma das celulas de S
 =============================================================================*/
void liberta()
{
    apL tmp2,tmp;
    int l,c,i;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
        {
            tmp=S[l][c].NoS;
            tmp2=tmp;
            while(tmp!=NULL)
            {
                tmp=(*tmp2).NoS;
                free(tmp2);
                tmp2=tmp;
            }
            if (tmp2!=NULL)
                free (tmp2);
            S[l][c].NoS=NULL;
        }
}
/*==== #lib ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 Função que remove o valor "val" da lista que está associada à celula [l][c]
 da matriz S.
 =============================================================================*/
int rem(int l,int c,int val)
{
    apL tmp,tmp2;
    if (contradiction) return 0;
    tmp=&S[l][c];
    tmp2=(*tmp).NoS;
    /*printf("[%i][%i] - %i\n",l+1,c+1,val);*/
    if (tmp2==NULL)
        return 0;
    while ((*tmp2).NoS!=NULL && (*tmp2).n!=val)
    {
        tmp=(*tmp).NoS;
        tmp2=(*tmp2).NoS;
    }
    if (tmp2==NULL)
    {
        (*tmp).NoS=(*tmp2).NoS;
        free(tmp2);
    }
    else
        if ((*tmp2).n==val)
        {
            (*tmp).NoS=(*tmp2).NoS;
            free(tmp2);
        }
    tmp=S[l][c].NoS;
    if (tmp==NULL) { contradiction=1; return 0; }
    tmp2=(*tmp).NoS;
    if(tmp2==NULL)
    {
        S[l][c].n=(*tmp).n;
        count++;             /* o contador é incrementado */
        S[l][c].NoS=NULL;
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
 Função que percorre linha "l", coluna "c" e a submatriz associada a [l][c]
 =============================================================================*/
void check (int l,int c,int val)
{
    int k,x,i;
    if (contradiction) return;
    for (k=0;k<T;k++)/* percorre as colunas */
    {
        x=rem(l,k,val);    /* Ao ser removido "val" e este for o unico elemento */
        if (x)             /* da lista, esse valor será introduzido na celula   */
        {                   /* correspondente da matriz,  esta função é chamada  */
                           /* novamente, sendo o valor a remover o que a função */
            check(l,k,x);  /* retornar             (A)                          */
        }
    }

    if (contradiction) return;
    for (k=0;k<T;k++)/* percorre as linhas  */
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
    for (k=l;k<l+3;k++)      /* Nestes dois "for" são      */
        for (i=c;i<c+3;i++)  /* percorridas as sub-grelhas */
        {
            x=rem(k,i,val);
            if (x)          /*       (A)     */
                check(k,i,x);
        }
}
/*==== #chk ==================================================================*/



                    /*///////////////////////////////////*/
/*==============================================================================
 Função que percorre os elementos da linha "l", excepto os elementos associados
 à submatriz que tem o elemento de índice [l][j].
 =============================================================================*/
elemL(int l,int val,int j)
{
    int x,i,tmp;
    tmp=j;
    while (j!=6 && j!=3 && j!=0)
        j--;
    for (i=0;i<T;i++)/* percorre as colunas */
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
 Função que percorre os elementos da coluna "c", excepto os elementos associados
 à submatriz que tem o elemento de índice [k][c].
 =============================================================================*/
elemC(int c,int val,int k)
{
    int i,x,tmp;
    tmp=k;
    while (k!=6 && k!=3 && k!=0)
        k--;
    for (i=0;i<T;i++)/* percorre as linhas  */
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
 Duas Funções que imprimem os valores da matriz
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
 Grupo de funções que percorre as sub-grelhas, introduz um valor no elemento
 se este for a única possibilidade para essa sub-grelha e elemina as
 possibilidades da linha ou coluna se as possibilidades forem da mesma linha
 ou coluna.
 =============================================================================*/
final(int linha,int num,int coluna,int k,int j)
{
    if (contradiction) return 0;
    if (linha!=-1 && coluna !=-1 && S[linha][coluna].NoS!=NULL && S[linha][coluna].n==0)
    {
        /*printf("[%i][%i]=%i\n",linha+1,coluna+1,num);*/
        S[linha][coluna].n=num;
        count++;
        S[linha][coluna].NoS=NULL;
        check(linha,coluna,num);

    }
    else
    {
        if (linha!=-1 && coluna==-1)
        {
            /*printf("l=%i;val=%i\n",linha+1,num);*/
            elemL(linha,num,j);
        }
        if (coluna!=-1 && linha==-1)
        {
            /*printf("c=%i;val=%i\n",coluna+1,num);*/
            elemC(coluna,num,k);
        }
    }
}

perc_subG2(int l,int c,int k,int j,apL ap,int num)
{
    apL ap2,tmp2;
    int n,m,i,linha,coluna;
    linha=k;
    coluna=j;
    for (n=l;n<l+3;n++)
        for (m=c;m<c+3;m++)
        {
            ap2=S[n][m].NoS;
            tmp2=S[n][m].NoS;
            while (ap!=ap2 && tmp2!=NULL && (*tmp2).n<=num)
            {
                if((*tmp2).n==num)
                {
                    if (linha!=n)
                        linha=-1;
                    if (coluna!=m)
                        coluna=-1;
                }
                tmp2=(*tmp2).NoS;
            }
        }
    final(linha,num,coluna,k,j);
        /*checkL(linha);*/
}


perc_subG(int l,int c)
{
    apL tmp,ap;
    int k,j,num;
    int nums[T], nn, idx;
    for (k=l;k<l+3;k++)
        for (j=c;j<c+3;j++)
        {
            nn=0;
            tmp=S[k][j].NoS;
            while (tmp!=NULL) { nums[nn++]=(*tmp).n; tmp=(*tmp).NoS; }
            ap=S[k][j].NoS;
            for (idx=0;idx<nn;idx++)
            {
                if (contradiction) return 0;
                if (S[k][j].NoS==NULL) break;
                perc_subG2(l,c,k,j,ap,nums[idx]);
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
                perc_subG(l,c);
                /*print();*/
                /*getchar();*/
            }
}
/*==== #sol ==================================================================*/

                    /*///////////////////////////////////*/
/*==============================================================================
 Função que verifica se o estado actual do puzzle e' valido (sem duplicados)
 =============================================================================*/
int valido()
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
 Função BACKTRACK - tenta resolver o puzzle por tentativa e erro usando a
 heuristica MRV (Minimum Remaining Values)
 =============================================================================*/
int backtrack()
{
    int save[T][T];
    int save_count, i, l, c, best_l, best_c, min_cands, ncands, num_cands;
    int cands[T];
    apL tmp;

    if (count>=81) return 1;

    /* Guardar estado */
    save_count=count;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
            save[l][c]=S[l][c].n;

    /* Encontrar celula com menos candidatos (MRV) */
    min_cands=T+1;
    best_l=-1;
    best_c=-1;
    for (l=0;l<T;l++)
        for (c=0;c<T;c++)
            if (S[l][c].n==0)
            {
                ncands=0;
                tmp=S[l][c].NoS;
                while (tmp!=NULL) { ncands++; tmp=(*tmp).NoS; }
                if (ncands==0) return 0;
                if (ncands<min_cands) { min_cands=ncands; best_l=l; best_c=c; }
            }

    if (best_l==-1) return 0;

    /* Guardar candidatos num array */
    num_cands=0;
    tmp=S[best_l][best_c].NoS;
    while (tmp!=NULL) { cands[num_cands++]=(*tmp).n; tmp=(*tmp).NoS; }

    /* Tentar cada candidato */
    for (i=0;i<num_cands;i++)
    {
        /* Restaurar estado guardado */
        liberta();
        inic();
        count=0;
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
            {
                S[l][c].n=save[l][c];
                if (save[l][c]!=0) count++;
            }

        /* Colocar o valor tentativo */
        S[best_l][best_c].n=cands[i];
        count++;
        contradiction=0;

        /* Re-propagar restricoes: remover candidatos para celulas ja preenchidas */
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
                if (save[l][c]!=0 || (l==best_l && c==best_c))
                {
                    S[l][c].NoS=NULL;
                }
        for (l=0;l<T;l++)
            for (c=0;c<T;c++)
                if ((save[l][c]!=0 || (l==best_l && c==best_c)) && !contradiction)
                {
                    check(l,c,S[l][c].n);
                }

        if (contradiction) continue;

        /* Executar iteracoes de solve */
        { int j=0; while (count<81 && j!=20 && !contradiction) { solve(); j++; } }

        if (contradiction || !valido()) continue;
        if (count>=81) return 1;
        if (backtrack()) return 1;
    }

    /* Todos os candidatos falharam - restaurar estado para o chamador */
    liberta();
    inic();
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
                S[l][c].NoS=NULL;
                check(l,c,S[l][c].n);
            }

    return 0;
}
/*==== #bkt ==================================================================*/


                    /*///////////////////////////////////*/
/*==============================================================================
 Função START
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
                S[l][c].NoS=NULL;
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
            printf("Impossivel resolver sudoku!!!\n\n");
    }
    else if (count>=81)
        printf("\7");
    else
        printf("Impossivel resolver sudoku!!!\n\n");
    print();
    printf("count=%i\n",count);
}
/*==== #str ==================================================================*/



                    /*///////////////////////////////////*/
/*==============================================================================
 Função INSERT
 =============================================================================*/
void insert()
{
    int l,c,v=1;
    liberta();
    inic();
    count=0;
    printf("introduza a Linha Coluna e repectivo Valor.\n");
    printf("Para terminar a insercao insira ZERO para a Linha,\npara a Coluna e para o Valor.\n");
    printf("Para preencher uma celula a branco insira ZERO para o Valor.\n");
    while (l && c)
    {
        printf("\nLinha : "); scanf(" %d",&l);
        printf("Coluna: "); scanf(" %d",&c);
        printf("Valor : "); scanf(" %d",&v);
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
            printf("Os valores estao fora do intervalo!!!:\n");
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
 Função que importa um puzzle a partir do ficheiro "sdk.txt"
 =============================================================================*/
void import()
{
    FILE *f;
    int l,c,v;
    char linha[12];
    char ficheiro[256];
    liberta();
    inic();
    count=0;
    printf("Ficheiro (ENTER para sdk.txt): ");
    while ((v=getchar())!='\n' && v!=EOF); /* limpar buffer */
    fgets(ficheiro,sizeof(ficheiro),stdin);
    /* remover newline */
    for (l=0;ficheiro[l]!='\0';l++)
        if (ficheiro[l]=='\n') { ficheiro[l]='\0'; break; }
    if (ficheiro[0]=='\0')
        sprintf(ficheiro,"sdk.txt");
    f=fopen(ficheiro,"r");
    if (f==NULL)
    {
        printf("Erro ao abrir o ficheiro %s!\n",ficheiro);
        return;
    }
    for (l=0;l<T;l++)
    {
        if (fgets(linha,sizeof(linha),f)==NULL)
            break;
        for (c=0;c<T;c++)
        {
            v=linha[c]-'0';
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
 Função Menu
 =============================================================================*/
void menu()
{
       char opcao;
       do
       {
           printf("\n");
           printf("\t\tMENU\n");
           printf("\n");
           printf("\t##########################\n");
           printf("\t#                        #\n");
           printf("\t# 1 - Novo Puzzle        #\n");
           printf("\t# 2 - Resolver Puzzle    #\n");
           printf("\t# 3 - Importar Puzzle    #\n");
           printf("\t# 0 - Sair               #\n");
           printf("\t#                        #\n");
           printf("\t##########################\n");
           printf("\n");
           printf("\t\t\tOpcao:");
           scanf(" %c",&opcao);
           fflush(stdin);/* Limpar o buffer do teclado*/
           switch(opcao)
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
                          printf("Quer mesmo sair? ( s / n )\n");
                          scanf(" %c",&opcao);
                          fflush(stdin);
                          /*duas opcoes para caso de ser introduzido maiuscula*/
                          if(opcao=='s' || opcao=='S')
                          {
                                    opcao='0';
                                    liberta();
                          }
                          break;
                
                default:
                         printf("Opcao Incorrecta!\n");
                         break;                   
    
           }
       }  
    while(opcao!='0');
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
        char linha[12];
        inic();
        count=0;
        f=fopen(argv[1],"r");
        if (f==NULL) { printf("Erro ao abrir %s\n",argv[1]); return 1; }
        for (l=0;l<T;l++)
        {
            if (fgets(linha,sizeof(linha),f)==NULL) break;
            for (c=0;c<T;c++)
            {
                v=linha[c]-'0';
                if (v>=1 && v<=9) { S[l][c].n=v; count++; }
            }
        }
        fclose(f);
        printf("Loaded %d clues\n",count);
        start();
        return 0;
    }
    inic();
    menu();
    return 0;
}
/*==== #main =================================================================*/
