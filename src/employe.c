#include "employe.h"
#include <stdio.h>

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

// ─── Ajouter un employé ───────────────────────
void ajouterEmploye(Employe tab[], int *nb, Employe e) {
    if (*nb < MAX_EMPLOYES) {
        calculNet(&e);
        tab[(*nb)++] = e;
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
    char filename[100];
    sprintf(filename, "%s_%s_fiche.txt", e->nom, e->prenom);

    FILE *f = fopen(filename, "w");
    if (f == NULL) return;

    float brut = calculBrut(e);

    fprintf(f, "========================================\n");
    fprintf(f, "         FICHE DE PAIE - EasySalaire   \n");
    fprintf(f, "========================================\n\n");

    fprintf(f, "INFORMATIONS PERSONNELLES\n");
    fprintf(f, "-------------------------\n");
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

void sauvegarderCSV(Employe tab[], int nb) {
    FILE *f = fopen("employes.csv", "w");
    if (f == NULL) return;

    // Header
    fprintf(f, "Nom,Prenom,Poste,Salaire Base,Heures Sup,Prime,CNSS,IR,Salaire Net\n");

    // Data
    for (int i = 0; i < nb; i++) {
        fprintf(f, "%s,%s,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
            tab[i].nom,
            tab[i].prenom,
            tab[i].poste,
            tab[i].salaire_base,
            tab[i].heures_sup,
            tab[i].prime,
            tab[i].cnss,
            tab[i].ir,
            tab[i].salaire_net);
    }

    fclose(f);
}
