#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "employe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ─── Colors ───────────────────────────────────
// ─── Colors ───────────────────────────────────
static Color COL_BG = {215, 225, 240, 255};
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

   // ─── Load font ────────────────────────────
   Font font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 24, 0, 256);
   SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
   GuiSetFont(font);
   GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

   // ─── Icon ─────────────────────────────────
   Image icon = LoadImage("easysalaire.ico.png");
   SetWindowIcon(icon);
   UnloadImage(icon);



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
        DrawTextEx(font,"EasySalaire", (Vector2){24, 14}, 28,1, WHITE);

        // ══════════════════════════════════════
        // ÉCRAN FORMULAIRE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FORMULAIRE) {

            // Subtitle
            DrawTextEx(font,"Ajouter un employe",
                     (Vector2){24, 75}, 18,1, COL_MUTED);

            // ─── Card ─────────────────────────
            int card_w = 480;
            int card_h = H * 0.75f;
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
            int fw  = card_w - 290;
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
                DrawTextEx(font, labels[i], (Vector2){lx, fy + gap*i + 10}, 16, 1, COL_TEXT);

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
            DrawTextEx(font,msg,(Vector2){ 24, H - 28}, 13,1, COL_MUTED);
        }

        // ══════════════════════════════════════
        // ÉCRAN LISTE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_LISTE) {

            // Subtitle
            DrawTextEx(font,"Liste des employes", (Vector2) {24, 75}, 18,1, COL_MUTED);

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
            DrawTextEx(font,"Recherche :", (Vector2){24, sb_y + 8}, 15,1, COL_TEXT);
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

           DrawTextEx(font, "Nom",         (Vector2){c1, table_y + 10}, 14, 1, WHITE);
           DrawTextEx(font, "Prenom",      (Vector2){c2, table_y + 10}, 14, 1, WHITE);
           DrawTextEx(font, "Poste",       (Vector2){c3, table_y + 10}, 14, 1, WHITE);
           DrawTextEx(font, "Salaire net", (Vector2){c4, table_y + 10}, 14, 1, WHITE);
           DrawTextEx(font, "Action",      (Vector2){c5, table_y + 10}, 14, 1, WHITE);

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

                DrawTextEx(font, employes[i].nom,    (Vector2){c1, ry+15}, 15, 1, COL_TEXT);
                DrawTextEx(font, employes[i].prenom, (Vector2){c2, ry+15}, 15, 1, COL_TEXT);
                DrawTextEx(font, employes[i].poste,  (Vector2){c3, ry+15}, 15, 1, COL_TEXT);

                char net[30];
                sprintf(net, "%.2f TND", employes[i].salaire_net);
                DrawTextEx(font, net, (Vector2){c4, ry+15}, 15, 1, COL_SUCCESS);

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
                DrawTextEx(font,"Aucun employe enregistre.",
                         (Vector2){W/2 - 130, H/2}, 16,1, COL_MUTED);
            else if (count == 0)
                DrawTextEx(font,"Aucun resultat trouve.", (Vector2){W/2 - 110, H/2}, 16,1, COL_MUTED);

            // Footer
            DrawRectangle(0, H - 36, W, 36, COL_CARD);
            DrawRectangle(0, H - 36, W, 1, COL_BORDER);
            char total[60];
            sprintf(total, "Total : %d employe(s)   Affiches : %d",
                    nb_employes, count);
            DrawTextEx(font, total, (Vector2){24, H - 24}, 13, 1, COL_MUTED);
        }

        // ══════════════════════════════════════
        // ÉCRAN FICHE
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_FICHE && employe_selectionne >= 0) {

            Employe *e   = &employes[employe_selectionne];
            float brut   = calculBrut(e);

            DrawTextEx(font,"Fiche de paie", (Vector2){ 24, 75}, 18,1, COL_MUTED);

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
            DrawTextEx(font, "Informations",  (Vector2){card_x + 20, cy + 12}, 13, 1, COL_ACCENT);
            DrawTextEx(font, "Nom :",         (Vector2){card_x + 20, cy + 34}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->nom,          (Vector2){card_x + 120, cy + 34}, 15, 1, COL_TEXT);
            DrawTextEx(font, "Prenom :",      (Vector2){card_x + 20, cy + 56}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->prenom,       (Vector2){card_x + 120, cy + 56}, 15, 1, COL_TEXT);
            DrawTextEx(font, "Poste :",       (Vector2){card_x + 20, cy + 78}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->poste,        (Vector2){card_x + 120, cy + 78}, 15, 1, COL_TEXT);

            // Salary card
            int sc_y = cy + 115;
            DrawRectangleRounded(
                (Rectangle){card_x, sc_y, card_w, 210},
                0.04f, 8, COL_CARD);
            DrawRectangleLinesEx(
    (Rectangle){card_x, sc_y, card_w, 210},
    1.5f, COL_BORDER);

            DrawTextEx(font,"Calcul du salaire", (Vector2){card_x + 20, sc_y + 12}, 13,1, COL_ACCENT);

            char txt[60];
            int lx = card_x + 20;
            int vx = card_x + 320;

           sprintf(txt, "%.2f TND", e->salaire_base);
           DrawTextEx(font, "Salaire base :", (Vector2){lx, sc_y + 40}, 15, 1, COL_MUTED);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 40}, 15, 1, COL_TEXT);

           sprintf(txt, "+ %.2f TND", e->heures_sup * 1.5f);
           DrawTextEx(font, "Heures sup :",   (Vector2){lx, sc_y + 68}, 15, 1, COL_MUTED);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 68}, 15, 1, COL_SUCCESS);

           sprintf(txt, "+ %.2f TND", e->prime);
           DrawTextEx(font, "Prime :",        (Vector2){lx, sc_y + 96}, 15, 1, COL_MUTED);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 96}, 15, 1, COL_SUCCESS);

           DrawRectangle(card_x + 20, sc_y + 122, card_w - 40, 1, COL_BORDER);

           sprintf(txt, "%.2f TND", brut);
           DrawTextEx(font, "Salaire brut :", (Vector2){lx, sc_y + 132}, 16, 1, COL_TEXT);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 132}, 16, 1, COL_TEXT);

           sprintf(txt, "- %.2f TND", e->cnss);
           DrawTextEx(font, "CNSS (9.18%) :", (Vector2){lx, sc_y + 160}, 15, 1, COL_MUTED);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 160}, 15, 1, COL_DANGER);

           sprintf(txt, "- %.2f TND", e->ir);
           DrawTextEx(font, "IR :",           (Vector2){lx, sc_y + 186}, 15, 1, COL_MUTED);
           DrawTextEx(font, txt,              (Vector2){vx, sc_y + 186}, 15, 1, COL_DANGER);

            // Net card
            int net_y = sc_y + 225;
            DrawRectangleRounded(
                (Rectangle){card_x, net_y, card_w, 54},
                0.04f, 8, (Color){220, 252, 231, 255});
            DrawRectangleLinesEx(
    (Rectangle){card_x, net_y, card_w, 54},
    1.5f, (Color){134, 239, 172, 255});
            sprintf(txt, "%.2f TND", e->salaire_net);
            DrawTextEx(font,"SALAIRE NET :",  (Vector2){ lx, net_y + 17}, 18,1, COL_TEXT);
            DrawTextEx(font, txt, (Vector2){vx, net_y + 17}, 20, 1, COL_SUCCESS);

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

            DrawTextEx(font,"Modifier un employe", (Vector2){ 24, 75}, 18,1, COL_MUTED);

            int card_w = 500;
            int card_h = 480;
            int card_x = (W - card_w) / 2;
            int card_y = (H - card_h) / 2;

            DrawRectangleRounded(
                (Rectangle){card_x, card_y, card_w, card_h},
                0.04f, 8, COL_CARD);
           DrawRectangleLinesEx(
    (Rectangle){card_x, card_y, card_w, card_h},
    1.5f, COL_BORDER);

            int lx  = card_x + 30;
            int fx  = card_x + 190;
            int fw  = 270;
            int fh  = 36;
            int fy  = card_y + 35;
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
                DrawTextEx(font, labels[i], (Vector2){lx, fy + gap*i + 10}, 16, 1, COL_TEXT);

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
    UnloadFont(font);

    CloseWindow();
    return 0;
}
