#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "employe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ECRAN_FORMULAIRE,
    ECRAN_LISTE,
    ECRAN_FICHE
} Ecran;

int main(void) {

    InitWindow(800, 600, "EasySalaire");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(600, 400);
    SetTargetFPS(60);

    Employe employes[MAX_EMPLOYES];
    int nb_employes = 0;
    Ecran ecran_actuel = ECRAN_FORMULAIRE;
    int employe_selectionne = -1;
    char recherche[50] = "";
    int champ_recherche = 0;

    char nom[50]    = "";
    char prenom[50] = "";
    char poste[50]  = "";
    char base[20]   = "";
    char hsup[20]   = "";
    char prime[20]  = "";

    // ─── Champ actif ──────────────────────────
    int champ_actif = -1;
    // 0=nom, 1=prenom, 2=poste, 3=base, 4=hsup, 5=prime

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ══════════════════════════════════════
        // ÉCRAN FORMULAIRE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FORMULAIRE) {

            DrawText("EasySalaire", 20, 20, 30, DARKBLUE);
            DrawText("Ajouter un employe", 20, 60, 18, GRAY);
            DrawLine(20, 85, 780, 85, LIGHTGRAY);

            // Définition des rectangles
            Rectangle r_nom    = {200, 105, 300, 32};
            Rectangle r_prenom = {200, 150, 300, 32};
            Rectangle r_poste  = {200, 195, 300, 32};
            Rectangle r_base   = {200, 240, 300, 32};
            Rectangle r_hsup   = {200, 285, 300, 32};
            Rectangle r_prime  = {200, 330, 300, 32};

            // Clic souris → changer champ actif
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 souris = GetMousePosition();
                if (CheckCollisionPointRec(souris, r_nom))    champ_actif = 0;
                else if (CheckCollisionPointRec(souris, r_prenom)) champ_actif = 1;
                else if (CheckCollisionPointRec(souris, r_poste))  champ_actif = 2;
                else if (CheckCollisionPointRec(souris, r_base))   champ_actif = 3;
                else if (CheckCollisionPointRec(souris, r_hsup))   champ_actif = 4;
                else if (CheckCollisionPointRec(souris, r_prime))  champ_actif = 5;
                else champ_actif = -1;
            }

            // Labels
            DrawText("Nom :",          20, 113, 16, DARKGRAY);
            DrawText("Prenom :",       20, 158, 16, DARKGRAY);
            DrawText("Poste :",        20, 203, 16, DARKGRAY);
            DrawText("Salaire base :", 20, 248, 16, DARKGRAY);
            DrawText("Heures sup :",   20, 293, 16, DARKGRAY);
            DrawText("Prime :",        20, 338, 16, DARKGRAY);

            // Champs texte
            GuiTextBox(r_nom,    nom,    50, champ_actif == 0);
            GuiTextBox(r_prenom, prenom, 50, champ_actif == 1);
            GuiTextBox(r_poste,  poste,  50, champ_actif == 2);
            GuiTextBox(r_base,   base,   20, champ_actif == 3);
            GuiTextBox(r_hsup,   hsup,   20, champ_actif == 4);
            GuiTextBox(r_prime,  prime,  20, champ_actif == 5);

            // Bouton Ajouter
            if (GuiButton((Rectangle){200, 390, 130, 35}, "Ajouter")) {
                if (strlen(nom) > 0 && strlen(base) > 0) {
                    Employe e;
                    strcpy(e.nom,    nom);
                    strcpy(e.prenom, prenom);
                    strcpy(e.poste,  poste);
                    e.salaire_base = atof(base);
                    e.heures_sup   = atof(hsup);
                    e.prime        = atof(prime);
                    ajouterEmploye(employes, &nb_employes, e);

                    nom[0] = prenom[0] = poste[0] = '\0';
                    base[0] = hsup[0] = prime[0] = '\0';
                    champ_actif = -1;
                }
            }

            // Bouton Voir liste
            if (GuiButton((Rectangle){350, 390, 130, 35}, "Voir liste")) {
                ecran_actuel = ECRAN_LISTE;
            }

            char msg[50];
            sprintf(msg, "Employes enregistres : %d", nb_employes);
            DrawText(msg, 20, 450, 14, GRAY);
        }

        // ══════════════════════════════════════
        // ÉCRAN LISTE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_LISTE) {

    // ─── Header ───────────────────────────
    DrawText("EasySalaire", 20, 15, 30, DARKBLUE);
    DrawLine(20, 52, 780, 52, LIGHTGRAY);

    // ─── Bouton Ajouter ───────────────────
    if (GuiButton((Rectangle){650, 15, 120, 30}, "Ajouter")) {
        ecran_actuel = ECRAN_FORMULAIRE;
    }

    // ─── Barre de recherche ───────────────
    DrawText("Recherche :", 20, 65, 14, DARKGRAY);
    Rectangle r_recherche = {120, 61, 200, 26};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 souris = GetMousePosition();
        champ_recherche = CheckCollisionPointRec(souris, r_recherche);
    }
    GuiTextBox(r_recherche, recherche, 50, champ_recherche);

    // Bouton effacer recherche
    if (GuiButton((Rectangle){330, 61, 60, 26}, "Effacer")) {
        recherche[0] = '\0';
    }

    DrawText("Liste des employes", 20, 98, 16, GRAY);
    DrawLine(20, 118, 780, 118, LIGHTGRAY);

    // ─── En-têtes colonnes ────────────────
    DrawRectangle(20, 119, 760, 28, (Color){230,240,255,255});
    DrawText("Nom",         35,  126, 14, DARKBLUE);
    DrawText("Prenom",      180, 126, 14, DARKBLUE);
    DrawText("Poste",       330, 126, 14, DARKBLUE);
    DrawText("Salaire net", 490, 126, 14, DARKBLUE);
    DrawText("Action",      650, 126, 14, DARKBLUE);
    DrawLine(20, 147, 780, 147, LIGHTGRAY);

    // ─── Liste filtrée ────────────────────
    int count = 0;
    for (int i = 0; i < nb_employes; i++) {

        if (strlen(recherche) > 0 &&
            strstr(employes[i].nom,    recherche) == NULL &&
            strstr(employes[i].prenom, recherche) == NULL) {
            continue;
        }

        int y = 155 + count * 42;
        Color ligne = (count % 2 == 0) ? RAYWHITE
                    : (Color){245,248,255,255};
        DrawRectangle(20, y, 760, 38, ligne);

        DrawText(employes[i].nom,    35,  y+12, 13, DARKGRAY);
        DrawText(employes[i].prenom, 180, y+12, 13, DARKGRAY);
        DrawText(employes[i].poste,  330, y+12, 13, DARKGRAY);

        char net[30];
        sprintf(net, "%.2f TND", employes[i].salaire_net);
        DrawText(net, 490, y+12, 13, DARKGREEN);

        if (GuiButton((Rectangle){645, y+7, 95, 24}, "Voir fiche")) {
            employe_selectionne = i;
            ecran_actuel = ECRAN_FICHE;
        }

        count++;
    }

    if (nb_employes == 0) {
        DrawText("Aucun employe enregistre.",
                 280, 320, 15, GRAY);
    } else if (count == 0) {
        DrawText("Aucun resultat trouve.",
                 280, 320, 15, GRAY);
    }

    // ─── Footer ───────────────────────────
    DrawLine(20, 555, 780, 555, LIGHTGRAY);
    char total[50];
    sprintf(total, "Total : %d employe(s)  |  Affichés : %d",
            nb_employes, count);
    DrawText(total, 20, 562, 12, GRAY);
}
        // ══════════════════════════════════════
        // ÉCRAN FICHE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FICHE && employe_selectionne >= 0) {

            Employe *e = &employes[employe_selectionne];
            float brut = calculBrut(e);

            // ─── Titre ────────────────────────
            DrawText("EasySalaire", 20, 20, 30, DARKBLUE);
            DrawText("Fiche de paie", 20, 60, 18, GRAY);
            DrawLine(20, 85, 780, 85, LIGHTGRAY);

            // ─── Infos employé ────────────────
            DrawRectangle(20, 95, 760, 110, (Color){240,248,255,255});
            DrawText("Informations personnelles", 35, 105, 14, DARKBLUE);

            DrawText("Nom :",    35, 128, 15, DARKGRAY);
            DrawText(e->nom,    150, 128, 15, BLACK);

            DrawText("Prenom :", 35, 153, 15, DARKGRAY);
            DrawText(e->prenom, 150, 153, 15, BLACK);

            DrawText("Poste :",  35, 178, 15, DARKGRAY);
            DrawText(e->poste,  150, 178, 15, BLACK);

            // ─── Calculs brut ─────────────────
            DrawLine(20, 215, 780, 215, LIGHTGRAY);
            DrawText("Calcul du salaire brut", 35, 225, 14, DARKBLUE);

            char txt[60];

            sprintf(txt, "%.2f TND", e->salaire_base);
            DrawText("Salaire base :",  35, 248, 15, DARKGRAY);
            DrawText(txt,              250, 248, 15, BLACK);

            sprintf(txt, "+ %.2f TND", e->heures_sup * 1.5f);
            DrawText("Heures sup :",   35, 273, 15, DARKGRAY);
            DrawText(txt,             250, 273, 15, DARKGREEN);

            sprintf(txt, "+ %.2f TND", e->prime);
            DrawText("Prime :",        35, 298, 15, DARKGRAY);
            DrawText(txt,             250, 298, 15, DARKGREEN);

            DrawLine(20, 320, 500, 320, LIGHTGRAY);

            sprintf(txt, "%.2f TND", brut);
            DrawText("Salaire brut :", 35, 330, 16, DARKGRAY);
            DrawText(txt,             250, 330, 16, BLACK);

            // ─── Retenues ─────────────────────
            DrawLine(20, 358, 780, 358, LIGHTGRAY);
            DrawText("Retenues", 35, 368, 14, DARKBLUE);

            sprintf(txt, "- %.2f TND", e->cnss);
            DrawText("CNSS (9.18%) :", 35, 390, 15, DARKGRAY);
            DrawText(txt,             250, 390, 15, RED);

            sprintf(txt, "- %.2f TND", e->ir);
            DrawText("IR :",           35, 415, 15, DARKGRAY);
            DrawText(txt,             250, 415, 15, RED);

            // ─── Salaire net ──────────────────
            DrawRectangle(20, 448, 760, 50, (Color){230,255,230,255});
            DrawLine(20, 448, 780, 448, LIGHTGRAY);

            sprintf(txt, "%.2f TND", e->salaire_net);
            DrawText("SALAIRE NET :", 35, 463, 18, DARKGRAY);
            DrawText(txt,            250, 463, 18, DARKGREEN);

            // ─── Boutons ──────────────────────
            if (GuiButton((Rectangle){35, 520, 150, 35},
                          "Retour liste")) {
                ecran_actuel = ECRAN_LISTE;
                employe_selectionne = -1;
            }

            if (GuiButton((Rectangle){210, 520, 150, 35},
                          "Supprimer")) {
                supprimerEmploye(employes, &nb_employes,
                                 employe_selectionne);
                employe_selectionne = -1;
                ecran_actuel = ECRAN_LISTE;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
