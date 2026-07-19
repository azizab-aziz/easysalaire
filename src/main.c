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
    ECRAN_FICHE,
    ECRAN_MODIFICATION
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
    // ─── Champ actif modification ─────────────
    char mod_nom[50]    = "";
    char mod_prenom[50] = "";
    char mod_poste[50]  = "";
    char mod_base[20]   = "";
    char mod_hsup[20]   = "";
    char mod_prime[20]  = "";
    int  champ_mod      = -1;
    // 0=nom, 1=prenom, 2=poste, 3=base, 4=hsup, 5=prime

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ══════════════════════════════════════
        // ÉCRAN FORMULAIRE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FORMULAIRE) {

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    int cx = W / 2;  // center X

    // ─── Header ───────────────────────────
    DrawText("EasySalaire", 20, 15, 30, DARKBLUE);
    DrawLine(20, 52, W - 20, 52, LIGHTGRAY);
    DrawText("Ajouter un employe", 20, 62, 16, GRAY);

    // ─── Form dimensions ──────────────────
    int field_w = 300;
    int label_x = cx - 200;
    int field_x = cx - 50;
    int start_y = H / 2 - 160;
    int gap     = 48;

    Rectangle r_nom    = {field_x, start_y + gap*0, field_w, 32};
    Rectangle r_prenom = {field_x, start_y + gap*1, field_w, 32};
    Rectangle r_poste  = {field_x, start_y + gap*2, field_w, 32};
    Rectangle r_base   = {field_x, start_y + gap*3, field_w, 32};
    Rectangle r_hsup   = {field_x, start_y + gap*4, field_w, 32};
    Rectangle r_prime  = {field_x, start_y + gap*5, field_w, 32};

    // ─── Mouse click ──────────────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 souris = GetMousePosition();
        if      (CheckCollisionPointRec(souris, r_nom))    champ_actif = 0;
        else if (CheckCollisionPointRec(souris, r_prenom)) champ_actif = 1;
        else if (CheckCollisionPointRec(souris, r_poste))  champ_actif = 2;
        else if (CheckCollisionPointRec(souris, r_base))   champ_actif = 3;
        else if (CheckCollisionPointRec(souris, r_hsup))   champ_actif = 4;
        else if (CheckCollisionPointRec(souris, r_prime))  champ_actif = 5;
        else champ_actif = -1;
    }

    // ─── Keyboard navigation ──────────────
    if (IsKeyPressed(KEY_TAB)) {
        if (IsKeyDown(KEY_LEFT_SHIFT))
            champ_actif = (champ_actif - 1 + 6) % 6;
        else
            champ_actif = (champ_actif + 1) % 6;
    }
    if (IsKeyPressed(KEY_DOWN))  champ_actif = (champ_actif + 1) % 6;
    if (IsKeyPressed(KEY_UP))    champ_actif = (champ_actif - 1 + 6) % 6;
    if (IsKeyPressed(KEY_ENTER)) champ_actif = (champ_actif + 1) % 6;

    // ─── Labels ───────────────────────────
    DrawText("Nom :",          label_x, start_y + gap*0 + 8, 16, DARKGRAY);
    DrawText("Prenom :",       label_x, start_y + gap*1 + 8, 16, DARKGRAY);
    DrawText("Poste :",        label_x, start_y + gap*2 + 8, 16, DARKGRAY);
    DrawText("Salaire base :", label_x, start_y + gap*3 + 8, 16, DARKGRAY);
    DrawText("Heures sup :",   label_x, start_y + gap*4 + 8, 16, DARKGRAY);
    DrawText("Prime :",        label_x, start_y + gap*5 + 8, 16, DARKGRAY);

    // ─── Fields ───────────────────────────
    GuiTextBox(r_nom,    nom,    50, champ_actif == 0);
    GuiTextBox(r_prenom, prenom, 50, champ_actif == 1);
    GuiTextBox(r_poste,  poste,  50, champ_actif == 2);
    GuiTextBox(r_base,   base,   20, champ_actif == 3);
    GuiTextBox(r_hsup,   hsup,   20, champ_actif == 4);
    GuiTextBox(r_prime,  prime,  20, champ_actif == 5);

    // ─── Buttons ──────────────────────────
    int btn_y  = start_y + gap*6 + 10;
    int btn_w  = 130;
    int btn_h  = 35;

    if (GuiButton((Rectangle){cx - 140, btn_y, btn_w, btn_h}, "Ajouter")) {
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

    if (GuiButton((Rectangle){cx + 10, btn_y, btn_w, btn_h}, "Voir liste")) {
        ecran_actuel = ECRAN_LISTE;
    }

    // ─── Counter ──────────────────────────
    char msg[50];
    sprintf(msg, "Employes enregistres : %d", nb_employes);
    DrawText(msg, 20, H - 30, 13, GRAY);
}

        // ══════════════════════════════════════
        // ÉCRAN LISTE
        // ══════════════════════════════════════
       if (ecran_actuel == ECRAN_LISTE) {

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // ─── Header ───────────────────────────
    DrawText("EasySalaire", 20, 15, 30, DARKBLUE);
    DrawLine(20, 52, W - 20, 52, LIGHTGRAY);

    if (GuiButton((Rectangle){W - 150, 15, 120, 30}, "Ajouter")) {
        ecran_actuel = ECRAN_FORMULAIRE;
    }

    // ─── Search bar ───────────────────────
    DrawText("Recherche :", 20, 65, 14, DARKGRAY);
    Rectangle r_recherche = {120, 61, 220, 26};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 souris = GetMousePosition();
        champ_recherche = CheckCollisionPointRec(souris, r_recherche);
    }
    GuiTextBox(r_recherche, recherche, 50, champ_recherche);

    if (GuiButton((Rectangle){350, 61, 70, 26}, "Effacer")) {
        recherche[0] = '\0';
    }

    DrawText("Liste des employes", 20, 98, 16, GRAY);
    DrawLine(20, 118, W - 20, 118, LIGHTGRAY);

    // ─── Column headers ───────────────────
    float col1 = W * 0.04f;
    float col2 = W * 0.22f;
    float col3 = W * 0.40f;
    float col4 = W * 0.60f;
    float col5 = W * 0.80f;

    DrawRectangle(20, 119, W - 40, 28, (Color){230,240,255,255});
    DrawText("Nom",         col1, 126, 14, DARKBLUE);
    DrawText("Prenom",      col2, 126, 14, DARKBLUE);
    DrawText("Poste",       col3, 126, 14, DARKBLUE);
    DrawText("Salaire net", col4, 126, 14, DARKBLUE);
    DrawText("Action",      col5, 126, 14, DARKBLUE);
    DrawLine(20, 147, W - 20, 147, LIGHTGRAY);

    // ─── Filtered list ────────────────────
    int count = 0;
    for (int i = 0; i < nb_employes; i++) {

        if (strlen(recherche) > 0 &&
            strstr(employes[i].nom,    recherche) == NULL &&
            strstr(employes[i].prenom, recherche) == NULL) {
            continue;
        }

        int y = 155 + count * 42;
        Color bg = (count % 2 == 0) ? RAYWHITE
                 : (Color){245,248,255,255};
        DrawRectangle(20, y, W - 40, 38, bg);

        DrawText(employes[i].nom,    col1, y+12, 13, DARKGRAY);
        DrawText(employes[i].prenom, col2, y+12, 13, DARKGRAY);
        DrawText(employes[i].poste,  col3, y+12, 13, DARKGRAY);

        char net[30];
        sprintf(net, "%.2f TND", employes[i].salaire_net);
        DrawText(net, col4, y+12, 13, DARKGREEN);

        if (GuiButton((Rectangle){col5, y+7, 110, 24}, "Voir fiche")) {
            employe_selectionne = i;
            ecran_actuel = ECRAN_FICHE;
        }

        count++;
    }

    if (nb_employes == 0)
        DrawText("Aucun employe enregistre.", W/2 - 120, H/2, 15, GRAY);
    else if (count == 0)
        DrawText("Aucun resultat trouve.",    W/2 - 100, H/2, 15, GRAY);

    // ─── Footer ───────────────────────────
    DrawLine(20, H - 30, W - 20, H - 30, LIGHTGRAY);
    char total[60];
    sprintf(total, "Total : %d employe(s)  |  Affiches : %d",
            nb_employes, count);
    DrawText(total, 20, H - 22, 12, GRAY);
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
            if (GuiButton((Rectangle){385, 520, 150, 35}, "Modifier")) {
                 // Pré-remplir les champs avec les données actuelles
                 strcpy(mod_nom,    employes[employe_selectionne].nom);
                 strcpy(mod_prenom, employes[employe_selectionne].prenom);
                 strcpy(mod_poste,  employes[employe_selectionne].poste);
                 sprintf(mod_base,  "%.2f", employes[employe_selectionne].salaire_base);
                 sprintf(mod_hsup,  "%.2f", employes[employe_selectionne].heures_sup);
                 sprintf(mod_prime, "%.2f", employes[employe_selectionne].prime);
                 champ_mod = -1;
                 ecran_actuel = ECRAN_MODIFICATION;
          }
        }
        // ══════════════════════════════════════
        // ÉCRAN MODIFICATION
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_MODIFICATION && employe_selectionne >= 0) {

           // ─── Header ───────────────────────
           DrawText("EasySalaire", 20, 15, 30, DARKBLUE);
           DrawLine(20, 52, 780, 52, LIGHTGRAY);
           DrawText("Modifier un employe", 20, 62, 16, GRAY);

           // ─── Rectangles ───────────────────
           Rectangle rm_nom    = {220, 100, 300, 32};
           Rectangle rm_prenom = {220, 145, 300, 32};
           Rectangle rm_poste  = {220, 190, 300, 32};
           Rectangle rm_base   = {220, 235, 300, 32};
           Rectangle rm_hsup   = {220, 280, 300, 32};
           Rectangle rm_prime  = {220, 325, 300, 32};

    // ─── Navigation souris ────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 souris = GetMousePosition();
        if      (CheckCollisionPointRec(souris, rm_nom))    champ_mod = 0;
        else if (CheckCollisionPointRec(souris, rm_prenom)) champ_mod = 1;
        else if (CheckCollisionPointRec(souris, rm_poste))  champ_mod = 2;
        else if (CheckCollisionPointRec(souris, rm_base))   champ_mod = 3;
        else if (CheckCollisionPointRec(souris, rm_hsup))   champ_mod = 4;
        else if (CheckCollisionPointRec(souris, rm_prime))  champ_mod = 5;
        else champ_mod = -1;
    }

    // ─── Navigation clavier ───────────
    if (IsKeyPressed(KEY_TAB)) {
        if (IsKeyDown(KEY_LEFT_SHIFT))
            champ_mod = (champ_mod - 1 + 6) % 6;
        else
            champ_mod = (champ_mod + 1) % 6;
    }
    if (IsKeyPressed(KEY_DOWN))
        champ_mod = (champ_mod + 1) % 6;
    if (IsKeyPressed(KEY_UP))
        champ_mod = (champ_mod - 1 + 6) % 6;

    // ─── Labels ───────────────────────
    DrawText("Nom :",          20, 108, 16, DARKGRAY);
    DrawText("Prenom :",       20, 153, 16, DARKGRAY);
    DrawText("Poste :",        20, 198, 16, DARKGRAY);
    DrawText("Salaire base :", 20, 243, 16, DARKGRAY);
    DrawText("Heures sup :",   20, 288, 16, DARKGRAY);
    DrawText("Prime :",        20, 333, 16, DARKGRAY);

    // ─── Champs pré-remplis ───────────
    GuiTextBox(rm_nom,    mod_nom,    50, champ_mod == 0);
    GuiTextBox(rm_prenom, mod_prenom, 50, champ_mod == 1);
    GuiTextBox(rm_poste,  mod_poste,  50, champ_mod == 2);
    GuiTextBox(rm_base,   mod_base,   20, champ_mod == 3);
    GuiTextBox(rm_hsup,   mod_hsup,   20, champ_mod == 4);
    GuiTextBox(rm_prime,  mod_prime,  20, champ_mod == 5);

    // ─── Bouton Sauvegarder ───────────
    if (GuiButton((Rectangle){220, 385, 150, 35}, "Sauvegarder")) {
        if (strlen(mod_nom) > 0 && strlen(mod_base) > 0) {
            strcpy(employes[employe_selectionne].nom,    mod_nom);
            strcpy(employes[employe_selectionne].prenom, mod_prenom);
            strcpy(employes[employe_selectionne].poste,  mod_poste);
            employes[employe_selectionne].salaire_base = atof(mod_base);
            employes[employe_selectionne].heures_sup   = atof(mod_hsup);
            employes[employe_selectionne].prime        = atof(mod_prime);

            // Recalcul automatique
            calculNet(&employes[employe_selectionne]);

            ecran_actuel = ECRAN_FICHE;
        }
    }

    // ─── Bouton Annuler ───────────────
    if (GuiButton((Rectangle){385, 385, 150, 35}, "Annuler")) {
        ecran_actuel = ECRAN_FICHE;
    }
}

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
