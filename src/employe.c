#include "employe.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


static int bulletin_counter = 0;
static int id_counter = 0;

int prochainId(void) {
    id_counter++;
    return id_counter;
}

void creerDossierSaves(void) {
    #ifdef _WIN32
        mkdir("C:\\EasySalaire\\saves");
    #else
        mkdir("saves", 0777);
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
        e.id = prochainId();
        tab[(*nb)++] = e;
        sauvegarderHistorique(&tab[(*nb)-1]);
    }
}

// ─── Générer un nouveau bulletin (même employé) ─
void nouveauBulletin(Employe *e, float base, float hsup, float prime) {
    e->salaire_base = base;
    e->heures_sup   = hsup;
    e->prime        = prime;
    calculNet(e);
    getDateActuelle(e->mois_annee);
    bulletin_counter++;
    e->numero_bulletin = bulletin_counter;
    sauvegarderHistorique(e);   // écrit dans le MÊME dossier emp_<id>
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

    // Create employee folder — par ID, jamais par nom (plus de collisions)
    char dossier[200];
    sprintf(dossier, "C:\\EasySalaire\\saves\\historique\\emp_%d", e->id);
    #ifdef _WIN32
        mkdir(dossier);
    #endif

    // Create file
    char filename[300];
    sprintf(filename,
        "C:\\EasySalaire\\saves\\historique\\emp_%d\\N%03d_%s.txt",
        e->id,
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
    fprintf(f, "ID Employe   : %d\n", e->id);
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

int compterBulletins(int id) {
    if (id <= 0) return 0;

    char dossier[200];
    sprintf(dossier,
        "C:\\EasySalaire\\saves\\historique\\emp_%d",
        id);

    char tmpfile[] = "C:\\EasySalaire\\saves\\tmp_count.txt";
    char cmd[400];
    sprintf(cmd, "dir \"%s\" /B /O-N > \"%s\" 2>nul",
            dossier, tmpfile);
    system(cmd);

    FILE *f = fopen(tmpfile, "r");
    if (!f) return 0;

    int count = 0;
    char line[200];
    while (fgets(line, sizeof(line), f))
        if (strstr(line, ".txt")) count++;

    fclose(f);
    return count;
}

void premierBulletin(int id, char *out_date) {
    char dossier[200];
    sprintf(dossier,
        "C:\\EasySalaire\\saves\\historique\\emp_%d",
        id);

    char tmpfile[] = "C:\\EasySalaire\\saves\\tmp_first.txt";
    char cmd[400];
    // /O-N = newest first, so last line = oldest
    sprintf(cmd, "dir \"%s\" /B /ON > \"%s\" 2>nul",
            dossier, tmpfile);
    system(cmd);

    FILE *f = fopen(tmpfile, "r");
    if (!f) { strcpy(out_date, ""); return; }

    char line[200];
    char last[200] = "";
    while (fgets(line, sizeof(line), f))
        if (strstr(line, ".txt"))
            strcpy(last, line);
    fclose(f);

    // Extract date from filename N001_Juillet_2026.txt
    // Find first _ then copy until .txt
    char *start = strchr(last, '_');
    if (!start) { strcpy(out_date, ""); return; }
    start++;

    char *end = strstr(start, ".txt");
    if (!end) { strcpy(out_date, ""); return; }

    int len = end - start;
    strncpy(out_date, start, len);
    out_date[len] = '\0';

    // Replace _ with space
    for (int i = 0; out_date[i]; i++)
        if (out_date[i] == '_') out_date[i] = ' ';
}

void sauvegarderCSV(Employe tab[], int nb) {
    creerDossierSaves();
    FILE *f = fopen("C:\\EasySalaire\\saves\\employes.csv", "w");
    if (f == NULL) return;

    // Header
   fprintf(f, "Id,Nom,Prenom,Poste,Salaire Base,Heures Sup,Prime,CNSS,IR,Salaire Net,Periode,Bulletin\n");

    // Data
    for (int i = 0; i < nb; i++) {
        fprintf(f, "%d,%s,%s,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,N°%03d\n",
    tab[i].id,
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
e.id = 0;
char bull_str[10] = "";
sscanf(line, "%d,%49[^,],%49[^,],%49[^,],%f,%f,%f,%f,%f,%f,%19[^,\n],%9[^,\n]",
    &e.id,
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

// Update counters to highest values seen, so nothing gets reused
if (e.numero_bulletin > bulletin_counter)
    bulletin_counter = e.numero_bulletin;
if (e.id > id_counter)
    id_counter = e.id;

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
    out->id = 0;
    out->numero_bulletin = 0;
    out->heures_sup = 0;
    out->prime = 0;
    while (fgets(line, sizeof(line), f)) {
        float val;
        char  str[100];
        int   idval;

        if (sscanf(line, "ID Employe   : %d", &idval) == 1)
            out->id = idval;
        else if (sscanf(line, "Bulletin N   : N%d", &idval) == 1)
            out->numero_bulletin = idval;
        else if (sscanf(line, "Nom          : %49[^\n]", str) == 1)
            strcpy(out->nom, str);
        else if (sscanf(line, "Prenom       : %49[^\n]", str) == 1)
            strcpy(out->prenom, str);
        else if (sscanf(line, "Poste        : %49[^\n]", str) == 1)
            strcpy(out->poste, str);
        else if (sscanf(line, "Periode      : %19[^\n]", str) == 1)
            strcpy(out->mois_annee, str);
        else if (sscanf(line, "Salaire base : %f", &val) == 1)
            out->salaire_base = val;
        else if (sscanf(line, "Heures sup   : + %f", &val) == 1)
            out->heures_sup = val / 1.5f;
        else if (sscanf(line, "Prime        : + %f", &val) == 1)
            out->prime = val;
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

// ─── Registre complet de tous les bulletins ───
// Ne scanne QUE les employes actuellement presents dans tab[]
// (evite d'inclure des dossiers orphelins d'employes supprimes)
void genererRegistreBulletins(Employe tab[], int nb) {
    creerDossierSaves();

    // Lister tous les dossiers emp_<id>
    char tmp_dirs[] = "C:\\EasySalaire\\saves\\tmp_dirs.txt";
    system("dir \"C:\\EasySalaire\\saves\\historique\" /AD /B > "
           "\"C:\\EasySalaire\\saves\\tmp_dirs.txt\" 2>nul");

    FILE *fd = fopen(tmp_dirs, "r");
    if (!fd) return;

    FILE *out = fopen("C:\\EasySalaire\\saves\\bulletins.csv", "w");
    if (!out) { fclose(fd); return; }

    fprintf(out,
        "Id,Nom,Prenom,Poste,Bulletin,Periode,"
        "Salaire Base,Heures Sup,Prime,CNSS,IR,Salaire Net\n");

    char dirline[100];
    while (fgets(dirline, sizeof(dirline), fd)) {
        dirline[strcspn(dirline, "\r\n")] = 0;
        if (strlen(dirline) == 0) continue;
        if (strncmp(dirline, "emp_", 4) != 0) continue;

        // Verifier que cet id existe encore dans la liste actuelle
        int folder_id = atoi(dirline + 4);
        int existe = 0;
        for (int k = 0; k < nb; k++) {
            if (tab[k].id == folder_id) { existe = 1; break; }
        }
        if (!existe) continue;

        char dossier[250];
        sprintf(dossier, "C:\\EasySalaire\\saves\\historique\\%s", dirline);

        char tmp_files[] = "C:\\EasySalaire\\saves\\tmp_files.txt";
        char cmd[500];
        sprintf(cmd, "dir \"%s\" /B /ON > \"%s\" 2>nul", dossier, tmp_files);
        system(cmd);

        FILE *ff = fopen(tmp_files, "r");
        if (!ff) continue;

        char fline[200];
        while (fgets(fline, sizeof(fline), ff)) {
            fline[strcspn(fline, "\r\n")] = 0;
            if (strlen(fline) == 0 || !strstr(fline, ".txt")) continue;

            char filepath[400];
            sprintf(filepath, "%s\\%s", dossier, fline);

            Employe b = {0};
            if (lireFiche(filepath, &b)) {
                fprintf(out,
                    "%s,%s,%s,%s,N%03d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                    dirline + 4, // id sans le prefixe "emp_"
                    b.nom, b.prenom, b.poste,
                    b.numero_bulletin, b.mois_annee,
                    b.salaire_base, b.heures_sup, b.prime,
                    b.cnss, b.ir, b.salaire_net);
            }
        }
        fclose(ff);
    }

    fclose(fd);
    fclose(out);
}

// ─── Supprimer le dossier historique d'un employe ─
void supprimerHistoriqueEmploye(int id) {
    char dossier[200];
    sprintf(dossier, "C:\\EasySalaire\\saves\\historique\\emp_%d", id);

    char cmd[300];
    sprintf(cmd, "rmdir /S /Q \"%s\" 2>nul", dossier);
    system(cmd);
}
