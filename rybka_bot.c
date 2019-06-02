#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INTERAKTYWNA 1

typedef struct {
    int ileRyb;
    int nrGracza;
} POLE;


typedef struct {
    POLE **pole;
    int n_rows;
    int n_cols;
    int IloscGraczy;
    int ilosc_ryb;
    int ilosc_pingwinow;
    int nr_gracza;
    char *imie_gracza;
} Plansza;

int IloscGraczy;
char ListaGraczy[9][20];
int RybyGraczy[9]= {0,0,0,0,0,0,0,0,0};
int iloscPingwinow;

void print_plansze(Plansza plansza)
{
    printf("\n    ");
    for(int i=0; i<plansza.n_cols; i++)
    {
        printf("%d ", i+1);
        if(i < 9)
            printf(" ");
    }
    printf("\n");
    for(int i=0; i<plansza.n_rows; i++)
    {
        printf("%d:  ", i+1);
        for(int j=0; j<plansza.n_cols; j++)
        {
            printf("%d%d ", plansza.pole[i][j].ileRyb, plansza.pole[i][j].nrGracza);
        }
        printf("\n");
    }
}

Plansza read_file(FILE *fp)
{
    int n_rows, n_cols;
    fscanf(fp,"%d", &n_rows);
    fscanf(fp,"%d", &n_cols);

    Plansza plansza;
    printf("ok\n");
    plansza.n_rows = n_rows;
    plansza.n_cols = n_cols;
    printf("ok\n");
    plansza.pole = malloc(n_rows*sizeof(POLE *));
    for(int i=0; i<n_rows; i++)
    {
        plansza.pole[i]=malloc(n_cols*sizeof(POLE));
    }
    printf("ok\n");
    for(int i=0; i<n_rows; i++)
    {
        for(int j=0; j<n_cols; j++)
        {
            char a;
            do
            {
                a=fgetc(fp);
                if(a>='0' && a<='3')
                {
                    plansza.pole[i][j].ileRyb=a-'0';
                }
            }
            while((a==' ' || a=='\n'));
            do
            {
                a=fgetc(fp);
                if(a>='0' && a<='9')
                    plansza.pole[i][j].nrGracza=a-'0';
                else
                {
                    printf("Blad wewnetrzny pliku!\n");
                    exit(3);
                }
            }
            while((a==' ' || a=='\n'));
        }
    }
    return plansza;
}

int split(char *arg, char *value)
{
    if(!arg)
        return -1;

    char ch;
    int len = 0;
    int loc = 0;
    int max_len = strlen(arg);
    for(len=0, ch=arg[0]; ch != '\0'; ch=arg[len++])
    {
        if(ch == '\0')
            return -1;
        if(ch == '=')
            loc = len;
    }
    if(loc == 0)
        return -1;
    for(int n=0; n<len; n++)
        value[n] = arg[n+loc];
    arg[loc-1] = '\0';
    return 0;
}

typedef enum p{
    placement = 0, movement = 1
} phase_type;

typedef struct game_params {
    POLE ** WskaznikNaPlansze;
    phase_type phase;
    int penguins;
    char inputboardfile[40];
    char outputboardfile[40];
}GameParameters;

int get_params(int argc, char **argv, GameParameters *params)
{
    phase_type phase;
    int penguins = 0;
    char inputboardfile[40];
    char outputboardfile[40];

    for(int i=1; i<argc; i++)
    {
        char *arg = argv[i];
        char value[40];
        if(!split(arg, value))
        {
            strcmp(value, "ale");
        }
        if(!strcmp(arg, "phase"))
        {
            if(!strcmp(value, "placement"))
                phase = placement;
            else
                phase = movement;
        }
        else if(!strcmp(arg, "penguins"))
        {
            printf("penguins set\n");
            sscanf(value, "%d", &penguins);
        }
        else if(!strcmp(arg, "inputboardfile"))
            strcpy(inputboardfile,value);
        else if(!strcmp(arg, "outputboardfile"))
            strcpy(outputboardfile, value);
        else
        {
            printf("wrong arguments!!\n");
            return -2;
        }
    }
    strcpy(params->inputboardfile, inputboardfile);
    strcpy(params->outputboardfile, outputboardfile);
    params->penguins = penguins;
    params->phase = phase;
    return 0;
}


int main(int argc, char **argv)
{
    GameParameters params;
    Plansza plansza;
    if(get_params(argc, argv, &params) != 0)
        return -3;
    printf("phase : %d\n", params.phase);
    printf("penguins = %d\n", params.penguins);
    printf("inputboardfile = %s\n", params.inputboardfile);
    printf("outputboardfile = %s\n", params.outputboardfile);
    FILE *fp = fopen(params.inputboardfile, "r");
    if(!fp)
    {
        printf("\nerror while opening a file\n");
        return -2; 
    }
        
    switch (params.phase)
    {
        case placement:
        {
            plansza = read_file(fp);
            if(plansza.pole == NULL)
                printf("pole = NULL\n");
            printf("n_rows = %d\nn_cols = %d\n", plansza.n_rows, plansza.n_cols);
            print_plansze(plansza);

            //ustaw_pingwina(plansza, int x, int y);
            //
        } break;

        case movement:
        {
            // wybierz pingwina, jeśli nie mozesz się ruszyć return 1
            //RuszPingwina
        } break;

    }
    return 0;
}
