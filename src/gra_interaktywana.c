#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
int m,n;
typedef struct
{
    int ileRyb;
    int nrGracza;
} POLE;
int IloscGraczy;
char ListaGraczy[9][20];
int RybyGraczy[9]= {0,0,0,0,0,0,0,0,0};
int iloscPingwinow;

POLE **pobierzZPliku(int *m, int *n)
{
    printf("Podaj nazwe pliku, w ktorym jest plansza. ");
    char NazwaPliku[20];
    scanf("%s",&NazwaPliku);
    FILE *plik;
    if((plik=fopen(NazwaPliku,"r"))==NULL)
    {
        printf("Blad otwarcia pliku.");
        exit(2);
    }
    fscanf(plik,"%d",m);
    fscanf(plik,"%d",n);
    POLE **Plansza;
    Plansza=malloc(*m*sizeof(POLE *));
    for(int i=0; i<*m; i++)
    {
        Plansza[i]=malloc(*n*sizeof(POLE));
    }
    for(int i=0; i<*m; i++)
    {
        for(int j=0; j<*n; j++)
        {
            char a;
            do
            {
                a=fgetc(plik);
                if(a>='0' && a<='3')
                    Plansza[i][j].ileRyb=a-'0';
            }
            while((a==' ' || a=='\n'));
            do
            {
                a=fgetc(plik);
                if(a>='0' && a<='9')
                    Plansza[i][j].nrGracza=a-'0';
                else
                {
                    printf("Blad wewnetrzny pliku!\n");
                    exit(3);
                }
            }
            while((a==' ' || a=='\n'));
        }
    }
    return Plansza;
}
void zapiszdopliku(POLE **WskaznikNaPlansze, int m, int n)
{
    FILE *plik;
    if((plik=fopen("zapis.txt","w"))==NULL)
    {
        printf("Nie mozna otworzyc pliku do zapisu.\n");
        exit(2);
    }
    fprintf(plik,"%d %d\n",m,n);
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            fprintf(plik,"%d%d ",WskaznikNaPlansze[i][j].ileRyb, WskaznikNaPlansze[i][j].nrGracza);
        }
        fprintf(plik,"\n");
    }
    for(int i=0; i<IloscGraczy; i++)
        fprintf(plik,"%s %d %d",ListaGraczy[i],i+1,RybyGraczy[i]);
    fclose(plik);
}
void drukujPlansze(POLE **WskaznikNaPlansze,int m,int n)
{
    printf("\n");
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            printf("%d.%d ", WskaznikNaPlansze[i][j].ileRyb, WskaznikNaPlansze[i][j].nrGracza);
        }
        printf("\n");
    }
}
void ustawGraczy(POLE **WskaznikNaPlansze,int m,int n,int *RybyGraczy)
{
    printf("\nPodaj liczbe graczy: ");
    scanf("%d",&IloscGraczy);
    while(IloscGraczy>9||IloscGraczy<1)
    {
        printf("Moze byc tylko od 1 do 9 graczy. Podaj liczbe graczy: ");
        scanf("%d",&IloscGraczy);
    }
    for(int i=0; i<IloscGraczy; i++)
    {
        printf("Podaj nazwe gracza nr:%d ",i+1);
        scanf("%s",ListaGraczy[i]);
    }
    printf("Podaj ile pingwinow ma kazdy gracz:\n");
    scanf("%d",&iloscPingwinow);
    while(iloscPingwinow<1)
    {
        printf("Liczba pingwinow musi byc dodatnia. Podaj liczbe pingwinow:\n");
        scanf("%d",&iloscPingwinow);
    }
    for(int i=0; i<iloscPingwinow; i++)
    {
        for(int j=0; j<IloscGraczy; j++)
        {
            int x,y;
powtoreczka:
            printf("Podaj koordynaty pola (wiersz kolumna),\nna ktorym gracz %s chce umiescic swojego pingwina:\n",ListaGraczy[j]);
            scanf("%d %d",&x,&y);
            x=x-1;
            y=y-1;
            if(WskaznikNaPlansze[x][y].nrGracza!=0 || WskaznikNaPlansze[x][y].ileRyb!=1)
            {
                printf("Na tym polu nie mozna ustawic pingwina!\n");
                goto powtoreczka;
            }
            else
            {
                WskaznikNaPlansze[x][y].nrGracza=j+1;
                RybyGraczy[j]+=WskaznikNaPlansze[x][y].ileRyb;
                printf("Ryby %s %d \n",ListaGraczy[j],RybyGraczy[j]);
                WskaznikNaPlansze[x][y].ileRyb=0;
                zapiszdopliku(WskaznikNaPlansze,m,n);
            }
        }
    }
}

