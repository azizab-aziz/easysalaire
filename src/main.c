#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "employe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ─── Colors ───────────────────────────────────
// ─── Colors ───────────────────────────────────
static Color COL_BG      = {244, 246, 249, 255};
static Color COL_HEADER  = {26,  43,  74,  255};
static Color COL_ACCENT  = {37,  99,  235, 255};
static Color COL_TEXT    = {30,  41,  59,  255};
static Color COL_MUTED   = {100, 116, 139, 255};
static Color COL_SUCCESS = {22,  163, 74,  255};
static Color COL_DANGER  = {220, 38,  38,  255};
static Color COL_CARD    = {255, 255, 255, 255};
static Color COL_BORDER  = {226, 232, 240, 255};
static Color COL_ROW_ALT = {248, 250, 252, 255};
static Color COL_HDR_ROW = {30,  41,  59,  255};
typedef enum {
    ECRAN_FORMULAIRE,
    ECRAN_LISTE,
    ECRAN_FICHE,
    ECRAN_MODIFICATION
} Ecran;

int main(void) {

    InitWindow(900, 650, "EasySalaire");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(700, 500);
    SetTargetFPS(60);

    Employe employes[MAX_EMPLOYES];
    int nb_employes        = 0;
    Ecran ecran_actuel     = ECRAN_FORMULAIRE;
    int employe_selectionne = -1;

    char recherche[50] = "";
    int  champ_recherche = 0;
    int  champ_actif     = -1;
    int  champ_mod       = -1;

    char nom[50]   = "", prenom[50] = "", poste[50] = "";
    char base[20]  = "", hsup[20]   = "", prime[20] = "";

    char mod_nom[50]   = "", mod_prenom[50] = "", mod_poste[50] = "";
    char mod_base[20]  = "", mod_hsup[20]   = "", mod_prime[20] = "";

    while (!WindowShouldClose()) {

        int W = GetScreenWidth();
        int H = GetScreenHeight();

        BeginDrawing();
        ClearBackground(COL_BG);

        // ══════════════════════════════════════
        // HEADER BAR (all screens)
        // ══════════════════════════════════════
        DrawRectangle(0, 0, W, 58, COL_HEADER);
        DrawText("EasySalaire", 24, 14, 28, WHITE);

        // ══════════════════════════════════════
        // ÉCRAN FORMULAIRE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FORMULAIRE) {

            // Subtitle
            DrawText("Ajouter un employe",
                     24, 75, 18, COL_MUTED);

            // ─── Card ─────────────────────────
            int card_w = 520;
            int card_h = 460;
            int card_x = (W - card_w) / 2;
            int card_y = (H - card_h) / 2;

            DrawRectangleRounded(
                (Rectangle){card_x, card_y, card_w, card_h},
                0.04f, 8, COL_CARD);
            DrawRectangleLinesEx(
    (Rectangle){card_x, card_y, card_w, card_h},
    1.5f, COL_BORDER);

            // ─── Fields ───────────────────────
            int lx  = card_x + 30;
            int fx  = card_x + 180;
            int fw  = 290;
            int fh  = 36;
            int fy  = card_y + 30;
            int gap = 62;

            Rectangle r_nom    = {fx, fy + gap*0, fw, fh};
            Rectangle r_prenom = {fx, fy + gap*1, fw, fh};
            Rectangle r_poste  = {fx, fy + gap*2, fw, fh};
            Rectangle r_base   = {fx, fy + gap*3, fw, fh};
            Rectangle r_hsup   = {fx, fy + gap*4, fw, fh};
            Rectangle r_prime  = {fx, fy + gap*5, fw, fh};

            // Mouse
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                if      (CheckCollisionPointRec(m, r_nom))    champ_actif = 0;
                else if (CheckCollisionPointRec(m, r_prenom)) champ_actif = 1;
                else if (CheckCollisionPointRec(m, r_poste))  champ_actif = 2;
                else if (CheckCollisionPointRec(m, r_base))   champ_actif = 3;
                else if (CheckCollisionPointRec(m, r_hsup))   champ_actif = 4;
                else if (CheckCollisionPointRec(m, r_prime))  champ_actif = 5;
                else champ_actif = -1;
            }

            // Keyboard
            if (IsKeyPressed(KEY_TAB)) {
                if (IsKeyDown(KEY_LEFT_SHIFT))
                    champ_actif = (champ_actif - 1 + 6) % 6;
                else
                    champ_actif = (champ_actif + 1) % 6;
            }
            if (IsKeyPressed(KEY_DOWN))  champ_actif = (champ_actif + 1) % 6;
            if (IsKeyPressed(KEY_UP))    champ_actif = (champ_actif - 1 + 6) % 6;
            if (IsKeyPressed(KEY_ENTER)) champ_actif = (champ_actif + 1) % 6;

            // Labels
            const char *labels[] = {
                "Nom :", "Prenom :", "Poste :",
                "Salaire base :", "Heures sup :", "Prime :"
            };
            for (int i = 0; i < 6; i++)
                DrawText(labels[i], lx, fy + gap*i + 10,
                         16, COL_TEXT);

            // Active field highlight
            Rectangle rects[] = {
                r_nom, r_prenom, r_poste,
                r_base, r_hsup, r_prime
            };
            for (int i = 0; i < 6; i++) {
                DrawRectangleRec(rects[i], COL_CARD);
                Color border = (champ_actif == i)
                             ? COL_ACCENT : COL_BORDER;
                DrawRectangleLinesEx(rects[i], 1.5f, border);
            }

            GuiTextBox(r_nom,    nom,    50, champ_actif == 0);
            GuiTextBox(r_prenom, prenom, 50, champ_actif == 1);
            GuiTextBox(r_poste,  poste,  50, champ_actif == 2);
            GuiTextBox(r_base,   base,   20, champ_actif == 3);
            GuiTextBox(r_hsup,   hsup,   20, champ_actif == 4);
            GuiTextBox(r_prime,  prime,  20, champ_actif == 5);

            // Buttons
            int btn_y = card_y + card_h - 60;
            int btn_w = 140;
            int btn_h = 38;

            // Ajouter button (accent)
            DrawRectangleRounded(
                (Rectangle){card_x + 60, btn_y, btn_w, btn_h},
                0.3f, 8, COL_ACCENT);
            if (GuiButton((Rectangle){card_x + 60, btn_y, btn_w, btn_h},
                          "Ajouter")) {
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

            // Voir liste button
            DrawRectangleRounded(
                (Rectangle){card_x + 220, btn_y, btn_w, btn_h},
                0.3f, 8, COL_BORDER);
            if (GuiButton((Rectangle){card_x + 220, btn_y, btn_w, btn_h},
                          "Voir liste")) {
                ecran_actuel = ECRAN_LISTE;
            }

            // Counter
            char msg[50];
            sprintf(msg, "Employes enregistres : %d", nb_employes);
            DrawText(msg, 24, H - 28, 13, COL_MUTED);
        }

        // ══════════════════════════════════════
        // ÉCRAN LISTE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_LISTE) {

            // Subtitle
            DrawText("Liste des employes", 24, 75, 18, COL_MUTED);

            // Ajouter button in header
            DrawRectangleRounded(
                (Rectangle){W - 160, 10, 135, 38},
                0.3f, 8, COL_ACCENT);
            if (GuiButton((Rectangle){W - 160, 10, 135, 38},
                          "Ajouter")) {
                ecran_actuel = ECRAN_FORMULAIRE;
            }

            // ─── Search bar ───────────────────
            int sb_y = 105;
            DrawText("Recherche :", 24, sb_y + 8, 15, COL_TEXT);
            Rectangle r_rech = {140, sb_y, 250, 34};
            DrawRectangleRec(r_rech, COL_CARD);
            DrawRectangleLinesEx(r_rech,
                1.5f, champ_recherche ? COL_ACCENT : COL_BORDER);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                champ_recherche = CheckCollisionPointRec(m, r_rech);
            }
            GuiTextBox(r_rech, recherche, 50, champ_recherche);

            DrawRectangleRounded(
                (Rectangle){402, sb_y, 80, 34},
                0.3f, 8, COL_BORDER);
            if (GuiButton((Rectangle){402, sb_y, 80, 34}, "Effacer"))
                recherche[0] = '\0';

            // ─── Table ────────────────────────
            int table_y = 152;
            int row_h   = 46;

            // Header row
            DrawRectangle(20, table_y, W - 40, 36, COL_HDR_ROW);

            float c1 = 20  + (W-40)*0.02f;
            float c2 = 20  + (W-40)*0.22f;
            float c3 = 20  + (W-40)*0.42f;
            float c4 = 20  + (W-40)*0.62f;
            float c5 = 20  + (W-40)*0.80f;

            DrawText("Nom",         c1, table_y + 10, 14, WHITE);
            DrawText("Prenom",      c2, table_y + 10, 14, WHITE);
            DrawText("Poste",       c3, table_y + 10, 14, WHITE);
            DrawText("Salaire net", c4, table_y + 10, 14, WHITE);
            DrawText("Action",      c5, table_y + 10, 14, WHITE);

            // Rows
            int count = 0;
            for (int i = 0; i < nb_employes; i++) {

                if (strlen(recherche) > 0 &&
                    strstr(employes[i].nom,    recherche) == NULL &&
                    strstr(employes[i].prenom, recherche) == NULL)
                    continue;

                int ry = table_y + 36 + count * row_h;
                Color bg = (count % 2 == 0) ? COL_CARD : COL_ROW_ALT;
                DrawRectangle(20, ry, W - 40, row_h, bg);
                DrawRectangle(20, ry + row_h - 1, W - 40, 1, COL_BORDER);

                DrawText(employes[i].nom,    c1, ry+15, 15, COL_TEXT);
                DrawText(employes[i].prenom, c2, ry+15, 15, COL_TEXT);
                DrawText(employes[i].poste,  c3, ry+15, 15, COL_TEXT);

                char net[30];
                sprintf(net, "%.2f TND", employes[i].salaire_net);
                DrawText(net, c4, ry+15, 15, COL_SUCCESS);

                DrawRectangleRounded(
                    (Rectangle){c5, ry+8, 110, 28},
                    0.3f, 8, COL_ACCENT);
                if (GuiButton((Rectangle){c5, ry+8, 110, 28},
                              "Voir fiche")) {
                    employe_selectionne = i;
                    ecran_actuel = ECRAN_FICHE;
                }
                count++;
            }

            if (nb_employes == 0)
                DrawText("Aucun employe enregistre.",
                         W/2 - 130, H/2, 16, COL_MUTED);
            else if (count == 0)
                DrawText("Aucun resultat trouve.",
                         W/2 - 110, H/2, 16, COL_MUTED);

            // Footer
            DrawRectangle(0, H - 36, W, 36, COL_CARD);
            DrawRectangle(0, H - 36, W, 1, COL_BORDER);
            char total[60];
            sprintf(total, "Total : %d employe(s)   Affiches : %d",
                    nb_employes, count);
            DrawText(total, 24, H - 24, 13, COL_MUTED);
        }

        // ══════════════════════════════════════
        // ÉCRAN FICHE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FICHE && employe_selectionne >= 0) {

            Employe *e   = &employes[employe_selectionne];
            float brut   = calculBrut(e);

            DrawText("Fiche de paie", 24, 75, 18, COL_MUTED);

            int cx    = W / 2;
            int card_w = 520;
            int card_x = cx - card_w / 2;
            int cy    = 110;

            // Info card
            DrawRectangleRounded(
                (Rectangle){card_x, cy, card_w, 100},
                0.04f, 8, COL_CARD);
           DrawRectangleLinesEx(
    (Rectangle){card_x, cy, card_w, 100},
    1.5f, COL_BORDER);
            DrawText("Informations",  card_x + 20, cy + 12, 13, COL_ACCENT);
            DrawText("Nom :",         card_x + 20, cy + 34, 15, COL_MUTED);
            DrawText(e->nom,          card_x + 120, cy + 34, 15, COL_TEXT);
            DrawText("Prenom :",      card_x + 20, cy + 56, 15, COL_MUTED);
            DrawText(e->prenom,       card_x + 120, cy + 56, 15, COL_TEXT);
            DrawText("Poste :",       card_x + 20, cy + 78, 15, COL_MUTED);
            DrawText(e->poste,        card_x + 120, cy + 78, 15, COL_TEXT);

            // Salary card
            int sc_y = cy + 115;
            DrawRectangleRounded(
                (Rectangle){card_x, sc_y, card_w, 210},
                0.04f, 8, COL_CARD);
            DrawRectangleLinesEx(
    (Rectangle){card_x, sc_y, card_w, 210},
    1.5f, COL_BORDER);

            DrawText("Calcul du salaire",
                     card_x + 20, sc_y + 12, 13, COL_ACCENT);

            char txt[60];
            int lx = card_x + 20;
            int vx = card_x + 320;

            sprintf(txt, "%.2f TND", e->salaire_base);
            DrawText("Salaire base :", lx, sc_y + 40, 15, COL_MUTED);
            DrawText(txt,              vx, sc_y + 40, 15, COL_TEXT);

            sprintf(txt, "+ %.2f TND", e->heures_sup * 1.5f);
            DrawText("Heures sup :",   lx, sc_y + 68, 15, COL_MUTED);
            DrawText(txt,              vx, sc_y + 68, 15, COL_SUCCESS);

            sprintf(txt, "+ %.2f TND", e->prime);
            DrawText("Prime :",        lx, sc_y + 96, 15, COL_MUTED);
            DrawText(txt,              vx, sc_y + 96, 15, COL_SUCCESS);

            DrawRectangle(card_x + 20, sc_y + 122,
                          card_w - 40, 1, COL_BORDER);

            sprintf(txt, "%.2f TND", brut);
            DrawText("Salaire brut :", lx, sc_y + 132, 16, COL_TEXT);
            DrawText(txt,              vx, sc_y + 132, 16, COL_TEXT);

            sprintf(txt, "- %.2f TND", e->cnss);
            DrawText("CNSS (9.18%) :", lx, sc_y + 160, 15, COL_MUTED);
            DrawText(txt,              vx, sc_y + 160, 15, COL_DANGER);

            sprintf(txt, "- %.2f TND", e->ir);
            DrawText("IR :",           lx, sc_y + 186, 15, COL_MUTED);
            DrawText(txt,              vx, sc_y + 186, 15, COL_DANGER);

            // Net card
            int net_y = sc_y + 225;
            DrawRectangleRounded(
                (Rectangle){card_x, net_y, card_w, 54},
                0.04f, 8, (Color){220, 252, 231, 255});
            DrawRectangleLinesEx(
    (Rectangle){card_x, net_y, card_w, 54},
    1.5f, (Color){134, 239, 172, 255});
            sprintf(txt, "%.2f TND", e->salaire_net);
            DrawText("SALAIRE NET :", lx, net_y + 17, 18, COL_TEXT);
            DrawText(txt, vx, net_y + 17, 20, COL_SUCCESS);

            // Buttons
            int btn_y = net_y + 70;
            int btn_w = 145;
            int btn_h = 38;

            DrawRectangleRounded(
                (Rectangle){card_x, btn_y, btn_w, btn_h},
                0.3f, 8, COL_BORDER);
            if (GuiButton((Rectangle){card_x, btn_y, btn_w, btn_h},
                          "Retour liste")) {
                ecran_actuel = ECRAN_LISTE;
                employe_selectionne = -1;
            }

            DrawRectangleRounded(
                (Rectangle){card_x + 160, btn_y, btn_w, btn_h},
                0.3f, 8, COL_ACCENT);
            if (GuiButton((Rectangle){card_x + 160, btn_y, btn_w, btn_h},
                          "Modifier")) {
                strcpy(mod_nom,    e->nom);
                strcpy(mod_prenom, e->prenom);
                strcpy(mod_poste,  e->poste);
                sprintf(mod_base,  "%.2f", e->salaire_base);
                sprintf(mod_hsup,  "%.2f", e->heures_sup);
                sprintf(mod_prime, "%.2f", e->prime);
                champ_mod = -1;
                ecran_actuel = ECRAN_MODIFICATION;
            }

            DrawRectangleRounded(
                (Rectangle){card_x + 320, btn_y, btn_w, btn_h},
                0.3f, 8, COL_DANGER);
            if (GuiButton((Rectangle){card_x + 320, btn_y, btn_w, btn_h},
                          "Supprimer")) {
                supprimerEmploye(employes, &nb_employes,
                                 employe_selectionne);
                employe_selectionne = -1;
                ecran_actuel = ECRAN_LISTE;
            }
        }

        // ══════════════════════════════════════
        // ÉCRAN MODIFICATION
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_MODIFICATION && employe_selectionne >= 0) {

            DrawText("Modifier un employe", 24, 75, 18, COL_MUTED);

            int card_w = 520;
            int card_h = 460;
            int card_x = (W - card_w) / 2;
            int card_y = (H - card_h) / 2;

            DrawRectangleRounded(
                (Rectangle){card_x, card_y, card_w, card_h},
                0.04f, 8, COL_CARD);
           DrawRectangleLinesEx(
    (Rectangle){card_x, card_y, card_w, card_h},
    1.5f, COL_BORDER);

            int lx  = card_x + 30;
            int fx  = card_x + 180;
            int fw  = 290;
            int fh  = 36;
            int fy  = card_y + 30;
            int gap = 62;

            Rectangle rm_nom    = {fx, fy + gap*0, fw, fh};
            Rectangle rm_prenom = {fx, fy + gap*1, fw, fh};
            Rectangle rm_poste  = {fx, fy + gap*2, fw, fh};
            Rectangle rm_base   = {fx, fy + gap*3, fw, fh};
            Rectangle rm_hsup   = {fx, fy + gap*4, fw, fh};
            Rectangle rm_prime  = {fx, fy + gap*5, fw, fh};

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                if      (CheckCollisionPointRec(m, rm_nom))    champ_mod = 0;
                else if (CheckCollisionPointRec(m, rm_prenom)) champ_mod = 1;
                else if (CheckCollisionPointRec(m, rm_poste))  champ_mod = 2;
                else if (CheckCollisionPointRec(m, rm_base))   champ_mod = 3;
                else if (CheckCollisionPointRec(m, rm_hsup))   champ_mod = 4;
                else if (CheckCollisionPointRec(m, rm_prime))  champ_mod = 5;
                else champ_mod = -1;
            }

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

            const char *labels[] = {
                "Nom :", "Prenom :", "Poste :",
                "Salaire base :", "Heures sup :", "Prime :"
            };
            for (int i = 0; i < 6; i++)
                DrawText(labels[i], lx, fy + gap*i + 10,
                         16, COL_TEXT);

            Rectangle rects[] = {
                rm_nom, rm_prenom, rm_poste,
                rm_base, rm_hsup, rm_prime
            };
            for (int i = 0; i < 6; i++) {
                DrawRectangleRec(rects[i], COL_CARD);
                Color border = (champ_mod == i)
                             ? COL_ACCENT : COL_BORDER;
                DrawRectangleLinesEx(rects[i], 1.5f, border);
            }

            GuiTextBox(rm_nom,    mod_nom,    50, champ_mod == 0);
            GuiTextBox(rm_prenom, mod_prenom, 50, champ_mod == 1);
            GuiTextBox(rm_poste,  mod_poste,  50, champ_mod == 2);
            GuiTextBox(rm_base,   mod_base,   20, champ_mod == 3);
            GuiTextBox(rm_hsup,   mod_hsup,   20, champ_mod == 4);
            GuiTextBox(rm_prime,  mod_prime,  20, champ_mod == 5);

            int btn_y = card_y + card_h - 60;
            int btn_w = 140;
            int btn_h = 38;

            DrawRectangleRounded(
                (Rectangle){card_x + 60, btn_y, btn_w, btn_h},
                0.3f, 8, COL_ACCENT);
            if (GuiButton((Rectangle){card_x + 60, btn_y, btn_w, btn_h},
                          "Sauvegarder")) {
                if (strlen(mod_nom) > 0 && strlen(mod_base) > 0) {
                    strcpy(employes[employe_selectionne].nom,    mod_nom);
                    strcpy(employes[employe_selectionne].prenom, mod_prenom);
                    strcpy(employes[employe_selectionne].poste,  mod_poste);
                    employes[employe_selectionne].salaire_base = atof(mod_base);
                    employes[employe_selectionne].heures_sup   = atof(mod_hsup);
                    employes[employe_selectionne].prime        = atof(mod_prime);
                    calculNet(&employes[employe_selectionne]);
                    ecran_actuel = ECRAN_FICHE;
                }
            }

            DrawRectangleRounded(
                (Rectangle){card_x + 220, btn_y, btn_w, btn_h},
                0.3f, 8, COL_BORDER);
            if (GuiButton((Rectangle){card_x + 220, btn_y, btn_w, btn_h},
                          "Annuler")) {
                ecran_actuel = ECRAN_FICHE;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
