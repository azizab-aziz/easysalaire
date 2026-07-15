#ifndef EMPLOYE_H
#define EMPLOYE_H

// ─── Constantes ───────────────────────────────
#define MAX_EMPLOYES     100
#define TAUX_CNSS        0.0918f   // 9.18% part salariale
#define TAUX_AT          0.005f    // 0.5% accident travail

// ─── Struct Employe ───────────────────────────
typedef struct {
    char  nom[50];
    char  prenom[50];
    char  poste[50];
    float salaire_base;
    float heures_sup;
    float prime;
    float cnss;
    float ir;
    float salaire_net;
} Employe;

// ─── Prototypes ───────────────────────────────

// Calculs
float calculBrut(Employe *e);
float calculCNSS(float brut);
float calculIR(float brut);
float calculNet(Employe *e);

// Gestion
void ajouterEmploye(Employe tab[], int *nb, Employe e);
void supprimerEmploye(Employe tab[], int *nb, int index);

#endif
