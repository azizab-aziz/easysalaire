#include "employe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static int bulletin_counter = 0;

void creerDossierSaves(void) {
    #ifdef _WIN32
        mkdir("C:\\EasySalaire\\saves");
        mkdir("C:\\EasySalaire\\saves\\historique");
    #else
        mkdir("saves", 0777);
        mkdir("saves/historique", 0777);
    #endif
}

// ─── Calcul salaire brut ──────────────────────
float calculBrut(Employe *e) {
    return e->salaire_base
         + (e->heures_sup * 1.5f)
         + e->prime;
}

// ─── Calcul CNSS ─────────────────────────────
float calculCNSS(float brut) {
    return brut * TAUX_CNSS;
}

// ─── Calcul IR (barème tunisien) ──────────────
float calculIR(float brut) {
    float ir = 0.0f;
    float annuel = brut * 12.0f;

    if (annuel <= 5000.0f)
        ir = 0.0f;
    else if (annuel <= 10000.0f)
        ir = (annuel - 5000.0f) * 0.15f;
    else if (annuel <= 20000.0f)
        ir = 5000.0f * 0.15f
           + (annuel - 10000.0f) * 0.25f;
    else if (annuel <= 30000.0f)
        ir = 5000.0f * 0.15f
           + 10000.0f * 0.25f
           + (annuel - 20000.0f) * 0.30f;
    else
        ir = 5000.0f * 0.15f
           + 10000.0f * 0.25f
           + 10000.0f * 0.30f
           + (annuel - 30000.0f) * 0.35f;

    return ir / 12.0f;
}

// ─── Calcul salaire net ───────────────────────
float calculNet(Employe *e) {
    float brut = calculBrut(e);
    e->cnss = calculCNSS(brut);
    e->ir   = calculIR(brut);
    e->salaire_net = brut - e->cnss - e->ir;
    return e->salaire_net;
}


void getDateActuelle(char *buf) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    const char *mois[] = {
        "Janvier", "Fevrier", "Mars", "Avril",
        "Mai", "Juin", "Juillet", "Aout",
        "Septembre", "Octobre", "Novembre", "Decembre"
    };

    sprintf(buf, "%s %d",
            mois[tm_info->tm_mon],
            tm_info->tm_year + 1900);
}

// ─── Ajouter un employé ───────────────────────
void ajouterEmploye(Employe tab[], int *nb, Employe e) {
    if (*nb < MAX_EMPLOYES) {
        calculNet(&e);
        getDateActuelle(e.mois_annee);
        bulletin_counter++;
        e.numero_bulletin = bulletin_counter;
        tab[(*nb)++] = e;
        sauvegarderHistorique(&tab[(*nb)-1]);
        ajouterIndex(&tab[(*nb)-1]);
    }
}

// ─── Supprimer un employé ─────────────────────
void supprimerEmploye(Employe tab[], int *nb, int index) {
    if (index < 0 || index >= *nb) return;
    for (int i = index; i < (*nb) - 1; i++) {
        tab[i] = tab[i + 1];
    }
    (*nb)--;
}
void sauvegarderFiche(Employe *e) {
    creerDossierSaves();
    char filename[200];
    sprintf(filename, "C:\\EasySalaire\\saves\\%s_%s_fiche.txt",
            e->nom, e->prenom);

    FILE *f = fopen(filename, "w");
    if (f == NULL) return;

    float brut = calculBrut(e);

    fprintf(f, "========================================\n");
    fprintf(f, "         FICHE DE PAIE - EasySalaire   \n");
    fprintf(f, "========================================\n\n");

    fprintf(f, "INFORMATIONS PERSONNELLES\n");
    fprintf(f, "-------------------------\n");
    fprintf(f, "Bulletin N   : %03d\n", e->numero_bulletin);
    fprintf(f, "Periode      : %s\n", e->mois_annee);
    fprintf(f, "\n");
    fprintf(f, "Nom          : %s\n", e->nom);
    fprintf(f, "Prenom       : %s\n", e->prenom);
    fprintf(f, "Poste        : %s\n", e->poste);

    fprintf(f, "\nCALCUL DU SALAIRE\n");
    fprintf(f, "-------------------------\n");
    fprintf(f, "Salaire base : %.2f TND\n", e->salaire_base);
    fprintf(f, "Heures sup   : + %.2f TND\n", e->heures_sup * 1.5f);
    fprintf(f, "Prime        : + %.2f TND\n", e->prime);
    fprintf(f, "Salaire brut : %.2f TND\n", brut);

    fprintf(f, "\nRETENUES\n");
    fprintf(f, "-------------------------\n");
    fprintf(f, "CNSS (9.18%%) : - %.2f TND\n", e->cnss);
    fprintf(f, "IR            : - %.2f TND\n", e->ir);

    fprintf(f, "\n========================================\n");
    fprintf(f, "SALAIRE NET  : %.2f TND\n", e->salaire_net);
    fprintf(f, "========================================\n");

    fclose(f);
}


