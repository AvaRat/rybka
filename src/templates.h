#define INTERAKTYWNA 1
#define EXIT_SUCCESS 0
#define EXIT_EXTERNAL_ERROR 2
#define EXIT_PROGRAM_ERROR 3
#define EXIT_NO_MOVE 1
#define TEAM_NAME "marcel"

typedef struct {
    int ileRyb;
    int nrGracza;
} POLE;

typedef struct {
    int row;
    int column;
} Wspolrzedne_pola;


typedef struct {
    int potencjal;
    int max_ryb;
    int ileRyb;
    Wspolrzedne_pola pole;
} analizator_pingwinow;

typedef enum  {
    PO_LICZBIE_RYB,
    PO_PRIORYTECIE
}typ_szukania;

typedef enum p{
    placement = 0, movement = 1
} phase_type;

typedef struct {
  char name[20];
  int nr_gracza;
  int n_ryb;
}players_stats_type;

typedef struct game_params {
    phase_type phase;
    int penguins;
    int n_players;
    char inputboardfile[40];
    char outputboardfile[40];
}GameParameters;

typedef struct {
    int n_rows;
    int n_cols;
    int nasz_nr;
    Wspolrzedne_pola *nasze_pingwiny;
    int *nasza_ilosc_ryb;   // wskaznik na naszego gracza w strukturze player_stats
    players_stats_type players_stats[9];
    POLE **pole;
} Plansza;

