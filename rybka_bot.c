#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "templates.h"

void clear_memory(Plansza *plansza)
{
    printf("czyszczenie pamieci i zamykanie plikow\n");
    for(int i=0; i<plansza->n_rows; i++)
    {
        free(plansza->pole[i]);
    }
    free(plansza->pole);
}
void print_game_info(Plansza plansza, GameParameters params)
{
    printf("\nfaza gry: %s\nn_pingwinow: %d\ninputFile: %s\noutputFile: %s\n", \
        params.phase ? "movement":"placement", params.penguins, params.inputboardfile, params.outputboardfile);
    printf("liczba_graczy: %d\nzebrane ryby: %d\nnasz_nr: %d\n", params.n_players, *plansza.nasza_ilosc_ryb, plansza.nasz_nr);
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
int read_file(FILE *fp_in, Plansza *plansza, GameParameters *params)
{
    int n_rows, n_cols;
    fscanf(fp_in,"%d", &n_rows);
    fscanf(fp_in,"%d", &n_cols);

    plansza->n_rows = n_rows;
    plansza->n_cols = n_cols;
    printf("alokowanie pamieci ...\t");
    plansza->pole = malloc(n_rows*sizeof(POLE *));
    for(int i=0; i<n_rows; i++)
    {
        plansza->pole[i]=malloc(n_cols*sizeof(POLE));
    }
    printf("ok\n");
    for(int i=0; i<n_rows; i++)
    {
        for(int j=0; j<n_cols; j++)
        {
            char a;
            do
            {
                a=fgetc(fp_in);
                if(a>='0' && a<='3')
                {
                    plansza->pole[i][j].ileRyb=a-'0';
                }
            }
            while((a==' ' || a=='\n'));
            do
            {
                a=fgetc(fp_in);
                if(a>='0' && a<='9')
                    plansza->pole[i][j].nrGracza=a-'0';
                else
                {
                    printf("Blad wewnetrzny pliku!\n");
                    return -2;
                }
            }
            while((a==' ' || a=='\n'));
        }
    }
    char player_name[20];
    int n_ryb, nr_gracza;
    players_stats_type player;
    int myidx = -1;
    int i=0;
    printf("pobieranie statystyk graczy ...\t");
    while(fscanf(fp_in, "%s %d %d", player_name, &nr_gracza, &n_ryb) != EOF)
    {        
        player.n_ryb = n_ryb;
        player.nr_gracza = nr_gracza;
        plansza->players_stats[i] = player;
        strcpy(plansza->players_stats[i].name, player_name);
        if(strcmp(player_name, TEAM_NAME) == 0)
            myidx = i;  // index of our team name
        i++;
    }

    if(i == 0 || myidx < 0)  //nie ma jeszcze zadnych graczy
    {
        assert(params->phase == placement);
        // pierwsze uruchomienie naszego programu
        plansza->players_stats[i].nr_gracza = i+1;
        plansza->players_stats[i].n_ryb = 0;
        strcpy(plansza->players_stats[i].name, TEAM_NAME);
        if(myidx < 0)
            myidx = i;
        else myidx = 0;
        i++;
    }
    params->n_players = i;
    plansza->nasz_nr = myidx+1;
    plansza->nasza_ilosc_ryb = &(plansza->players_stats[myidx].n_ryb);
    printf("ok\n");
    return 0;
}
int save_to_file(FILE *fp_in, FILE *fp_out, Plansza *plansza, GameParameters params)
{
    printf("zapisywanie do pliku ...\t");
    fprintf(fp_out,"%d %d\n",plansza->n_rows, plansza->n_cols);
    for(int i=0; i<plansza->n_rows; i++)
    {
        for(int j=0; j<plansza->n_cols; j++)
        {
            fprintf(fp_out,"%d%d ",plansza->pole[i][j].ileRyb, plansza->pole[i][j].nrGracza);
        }
        fprintf(fp_out,"\n");
    }
    printf("ok\nzapisywanie statystyk ...\t");
    //zapisz statystyki graczy
    for(int i=0; i<params.n_players; i++)
    {
        char *name = plansza->players_stats[i].name;
        int n_ryb = plansza->players_stats[i].n_ryb;
        int nr_gracza = plansza->players_stats[i].nr_gracza;
        fprintf(fp_out, "%s %d %d\n", name, nr_gracza, n_ryb);
    }
    printf("ok\n");
    return 0;
}
int split(char *arg, char *value)
{
    if(!arg)
        return -1;

    char ch;
    int len = 0;
    int loc = 0;
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
int get_params(int argc, char **argv, GameParameters *params)
{
    phase_type phase;
    int penguins = 0;
    char inputboardfile[40];
    char outputboardfile[40];
    if(argc != 5)
    {
        printf("brak danych wejsciowychn");
    }

    for(int i=1; i<argc; i++)
    {
        char *arg = argv[i];
        char value[40];
        if(split(arg, value) != 0)
        {
            printf("error in split function\n");
            return 3;
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

void print_pole(Wspolrzedne_pola pole, char title[20])
{
    printf("%s: (%d, %d)\t", title, pole.row, pole.column);
}
/**
 * ustawia pingwina na zadanym polu
 * zwraca 
 *  0->ok
 *  2->nie mam wiecej pingwinow
 *  1->na tym polu nie mozna ustawic pingwina
*/
int ustaw_pingwina(Plansza *plansza, GameParameters params, int x, int y)
{
    assert(params.phase == placement);
    if(params.penguins <= *plansza->nasza_ilosc_ryb)
    {
        printf("nie mam juz zadnych pingwinow do rozlozenia\n");
        return EXIT_NO_MOVE;
    }
    if(plansza->pole[x][y].nrGracza!=0 || plansza->pole[x][y].ileRyb!=1)
    {
            printf("Na tym polu nie mozna ustawic pingwina!\n");
            return -1;
    } else
    {
        plansza->pole[x][y].nrGracza=plansza->nasz_nr;
        *(plansza->nasza_ilosc_ryb) += plansza->pole[x][y].ileRyb;
        plansza->pole[x][y].ileRyb=0;
    }
    return EXIT_SUCCESS;
}
/**
 *zwraca
 *  0->ok
 *  1->nie mozna tam isc
 * 
*/ 

int znajdz_nasze_pingwiny(Plansza *plansza, GameParameters params)
{
    printf("znajdywanie naszych pingwinow: ...\t");
    assert(params.phase==movement && plansza != NULL);
    
    int i=0;
    plansza->nasze_pingwiny = malloc(sizeof(Wspolrzedne_pola) *params.penguins);
    for(int r=0; r<plansza->n_rows; r++)
    {
        for(int c=0; c<plansza->n_cols; c++)
        {
            if(plansza->pole[r][c].nrGracza == plansza->nasz_nr)
            {
                plansza->nasze_pingwiny[i].row = r;
                plansza->nasze_pingwiny[i++].column = c;
            }
        }
    }
    if(i != params.penguins)
        return EXIT_EXTERNAL_ERROR;
    printf("ok\n\n");
    return EXIT_SUCCESS;
}
void print_pingwiny(Plansza *plansza, GameParameters params)
{
    assert(params.phase == movement);
    printf("Nasze pingwiny: ");
    for(int i=0; i<params.penguins; i++)
    {
        print_pole(plansza->nasze_pingwiny[i], " ");
    }
    printf("\n\n");
}
int rusz_pingwinem(Plansza *plansza, GameParameters params, Wspolrzedne_pola pingwin, Wspolrzedne_pola ruch)
{
    printf("ruch pingwinem ...\t");
    assert(plansza->pole[pingwin.row][pingwin.column].ileRyb == 0); //czy pole pingwina jest poprawne
    assert(plansza->pole[pingwin.row][pingwin.column].nrGracza == plansza->nasz_nr); //czy pingwin jest nasz
    assert(plansza->pole[ruch.row][ruch.column].nrGracza == 0);  //czy ruch jest na pewno mozliwy

    int zebrane_ryby = 0;
    plansza->pole[pingwin.row][pingwin.column].nrGracza = 0;
    plansza->pole[ruch.row][ruch.column].nrGracza = plansza->nasz_nr;

    zebrane_ryby = plansza->pole[ruch.row][ruch.column].ileRyb;
    plansza->pole[ruch.row][ruch.column].ileRyb = 0;
    plansza->players_stats[plansza->nasz_nr - 1].n_ryb += zebrane_ryby;
    printf("ok zebrano %d ryb\n", zebrane_ryby);
    return EXIT_SUCCESS;
}

analizator_pingwinow analyze_point(Plansza *plansza, Wspolrzedne_pola punkt)
{
    int potencjal_pingwina = -1;
    int max_ryb = -1;
    analizator_pingwinow pingwin_rozszerzony;
    pingwin_rozszerzony.pole = punkt;
    pingwin_rozszerzony.ileRyb = plansza->pole[punkt.row][punkt.column].ileRyb;
       // printf("pingwin: (%d, %d)\n", pingwin.row, pingwin.column);
        //idziemy w góre(dekrementacja r)
        // r -> rząd w którym się znajdujemy
        for(int r=punkt.row-1; r >= 0; r--)
        {
         //   printf("\t(%d, %d)\n", r, pingwin.column);
            if(plansza->pole[r][punkt.column].nrGracza != 0)
                break;  // napotkalismy na innego pingwina
            int ile_ryb = plansza->pole[r][punkt.column].ileRyb;
            potencjal_pingwina += ile_ryb;
            if(ile_ryb > max_ryb)
                max_ryb = ile_ryb;
         //   printf("\t\tpot: %d\n", potencjal_pingwina);
        }
        //idziemy w dół (inkrementajca r)
        // r -> rząd w którym się znajdujemy
        for(int r=punkt.row+1; r < plansza->n_rows; r++)
        {
         //   printf("\t(%d, %d)\n", r, pingwin.column);
            if(plansza->pole[r][punkt.column].nrGracza != 0)
                break;  // napotkalismy na innego pingwina
            int ile_ryb = plansza->pole[r][punkt.column].ileRyb;
            potencjal_pingwina += ile_ryb;
            if(ile_ryb > max_ryb)
                max_ryb = ile_ryb;
        //    printf("\t\tpot: %d\n", potencjal_pingwina);
        }
        //idziemy w lewo
        for(int c=punkt.column-1; c >= 0; c--)
        {
         //   printf("\t(%d, %d)\n", pingwin.row, c);
            if(plansza->pole[punkt.row][c].nrGracza != 0)
                break;  // napotkalismy na innego pingwina
            int ile_ryb = plansza->pole[punkt.row][c].ileRyb;
            potencjal_pingwina += ile_ryb;
            if(ile_ryb > max_ryb)
                max_ryb = ile_ryb;
         //   printf("\t\tpot: %d\n", potencjal_pingwina);
        }
        // idziemy w prawo
        for(int c=punkt.column+1; c < plansza->n_cols; c++)
        {
         //   printf("\t(%d, %d)\n", pingwin.row, c);
            if(plansza->pole[punkt.row][c].nrGracza != 0)
            {
          //      printf("pingwin napotkany\n");
                break;  // napotkalismy na innego pingwina
            }
            int ile_ryb = plansza->pole[punkt.row][c].ileRyb;
            potencjal_pingwina += ile_ryb;
            if(ile_ryb > max_ryb)
                max_ryb = ile_ryb;
         //   printf("\t\tpot: %d\n", potencjal_pingwina);
        }
        pingwin_rozszerzony.max_ryb = max_ryb;
        pingwin_rozszerzony.potencjal = potencjal_pingwina;
        return pingwin_rozszerzony;
}
/**
 * zwraca tablice analizator o dlugosci n_points
 */ 
analizator_pingwinow* analize_array_of_points(Plansza *plansza, Wspolrzedne_pola *punkty, int n_points)
{
    analizator_pingwinow *analizator = malloc(sizeof(analizator_pingwinow) * n_points);
    for(int i=0; i<n_points; i++)
    {
        analizator[i] = analyze_point(plansza, punkty[i]);
    }
    int max_prior = -1;
    int max_ryb = -1;
    for(int i=0; i<n_points; i++)
    {
     //   printf("pot: %d max_ryb: %d ileRyb: %d",analizator[i].potencjal, analizator[i].max_ryb, analizator[i].ileRyb);
      //  print_pole(analizator[i].pole, " pole");
        if(analizator[i].max_ryb > max_ryb || analizator[i].potencjal > max_prior)
        {
            max_prior = 1;
            max_ryb = 1;
        }
    }
    if(max_prior < 0 && max_ryb < 0)
        return NULL;
    else
        return analizator;
}
Wspolrzedne_pola znajdz_najlepszego_pingwina(Plansza *plansza, GameParameters params, typ_szukania szukanie)
{
    int n_penguins = params.penguins;
    analizator_pingwinow *analizator =  analize_array_of_points(plansza, plansza->nasze_pingwiny, n_penguins);
    if(analizator == NULL)
    {
        Wspolrzedne_pola error = {-1, -1};
        return error;
    }
    int max_ryb = -1;
    int max_prior = -1;
    Wspolrzedne_pola pole_max_ryb;
    Wspolrzedne_pola pole_max_prior;
    for(int i=0; i<n_penguins; i++)
    {
        if(analizator[i].max_ryb > max_ryb)
        {
            max_ryb = analizator[i].max_ryb;
            pole_max_ryb = analizator[i].pole;
        }
        if(analizator[i].potencjal > max_prior)
        {
            max_prior = analizator[i].potencjal;
            pole_max_prior = analizator[i].pole;
        }
     //   printf("potencjal: %d, wspolrzedne: (%d, %d)\n", analizator[i].potencjal, 
      //          analizator[i].pole.row, analizator[i].pole.column);
    }
 //   printf("pol z max ryb: (%d, %d)\n", pole_max_prior.row, pole_max_ryb.column);
  //  printf("pole z max potencjal: (%d, %d) \n", pole_max_prior.row, pole_max_prior.column);
    if(szukanie == PO_LICZBIE_RYB)    
        return pole_max_ryb;
    else
        return pole_max_prior;
}
Wspolrzedne_pola znajdz_najlepszy_ruch(Plansza *plansza, GameParameters params, Wspolrzedne_pola pingwin)
{
    print_pole(pingwin, "wybrany pingwin");
    analizator_pingwinow mozliwe_ruchy[plansza->n_cols + plansza->n_rows];
    int counter = 0;
    Wspolrzedne_pola pole;
    // lecimy w góre
    for(int r=pingwin.row-1; r>=0; r--)
    {
        pole.row = r;
        pole.column = pingwin.column;
        if(plansza->pole[r][pingwin.column].nrGracza != 0)
            break;
        mozliwe_ruchy[counter++] = analyze_point(plansza, pole);
    }
    // lecimy w dół
    for(int r=pingwin.row+1; r<plansza->n_rows; r++)
    {
        pole.row = r;
        pole.column = pingwin.column;
        if(plansza->pole[r][pingwin.column].nrGracza != 0)
            break;
        mozliwe_ruchy[counter++] = analyze_point(plansza, pole);;
    }
    //lecimy w lewo
    for(int c=pingwin.column-1; c>=0; c--)
    {
        pole.row = pingwin.row;
        pole.column = c;
        if(plansza->pole[pingwin.row][c].nrGracza != 0)
            break;
        mozliwe_ruchy[counter++] = analyze_point(plansza, pole);
    } 
    //lecimy w prawo
    for(int c=pingwin.column+1; c<plansza->n_cols; c++)
    {
        pole.row = pingwin.row;
        pole.column = c;
        if(plansza->pole[pingwin.row][c].nrGracza != 0)
            break;
        mozliwe_ruchy[counter++] = analyze_point(plansza, pole);
    } 

  //  printf("\nmozliwe ruchy: %d\n", counter);
    analizator_pingwinow pola_max_ryb[counter]; //maksymalna ilosc pól to counter, bo tyle jest mozliwych ruchow
    int max_ryb = 0;
    int counter_max_ryb = 0;    // tyle jest pól z maksymalną ilością ryb
    for(int i=0; i<counter; i++)
    {
     //   print_pole(mozliwe_ruchy[i].pole, "mozliwy ruch");
        if(mozliwe_ruchy[i].ileRyb > max_ryb)
            max_ryb = mozliwe_ruchy[i].ileRyb;
    }
 //   printf("max ryb: %d\n", max_ryb);
    //znalezlismy max ryb, counter->ilosc mozliwych ruchow
    for(int i=0; i<counter; i++)
    {
        if(mozliwe_ruchy[i].ileRyb == max_ryb)
        {
            pola_max_ryb[counter_max_ryb++] = mozliwe_ruchy[i];
        //    print_pole(mozliwe_ruchy[i].pole, "pole max ryb znalezione");
        }
    } 
    assert(counter_max_ryb != 0);   //gdzies musi byc wartosc max wiec counter >0
    // po wyjsciu z petli mamy counter_max_ryb pól z maksymalną ilością ryb
    if(counter_max_ryb == 1)
        return pola_max_ryb[0].pole; // jesli wsrod mozliwych pol jest tylko 1 pole z max ryb
    
 //   printf("counter_max_ryb: %d\n", counter_max_ryb);
    // jest counter_max_ryb pól z max iloscią ryb
    int max_potencjal = 0;
    int idx_max_potencjal = -1; //index pola_max_ryb, dla którego pole ma najwiekszy potencjal
    for(int i=0; i<counter_max_ryb; i++)
    {   // i-> index pola_max_ryb
 //       print_pole(pola_max_ryb[i].pole, "pole_max_ryb");
        if(pola_max_ryb[i].potencjal > max_potencjal)
        {
            max_potencjal = pola_max_ryb[i].potencjal;
            idx_max_potencjal = i;
        }
    }
    return pola_max_ryb[idx_max_potencjal].pole;
}
void exit_program(int exit_code)
{
    printf("\n\t\t***");
    if(exit_code == EXIT_PROGRAM_ERROR)
    {
        printf("blad wewnetrzny programu");
    }
    else if(exit_code == EXIT_EXTERNAL_ERROR)
    {
        printf("blad danych wejsciowych");
    }
    else if(exit_code == EXIT_NO_MOVE)
    {
        printf("nie moge wykonac ruchu, ale wszystko jest OK");
    }
    else if(exit_code == EXIT_SUCCESS)
    {
        printf("wszystko ok");
    }
    else
    {
        printf("nieznany kod\n");
    }
    printf("***\n\n");
}

int main(int argc, char **argv)
{
    GameParameters params;
    Plansza plansza;
    int exit_code = 0;
    if(get_params(argc, argv, &params) != 0)
        return 3;
    FILE *fp_in = fopen(params.inputboardfile, "rt");
    if(!fp_in)
    {
        printf("\nerror while opening a file\n");
        return 2; 
    }
        
    switch (params.phase)
    {
        case placement:
        {
            exit_code = read_file(fp_in, &plansza, &params);
            if(exit_code != 0)
            {
                exit_program(exit_code);
                clear_memory(&plansza);
                fclose(fp_in);
                return exit_code;
            }
            assert(plansza.pole != NULL);
            print_game_info(plansza, params);
            srand(time(NULL));
            int x;
            int y;
            for(exit_code=2; exit_code != 0;)
            {
                x = rand()%plansza.n_rows;
                y = rand()%plansza.n_cols;
                exit_code = ustaw_pingwina(&plansza, params, x, y);
                if(exit_code == 1)
                    break;
                else if(exit_code==0)
                    printf("pingwin ustawiony\n");
            }
          //  
            //
        } break;

        case movement:
        {
            exit_code = read_file(fp_in, &plansza, &params);
            if(exit_code != 0)
            {
                exit_program(exit_code);
                clear_memory(&plansza);
                fclose(fp_in);
                return exit_code;
            }
        //    print_game_info(plansza, params);
            exit_code = znajdz_nasze_pingwiny(&plansza, params);
            if(exit_code != EXIT_SUCCESS)
            {   // blad liczby pingwinow // ktos nam zarąbał, lub zle podal do programu
                exit_program(exit_code);
                clear_memory(&plansza);
                fclose(fp_in);
                return exit_code;
            }
            print_pingwiny(&plansza, params);
            Wspolrzedne_pola pingwin = znajdz_najlepszego_pingwina(&plansza, params, PO_LICZBIE_RYB);
            if(pingwin.row < 0 || pingwin.column <0)
            {   //nie ma juz gdzie isc
                exit_program(EXIT_NO_MOVE);
                clear_memory(&plansza);
                fclose(fp_in);
                return EXIT_NO_MOVE;
            }
            Wspolrzedne_pola ruch = znajdz_najlepszy_ruch(&plansza, params, pingwin);
            printf("\n\n\t***PINGWIN (%d, %d) RUSZA NA: (%d, %d)\n", pingwin.row, pingwin.column, ruch.row, ruch.column);
            exit_code = rusz_pingwinem(&plansza, params, pingwin, ruch);
        } break;

    }
    FILE *fp_out = fopen(params.outputboardfile, "wt");
    save_to_file(fp_in, fp_out, &plansza, params);
    fclose(fp_in);
    fclose(fp_out);
    clear_memory(&plansza);
    printf("\n\texit_code: %d\n", exit_code);
    return exit_code;
}