void sauvegarderHistorique(Employe *e) {
    // Create base historique folder
    #ifdef _WIN32
        mkdir("C:\\EasySalaire\\saves\\historique");
    #endif

    // Create employee folder
    char dossier[200];
    sprintf(dossier, "C:\\EasySalaire\\saves\\historique\\%s_%s",
            e->nom, e->prenom);
    #ifdef _WIN32
        mkdir(dossier);
    #endif

    // Create file
    char filename[300];
    sprintf(filename,
        "C:\\EasySalaire\\saves\\historique\\%s_%s\\N%03d_%s.txt",
        e->nom, e->prenom,
        e->numero_bulletin,
        e->mois_annee);

    // Replace spaces in filename
    for (int i = 0; filename[i]; i++)
        if (filename[i] == ' ') filename[i] = '_';

    FILE *f = fopen(filename, "w");
    if (f == NULL) return;

    float brut = calculBrut(e);

    fprintf(f, "========================================\n");
    fprintf(f, "      FICHE DE PAIE - EasySalaire      \n");
    fprintf(f, "========================================\n\n");
    fprintf(f, "Bulletin N   : N%03d\n", e->numero_bulletin);
    fprintf(f, "Periode      : %s\n\n", e->mois_annee);
    fprintf(f, "Nom          : %s\n", e->nom);
    fprintf(f, "Prenom       : %s\n", e->prenom);
    fprintf(f, "Poste        : %s\n", e->poste);
    fprintf(f, "\nCALCUL DU SALAIRE\n");
    fprintf(f, "-------------------------\n");
    fprintf(f, "Salaire base : %.2f TND\n", e->salaire_base);
    fprintf(f, "Heures sup   : + %.2f TND\n", e->heures_sup * 1.5f);
    fprintf(f, "Prime        : + %.2f TND\n", e->prime);
    fprintf(f, "Salaire brut : %.2f TND\n", brut);
    fprintf(f, "\nRETENUES\n");
    fprintf(f, "-------------------------\n");
    fprintf(f, "CNSS (9.18%%) : - %.2f TND\n", e->cnss);
    fprintf(f, "IR            : - %.2f TND\n", e->ir);
    fprintf(f, "\n========================================\n");
    fprintf(f, "SALAIRE NET  : %.2f TND\n", e->salaire_net);
    fprintf(f, "========================================\n");

    fclose(f);
}

void ajouterIndex(Employe *e) {
    FILE *f = fopen("C:\\EasySalaire\\saves\\historique\\index.csv", "a");
    if (!f) return;
    fprintf(f, "%s,%s,%d,%s,%.2f\n",
        e->nom, e->prenom,
        e->numero_bulletin,
        e->mois_annee,
        e->salaire_net);
    fclose(f);
}

int compterBulletins(const char *nom, const char *prenom) {
    FILE *f = fopen("C:\\EasySalaire\\saves\\historique\\index.csv", "r");
    if (!f) return 0;
    int count = 0;
    char line[300];
    while (fgets(line, sizeof(line), f)) {
        char n[50], p[50];
        sscanf(line, "%49[^,],%49[^,]", n, p);
        if (strcmp(n, nom) == 0 && strcmp(p, prenom) == 0)
            count++;
    }
    fclose(f);
    return count;
}

void premierBulletin(const char *nom, const char *prenom,
                     char *out_date) {
    FILE *f = fopen("C:\\EasySalaire\\saves\\historique\\index.csv", "r");
    if (!f) { strcpy(out_date, ""); return; }
    char line[300];
    strcpy(out_date, "");
    while (fgets(line, sizeof(line), f)) {
        char n[50], p[50], date[30];
        sscanf(line, "%49[^,],%49[^,],%*d,%29[^,]", n, p, date);
        if (strcmp(n, nom) == 0 && strcmp(p, prenom) == 0) {
            if (strlen(out_date) == 0)
                strcpy(out_date, date);
        }
    }
    fclose(f);
}

typedef struct {
    int   numero;
    char  mois[30];
    float net;
} BulletinInfo;