void RuszPingwina(int x,int y,POLE **WskaznikNaPlansze,int obecnyGracz)
{
    printf("Obecny gracz %d \n",obecnyGracz);
    char lewo[]="lewo";
    char prawo[]= "prawo";
    char dol[]="dol";
    char gora[]="gora";
    char *koniec="koniec";
    char ruch[4];
    int liczbaKrokow;
repeat:
    printf("W ktora strone chcesz ruszyc pingwinem? (lewo prawo gora dol)\nlub koniec, jesli sie poddajesz.\n");
    scanf("%s",ruch);
    if((strcmp(ruch,koniec)==0))
    {
        return;
    }
    if(0!=(strcmp(ruch,lewo) && strcmp(ruch,prawo) && strcmp(ruch,gora) && strcmp(ruch,dol)))
    {
        printf("Nie ma takiego ruchu!");
        goto repeat;
    }
krokagain:
    printf("Ile pol chcesz sie przemiescic?\n");
    scanf("%d",&liczbaKrokow);
    if(liczbaKrokow<=0)
    {
        printf("Musisz podac przynajmniej jeden krok!\n");
        goto krokagain;
    }
    if((strcmp(ruch,lewo)==0))
    {
        for(int i=1; i<=liczbaKrokow; i++)
        {
            if(y-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
            if(WskaznikNaPlansze[x][y-i].ileRyb==0 || WskaznikNaPlansze[x][y-i].nrGracza!=0 || y-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
        }
        WskaznikNaPlansze[x][y-liczbaKrokow].nrGracza=obecnyGracz;
        WskaznikNaPlansze[x][y].nrGracza=0;
        RybyGraczy[obecnyGracz-1]+= WskaznikNaPlansze[x][y-liczbaKrokow].ileRyb;
        WskaznikNaPlansze[x][y-liczbaKrokow].ileRyb=0;
        drukujPlansze(WskaznikNaPlansze,m,n);
    }
    if((strcmp(ruch,prawo)==0))
    {
        for(int i=1; i<=liczbaKrokow; i++)
        {
            if(y+i>=m-1)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
            if(WskaznikNaPlansze[x][y+i].ileRyb==0 || WskaznikNaPlansze[x][y+i].nrGracza!=0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
        }
        WskaznikNaPlansze[x][y+liczbaKrokow].nrGracza=obecnyGracz;
        WskaznikNaPlansze[x][y].nrGracza=0;
        RybyGraczy[obecnyGracz-1]+= WskaznikNaPlansze[x][y+liczbaKrokow].ileRyb;
        WskaznikNaPlansze[x][y+liczbaKrokow].ileRyb=0;
        drukujPlansze(WskaznikNaPlansze,m,n);
    }
    if((strcmp(ruch,gora)==0))
    {
        for(int i=1; i<=liczbaKrokow; i++)
        {
            if(x-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
            if(WskaznikNaPlansze[x-i][y].ileRyb==0 || WskaznikNaPlansze[x-i][y].nrGracza!=0 || x-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
        }
        WskaznikNaPlansze[x-liczbaKrokow][y].nrGracza=obecnyGracz;
        WskaznikNaPlansze[x][y].nrGracza=0;
        RybyGraczy[obecnyGracz-1]+= WskaznikNaPlansze[x-liczbaKrokow][y].ileRyb;
        WskaznikNaPlansze[x-liczbaKrokow][y].ileRyb=0;
        drukujPlansze(WskaznikNaPlansze,m,n);
    }
    if((strcmp(ruch,dol)==0))
    {
        for(int i=1; i<=liczbaKrokow; i++)
        {
            if(x+i>n)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
            if(WskaznikNaPlansze[x+i][y].ileRyb==0 || WskaznikNaPlansze[x+i][y].nrGracza!=0 || x+i>n)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                goto repeat;
            }
        }
        WskaznikNaPlansze[x+liczbaKrokow][y].nrGracza=obecnyGracz;
        WskaznikNaPlansze[x][y].nrGracza=0;
        RybyGraczy[obecnyGracz-1]+= WskaznikNaPlansze[x+liczbaKrokow][y].ileRyb;
        WskaznikNaPlansze[x+liczbaKrokow][y].ileRyb=0;
        drukujPlansze(WskaznikNaPlansze,m,n);
    }
    printf("Po tej rundzie gracz numer %d %s ma %d ryb.\n",(obecnyGracz),ListaGraczy[obecnyGracz-1],RybyGraczy[obecnyGracz-1]);
    zapiszdopliku(WskaznikNaPlansze,m,n);
}
void rozgrywka(POLE **WskaznikNaPlansze)
{
    for(int k=0;; k++)
    {
        int x,y;
again:
        printf("%s wybierz koordynaty (wiersz kolumna) pingwina, ktorym chcesz ruszyc.\n",ListaGraczy[k%IloscGraczy]);
        printf("Jezeli nie masz wolnego pingwina wpisz -1 -1,\na jesli nikt nie wolnego pingwina -2 -2.\n");
        scanf("%d %d",&x,&y);
        x=x-1;
        y=y-1;
        if(x==-2 && y==-2)
            goto koniecTury;
        if(x==-3 && y==-3)
            break;
        if(WskaznikNaPlansze[x][y].nrGracza!=(k%IloscGraczy+1))
        {
            printf("Nie ma tu twojego pingwina! \n");
            goto again;
        }
        else
            RuszPingwina(x,y,WskaznikNaPlansze,k%IloscGraczy+1);
koniecTury:
        printf("\n");
    }
    for(int i=0; i<IloscGraczy; i++)
    {
        printf("Pingwin %s zdobyl %d ryb.\n",ListaGraczy[i],RybyGraczy[i]);
    }
}
void pobierzgraczyiryby(POLE **WskaznikNaPlansze)
{
    printf("Podaj nazwe pliku z poprzednim stanem gry.\nJesli zaczynasz nowe gre wpisz \"nowa\". ");
    char NazwaPliku[20];
    char nowa[]="nowa";
    scanf("%s",&NazwaPliku);
    if(strcmp(NazwaPliku,nowa)!=0)
    {
        char pomoc;
        FILE *plik;
        int index;
        if((plik=fopen(NazwaPliku,"r"))==NULL)
        {
            printf("Blad otwarcia pliku z zapisanym stanem gry!");
            exit(2);
        }
        pomoc=fgetc(plik);
        while(isalpha(pomoc)!=1)
            pomoc=fgetc(plik);
        for(int i=0; i<IloscGraczy+1; i++)
        {
            fscanf(plik, "%s", ListaGraczy[i]);
            pomoc=fgetc(plik);
            fscanf(plik, "%d", &IloscGraczy);
            pomoc=fgetc(plik);
            fscanf(plik, "%d", &RybyGraczy);
            }
        fclose(plik);
    }
    else
    {
        drukujPlansze(WskaznikNaPlansze,m,n);
        ustawGraczy(WskaznikNaPlansze,m,n,RybyGraczy);
    }
}
int main()
{
    POLE **WskaznikNaPlansze=pobierzZPliku(&m,&n);
    pobierzgraczyiryby(WskaznikNaPlansze);
    drukujPlansze(WskaznikNaPlansze,m,n);

    rozgrywka(WskaznikNaPlansze);
    return 0;
}