int scannerHistorique(const char *nom, const char *prenom,
                      char fichiers[][100], int max) {
    FILE *f = fopen("C:\\EasySalaire\\saves\\historique\\index.csv", "r");
    if (!f) return 0;
    int nb = 0;
    char line[300];
    while (fgets(line, sizeof(line), f) && nb < max) {
        char n[50], p[50], date[30];
        int  num;
        float net;
        sscanf(line, "%49[^,],%49[^,],%d,%29[^,],%f",
               n, p, &num, date, &net);
        if (strcmp(n, nom) == 0 && strcmp(p, prenom) == 0) {
            sprintf(fichiers[nb++], "N%03d_%s.txt", num, date);
        }
    }
    fclose(f);
    // Sort newest first
    for (int a = 0; a < nb-1; a++)
        for (int b = a+1; b < nb; b++)
            if (strcmp(fichiers[a], fichiers[b]) < 0) {
                char t[100];
                strcpy(t, fichiers[a]);
                strcpy(fichiers[a], fichiers[b]);
                strcpy(fichiers[b], t);
            }
    return nb;
}

void sauvegarderCSV(Employe tab[], int nb) {
    creerDossierSaves();
    FILE *f = fopen("C:\\EasySalaire\\saves\\employes.csv", "w");
    if (f == NULL) return;

    // Header
   fprintf(f, "Nom,Prenom,Poste,Salaire Base,Heures Sup,Prime,CNSS,IR,Salaire Net,Periode,Bulletin\n");

    // Data
    for (int i = 0; i < nb; i++) {
        fprintf(f, "%s,%s,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,N°%03d\n",
    tab[i].nom,
    tab[i].prenom,
    tab[i].poste,
    tab[i].salaire_base,
    tab[i].heures_sup,
    tab[i].prime,
    tab[i].cnss,
    tab[i].ir,
    tab[i].salaire_net,
    tab[i].mois_annee,
    tab[i].numero_bulletin);
    }

    fclose(f);
}

int chargerCSV(Employe tab[]) {
    FILE *f = fopen("C:\\EasySalaire\\saves\\employes.csv", "r");
    if (f == NULL) return 0;

    int nb = 0;
    char line[500];

    // Skip header line
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f) && nb < MAX_EMPLOYES) {
        Employe e;
e.numero_bulletin = 0;
char bull_str[10] = "";
sscanf(line, "%49[^,],%49[^,],%49[^,],%f,%f,%f,%f,%f,%f,%19[^,\n],%9[^,\n]",
    e.nom, e.prenom, e.poste,
    &e.salaire_base, &e.heures_sup, &e.prime,
    &e.cnss, &e.ir, &e.salaire_net,
    e.mois_annee, bull_str);

// Parse N°001 → 1
e.numero_bulletin = 0;
// Find first digit in bull_str
int bi = 0;
while (bull_str[bi] && (bull_str[bi] < '0' || bull_str[bi] > '9'))
    bi++;
e.numero_bulletin = atoi(bull_str + bi);

if (strlen(e.mois_annee) == 0)
    getDateActuelle(e.mois_annee);

// Update counter to highest bulletin number
if (e.numero_bulletin > bulletin_counter)
    bulletin_counter = e.numero_bulletin;
        tab[nb++] = e;

if (strlen(e.mois_annee) == 0) {
    getDateActuelle(e.mois_annee);
}
    }

    fclose(f);
    return nb;
}


int lireFiche(const char *filepath, Employe *out) {
    FILE *f = fopen(filepath, "r");
    if (!f) return 0;

    char line[200];
    while (fgets(line, sizeof(line), f)) {
        float val;
        char  str[100];

        if (sscanf(line, "Nom          : %49[^\n]", str) == 1)
            strcpy(out->nom, str);
        else if (sscanf(line, "Prenom       : %49[^\n]", str) == 1)
            strcpy(out->prenom, str);
        else if (sscanf(line, "Poste        : %49[^\n]", str) == 1)
            strcpy(out->poste, str);
        else if (sscanf(line, "Periode      : %19[^\n]", str) == 1)
            strcpy(out->mois_annee, str);
        else if (sscanf(line, "Salaire base : %f", &val) == 1)
            out->salaire_base = val;
        else if (sscanf(line, "Salaire brut : %f", &val) == 1)
            out->heures_sup = val;
        else if (sscanf(line, "CNSS (9.18%%) : - %f", &val) == 1)
            out->cnss = val;
        else if (sscanf(line, "IR            : - %f", &val) == 1)
            out->ir = val;
        else if (sscanf(line, "SALAIRE NET  : %f", &val) == 1)
            out->salaire_net = val;
    }
    fclose(f);
    return 1;
}



