#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "employe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
    ECRAN_MODIFICATION,
    ECRAN_STATS
} Ecran;


// ─── Compare functions for sort ───────────────
int compareNom(const void *a, const void *b) {
    return strcmp(((Employe*)a)->nom, ((Employe*)b)->nom);
}
int compareNomDesc(const void *a, const void *b) {
    return strcmp(((Employe*)b)->nom, ((Employe*)a)->nom);
}
int comparePoste(const void *a, const void *b) {
    return strcmp(((Employe*)a)->poste, ((Employe*)b)->poste);
}
int comparePosteDesc(const void *a, const void *b) {
    return strcmp(((Employe*)b)->poste, ((Employe*)a)->poste);
}
int compareSalaire(const void *a, const void *b) {
    float diff = ((Employe*)a)->salaire_net - ((Employe*)b)->salaire_net;
    return (diff > 0) - (diff < 0);
}
int compareSalaireDesc(const void *a, const void *b) {
    float diff = ((Employe*)b)->salaire_net - ((Employe*)a)->salaire_net;
    return (diff > 0) - (diff < 0);
}


int main(void) {


    //SetConfigFlags(FLAG_WINDOW_HIGHDPI);
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
    int sort_col = -1;
    int sort_asc = 1;
    float splash_timer = 2.5f;
    int splash_done = 0;
    int  champ_actif     = -1;
    int  champ_mod      = -1;
    int  champ_mod_prev = -1;
    int popup_save = 0; // 0=off, 1=show popup
    int save_index = -1;
    int popup_confirm = 0;
    char success_msg[100] = "";
    float success_timer = 0.0f;

    char nom[50]   = "", prenom[50] = "", poste[50] = "";
    char base[20]  = "", hsup[20]   = "", prime[20] = "";

    char mod_nom[50]   = "", mod_prenom[50] = "", mod_poste[50] = "";
    char mod_base[20]  = "", mod_hsup[20]   = "", mod_prime[20] = "";

    nb_employes = chargerCSV(employes);
    printf("Loaded %d employees from CSV\n", nb_employes);

    while (!WindowShouldClose()) {

        int W = GetScreenWidth();
        int H = GetScreenHeight();

        //Vector2 dpiScale = GetWindowScaleDPI();
        //SetMouseScale(1.0f / dpiScale.x, 1.0f / dpiScale.y);

        BeginDrawing();
        ClearBackground(COL_BG);

        // ══════════════════════════════════════
// SPLASH SCREEN
// ══════════════════════════════════════
if (!splash_done) {
    splash_timer -= GetFrameTime();
    if (splash_timer <= 0.0f) splash_done = 1;

    // Background
    ClearBackground(COL_HEADER);

    float progress = 1.0f - (splash_timer / 2.5f);

    // ─── Fade in alpha ────────────────
    unsigned char alpha = (unsigned char)(
        progress < 0.4f ? (progress / 0.4f) * 255
                        : 255);

    // ─── Animated circle ──────────────
    float scale = 0.5f + progress * 0.5f;
    int   cx    = W / 2;
    int   cy    = H / 2 - 60;
    float r1    = 90 * scale;
    float r2    = 70 * scale;

    // Outer circle navy
    DrawCircle(cx, cy, r1,
        (Color){26, 43, 74, alpha});
    // Inner circle accent
    DrawCircle(cx, cy, r2,
        (Color){37, 99, 235, alpha});

    // White card shape
    int cw = (int)(80 * scale);
    int ch = (int)(90 * scale);
    DrawRectangleRounded(
        (Rectangle){cx - cw/2, cy - ch/2, cw, ch},
        0.15f, 8,
        (Color){255, 255, 255, alpha});

    // Green net bar
    DrawRectangleRounded(
        (Rectangle){cx - cw/2, cy + ch/2 - 18, cw, 18},
        0.1f, 8,
        (Color){22, 163, 74, alpha});

    // Lines on card
    for (int i = 0; i < 3; i++) {
        DrawRectangleRounded(
            (Rectangle){cx - cw/2 + 8,
                        cy - ch/2 + 12 + i*18,
                        cw - 16, 8},
            0.5f, 4,
            (Color){226, 232, 240, alpha});
    }

    // ─── Title ────────────────────────
    Vector2 ts = MeasureTextEx(font,
        "EasySalaire", 42, 1);
    DrawTextEx(font, "EasySalaire",
        (Vector2){cx - ts.x/2, cy + r1 + 20},
        42, 1,
        (Color){255, 255, 255, alpha});

    // Subtitle
    Vector2 ss = MeasureTextEx(font,
        "Gestion de paie simplifiee", 16, 1);
    DrawTextEx(font,
        "Gestion de paie simplifiee",
        (Vector2){cx - ss.x/2, cy + r1 + 72},
        16, 1,
        (Color){148, 163, 184, alpha});

    // ─── Loading bar ──────────────────
    int bar_w = 200;
    int bar_x = cx - bar_w / 2;
    int bar_y = H - 80;

    DrawRectangleRounded(
        (Rectangle){bar_x, bar_y, bar_w, 6},
        0.5f, 4,
        (Color){255, 255, 255, 40});
    DrawRectangleRounded(
        (Rectangle){bar_x, bar_y,
                    (int)(bar_w * progress), 6},
        0.5f, 4,
        (Color){37, 99, 235, alpha});

    DrawTextEx(font, "Chargement...",
        (Vector2){cx - 50, bar_y + 16},
        13, 1,
        (Color){148, 163, 184, alpha});

    // ─── Fade out at end ──────────────
    if (splash_timer < 0.4f) {
        float fade = splash_timer / 0.4f;
        unsigned char fa = (unsigned char)(
            (1.0f - fade) * 200);
        DrawRectangle(0, 0, W, H,
            (Color){0, 0, 0, fa});
    }

    EndDrawing();
    continue;
}

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
            int card_w = 560;
            int card_h = H * 0.85f;
            int card_x = (W - card_w) / 2;
            int card_y = 70+ (H - card_h - 70 ) / 2;

            DrawRectangleRounded(
                (Rectangle){card_x, card_y, card_w, card_h},
                0.04f, 8, COL_CARD);
            DrawRectangleLinesEx(
    (Rectangle){card_x, card_y, card_w, card_h},
    1.5f, COL_BORDER);

            // ─── Fields ───────────────────────
            int lx  = card_x + 30;
            int fx  = card_x + 180;
            int fw  = card_w - 230;
            int fh  = 36;
            int fy  = card_y + 30;
            int gap = 72;

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


            // ─── Keyboard navigation ──────────────────
            if (IsKeyPressed(KEY_TAB)) {
                if (IsKeyDown(KEY_LEFT_SHIFT))
                  champ_actif = (champ_actif - 1 + 6) % 6;
                else
                  champ_actif = (champ_actif + 1) % 6;
           }
if (IsKeyPressed(KEY_DOWN))
    champ_actif = (champ_actif + 1) % 6;
if (IsKeyPressed(KEY_UP))
    champ_actif = (champ_actif - 1 + 6) % 6;

if (IsKeyPressed(KEY_ENTER)) {
    // Check if all fields are filled
    if (strlen(nom)    > 0 &&
        strlen(prenom) > 0 &&
        strlen(poste)  > 0 &&
        strlen(base)   > 0 &&
        strlen(hsup)   > 0 &&
        strlen(prime)  > 0) {
        // All filled → same as clicking Ajouter
        Employe e;
        strcpy(e.nom,    nom);
        strcpy(e.prenom, prenom);
        strcpy(e.poste,  poste);
        e.salaire_base = atof(base);
        e.heures_sup   = atof(hsup);
        e.prime        = atof(prime);
        ajouterEmploye(employes, &nb_employes, e);
        sauvegarderCSV(employes, nb_employes);
        strcpy(success_msg, "Employe ajoute avec succes !");
        success_timer = 3.0f;
        nom[0] = prenom[0] = poste[0] = '\0';
        base[0] = hsup[0] = prime[0] = '\0';
        champ_actif = 0; // back to first field
    } else {
        // Not all filled → go to next field
        champ_actif = (champ_actif + 1) % 6;
    }
}
            // Labels
          const char *labels[] = {
    "Nom :", "Prenom :", "Poste :",
    "Salaire base :", "Heures sup :", "Prime :"
};

// Store display values before clearing
char *mod_fields[] = {
    mod_nom, mod_prenom, mod_poste,
    mod_base, mod_hsup, mod_prime
};

for (int i = 0; i < 6; i++) {
    DrawTextEx(font, labels[i],
               (Vector2){lx, fy + gap*i + 10},
               16, 1, COL_TEXT);
}

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


            int btn_w = 140;
            int btn_gap = 20;
            int total_btn_w = btn_w * 2 + btn_gap;
            int btn_x = card_x + (card_w - total_btn_w) / 2;
            int btn_y = fy + gap * 6 + 5;

            if (GuiButton((Rectangle){btn_x, btn_y, btn_w, 38},
                          "Ajouter")) {
                if (strlen(nom)    > 0 &&
                    strlen(prenom) > 0 &&
                    strlen(poste)  > 0 &&
                    strlen(base)   > 0 &&
                    strlen(hsup)   > 0 &&
                    strlen(prime)  > 0) {
                    Employe e;
                    strcpy(e.nom,    nom);
                    strcpy(e.prenom, prenom);
                    strcpy(e.poste,  poste);


                    e.salaire_base = atof(base);
                    e.heures_sup   = atof(hsup);
                    e.prime        = atof(prime);
                    ajouterEmploye(employes, &nb_employes, e);
                    sauvegarderCSV(employes, nb_employes);
                    nom[0] = prenom[0] = poste[0] = '\0';
                    base[0] = hsup[0] = prime[0] = '\0';
                    champ_actif = 0;
                }
            }

            if (GuiButton((Rectangle){btn_x + btn_w + btn_gap, btn_y, btn_w, 38},
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
                employe_selectionne = -1;
            }

            if (GuiButton((Rectangle){W - 310, 10, 135, 38},
                         "Statistiques")) {
               ecran_actuel = ECRAN_STATS;
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
            if (GuiButton((Rectangle){500, sb_y, 150, 34}, "Exporter Excel")) {
                system("python C:\\EasySalaire\\saves\\export_excel.py");
            }

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

// ─── Clickable sort headers ───────────────────
const char *arr0 = (sort_col == 0) ? (sort_asc ? " ^" : " v") : "";
const char *arr2 = (sort_col == 1) ? (sort_asc ? " ^" : " v") : "";
const char *arr3 = (sort_col == 2) ? (sort_asc ? " ^" : " v") : "";

char h_nom[20], h_poste[20], h_sal[20];
sprintf(h_nom,  "Nom%s",         arr0);
sprintf(h_poste,"Poste%s",       arr2);
sprintf(h_sal,  "Salaire net%s", arr3);

// Nom header — clickable
Rectangle r_h_nom  = {c1, table_y, (W-40)*0.20f, 36};
Rectangle r_h_post = {c3, table_y, (W-40)*0.20f, 36};
Rectangle r_h_sal  = {c4, table_y, (W-40)*0.18f, 36};

if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, r_h_nom)) {
        if (sort_col == 0) sort_asc = !sort_asc;
        else { sort_col = 0; sort_asc = 1; }
        if (sort_asc) qsort(employes, nb_employes, sizeof(Employe), compareNom);
        else          qsort(employes, nb_employes, sizeof(Employe), compareNomDesc);
        sauvegarderCSV(employes, nb_employes);
    }
    if (CheckCollisionPointRec(m, r_h_post)) {
        if (sort_col == 1) sort_asc = !sort_asc;
        else { sort_col = 1; sort_asc = 1; }
        if (sort_asc) qsort(employes, nb_employes, sizeof(Employe), comparePoste);
        else          qsort(employes, nb_employes, sizeof(Employe), comparePosteDesc);
        sauvegarderCSV(employes, nb_employes);
    }
    if (CheckCollisionPointRec(m, r_h_sal)) {
        if (sort_col == 2) sort_asc = !sort_asc;
        else { sort_col = 2; sort_asc = 1; }
        if (sort_asc) qsort(employes, nb_employes, sizeof(Employe), compareSalaire);
        else          qsort(employes, nb_employes, sizeof(Employe), compareSalaireDesc);
        sauvegarderCSV(employes, nb_employes);
    }
}

// Hover effect
Vector2 mouse = GetMousePosition();
if (CheckCollisionPointRec(mouse, r_h_nom))
    DrawRectangle(c1, table_y, (W-40)*0.20f, 36, (Color){255,255,255,30});
if (CheckCollisionPointRec(mouse, r_h_post))
    DrawRectangle(c3, table_y, (W-40)*0.20f, 36, (Color){255,255,255,30});
if (CheckCollisionPointRec(mouse, r_h_sal))
    DrawRectangle(c4, table_y, (W-40)*0.18f, 36, (Color){255,255,255,30});

DrawTextEx(font, h_nom,   (Vector2){c1, table_y + 10}, 14, 1, WHITE);
DrawTextEx(font, "Prenom",(Vector2){c2, table_y + 10}, 14, 1, WHITE);
DrawTextEx(font, h_poste, (Vector2){c3, table_y + 10}, 14, 1, WHITE);
DrawTextEx(font, h_sal,   (Vector2){c4, table_y + 10}, 14, 1, WHITE);
DrawTextEx(font, "Action",(Vector2){c5, table_y + 10}, 14, 1, WHITE);
            // Rows
            int count = 0;
            for (int i = 0; i < nb_employes; i++) {

                   if (strlen(recherche) > 0) {
                       char r_lower[50], nom_lower[50], prenom_lower[50];

                       // Copy and convert to lowercase
                       for (int j = 0; recherche[j]; j++)
                          r_lower[j] = tolower(recherche[j]);
                       r_lower[strlen(recherche)] = '\0';

                       for (int j = 0; employes[i].nom[j]; j++)
                          nom_lower[j] = tolower(employes[i].nom[j]);
                       nom_lower[strlen(employes[i].nom)] = '\0';

                       for (int j = 0; employes[i].prenom[j]; j++)
                          prenom_lower[j] = tolower(employes[i].prenom[j]);
                        prenom_lower[strlen(employes[i].prenom)] = '\0';

                       if (strstr(nom_lower,    r_lower) == NULL &&
                          strstr(prenom_lower, r_lower) == NULL)
                          continue;
}

                int ry = table_y + 36 + count * row_h;
                Color bg = (count % 2 == 0) ? COL_CARD : COL_ROW_ALT;
                DrawRectangle(20, ry, W - 40, row_h, bg);
                DrawRectangle(20, ry + row_h - 1, W - 40, 1, COL_BORDER);

                // ─── Truncate text to fit column ──────────
char nom_cut[20], prenom_cut[20], poste_cut[22];

strncpy(nom_cut,    employes[i].nom,    19); nom_cut[19]    = '\0';
strncpy(prenom_cut, employes[i].prenom, 19); prenom_cut[19] = '\0';
strncpy(poste_cut,  employes[i].poste,  21); poste_cut[21]  = '\0';

// Add "..." if truncated
if (strlen(employes[i].nom)    > 19) strcpy(nom_cut    + 16, "...");
if (strlen(employes[i].prenom) > 19) strcpy(prenom_cut + 16, "...");
if (strlen(employes[i].poste)  > 21) strcpy(poste_cut  + 18, "...");

DrawTextEx(font, nom_cut,    (Vector2){c1, ry+15}, 15, 1, COL_TEXT);
DrawTextEx(font, prenom_cut, (Vector2){c2, ry+15}, 15, 1, COL_TEXT);
DrawTextEx(font, poste_cut,  (Vector2){c3, ry+15}, 15, 1, COL_TEXT);

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
        if (ecran_actuel == ECRAN_FICHE && employe_selectionne >= 0 && popup_save == 0) {

               // Block all interaction if confirm popup is open
    if (popup_confirm == 1) goto draw_fiche_end;

            Employe *e   = &employes[employe_selectionne];
            float brut   = calculBrut(e);

           DrawTextEx(font,"Fiche de paie", (Vector2){ 24, 68}, 16,1, COL_MUTED);

            int card_w = (W > 900) ? W - 200 : W - 60;
            int card_x = (W - card_w) / 2;
            int cy     = 100;
            int vx = card_x + card_w * 0.65f;

            // Info card
            DrawRectangleRounded(
                (Rectangle){card_x, cy, card_w, 100},
                0.04f, 8, COL_CARD);
           DrawRectangleLinesEx(
    (Rectangle){card_x, cy, card_w, 100},
    1.5f, COL_BORDER);
           DrawTextEx(font, "Informations",
           (Vector2){card_x + 20, cy + 12},
           13, 1, COL_ACCENT);

// Date on the right side
char date_label[40];
if (strlen(e->mois_annee) == 0)
    getDateActuelle(e->mois_annee);
sprintf(date_label, "Periode : %s", e->mois_annee);
Vector2 date_sz = MeasureTextEx(font, date_label, 13, 1);
DrawTextEx(font, date_label,
           (Vector2){card_x + card_w - date_sz.x - 20, cy + 12},
           13, 1, COL_MUTED);
            DrawTextEx(font, "Nom :",         (Vector2){card_x + 20, cy + 34}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->nom,          (Vector2){card_x + 120, cy + 34}, 15, 1, COL_TEXT);
            DrawTextEx(font, "Prenom :",      (Vector2){card_x + 20, cy + 56}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->prenom,       (Vector2){card_x + 120, cy + 56}, 15, 1, COL_TEXT);
            DrawTextEx(font, "Poste :",       (Vector2){card_x + 20, cy + 78}, 15, 1, COL_MUTED);
            DrawTextEx(font, e->poste,        (Vector2){card_x + 120, cy + 78}, 15, 1, COL_TEXT);

            // Salary card
            int sc_y = cy + 120;
            DrawRectangleRounded(
                (Rectangle){card_x, sc_y, card_w, 210},
                0.04f, 8, COL_CARD);
            DrawRectangleLinesEx(
    (Rectangle){card_x, sc_y, card_w, 210},
    1.5f, COL_BORDER);

            DrawTextEx(font,"Calcul du salaire", (Vector2){card_x + 20, sc_y + 12}, 13,1, COL_ACCENT);

            char txt[60];
            int lx = card_x + 20;

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
            int btn_y = net_y + 65;
            DrawRectangleRounded(
                (Rectangle){card_x, net_y, card_w, 54},
                0.04f, 8, (Color){220, 252, 231, 255});
            DrawRectangleLinesEx(
    (Rectangle){card_x, net_y, card_w, 54},
    1.5f, (Color){134, 239, 172, 255});
            sprintf(txt, "%.2f TND", e->salaire_net);
            DrawTextEx(font,"SALAIRE NET :",  (Vector2){ lx, net_y + 17}, 18,1, COL_TEXT);
            DrawTextEx(font, txt, (Vector2){vx, net_y + 17}, 20, 1, COL_SUCCESS);
// ─── Buttons ──────────────────────────────
            int btn_h   = 38;
            int btn_w   = 145;
            int total_w = btn_w * 4 + 30 * 3;
            int start_x = (W - total_w) / 2;

            if (GuiButton((Rectangle){start_x, btn_y, btn_w, btn_h},
                          "Retour liste")) {
                ecran_actuel = ECRAN_LISTE;
                employe_selectionne = -1;
            }


            Rectangle rect_mod = {start_x + btn_w + 30, btn_y, btn_w, btn_h};

            bool inside_mod = CheckCollisionPointRec(GetMousePosition(), rect_mod);
            bool clicked_mod = GuiButton(rect_mod, "Modifier");

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                DrawText(TextFormat("Click at %.0f,%.0f | Inside rect? %s | GuiButton fired? %s",
                    GetMousePosition().x, GetMousePosition().y,
                    inside_mod ? "YES" : "NO",
                    clicked_mod ? "YES" : "NO"),
                    10, H - 30, 14, RED);
            }

            if (clicked_mod) {
               // Store original values for display
               strcpy(mod_nom,    e->nom);
               strcpy(mod_prenom, e->prenom);
               strcpy(mod_poste,  e->poste);
               sprintf(mod_base,  "%.2f", e->salaire_base);
               sprintf(mod_hsup,  "%.2f", e->heures_sup);
               sprintf(mod_prime, "%.2f", e->prime);

               // Store originals for hint display
               char orig_nom[50], orig_prenom[50], orig_poste[50];
               char orig_base[20], orig_hsup[20], orig_prime[20];
               strcpy(orig_nom,    mod_nom);
               strcpy(orig_prenom, mod_prenom);
               strcpy(orig_poste,  mod_poste);
               strcpy(orig_base,   mod_base);
               strcpy(orig_hsup,   mod_hsup);
               strcpy(orig_prime,  mod_prime);

              champ_mod = 0;
              champ_mod_prev = -1;  // ← reset
              ecran_actuel = ECRAN_MODIFICATION;
            }

            if (GuiButton((Rectangle){start_x + (btn_w + 30)*2, btn_y, btn_w, btn_h},
                          "Enregistrer")) {
                save_index = employe_selectionne;
                popup_save = 1;
            }

            if (GuiButton((Rectangle){start_x + (btn_w + 30)*3, btn_y, btn_w, btn_h},
              "Supprimer")) {
    popup_confirm = 1;
}
           draw_fiche_end:;
        }
// ══════════════════════════════════════
        // ÉCRAN MODIFICATION
        // ══════════════════════════════════════
        if (ecran_actuel == ECRAN_MODIFICATION && employe_selectionne >= 0 && popup_save == 0) {

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

if (IsKeyPressed(KEY_ENTER)) {
    if (champ_mod == 5) {
        // Last field → auto save
        if (strlen(mod_nom) > 0 && strlen(mod_base) > 0) {
            strcpy(employes[employe_selectionne].nom,    mod_nom);
            strcpy(employes[employe_selectionne].prenom, mod_prenom);
            strcpy(employes[employe_selectionne].poste,  mod_poste);
            employes[employe_selectionne].salaire_base = atof(mod_base);
            employes[employe_selectionne].heures_sup   = atof(mod_hsup);
            employes[employe_selectionne].prime        = atof(mod_prime);
            calculNet(&employes[employe_selectionne]);
            sauvegarderCSV(employes, nb_employes);
            strcpy(success_msg, "Modifications sauvegardees !");
            success_timer = 3.0f;
            ecran_actuel = ECRAN_FICHE;
        }
    } else {
        // Go to next field
        champ_mod = (champ_mod + 1) % 6;
    }
}

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

            // ─── Hint text ────────────────────────────
            DrawTextEx(font, "Modifiez les champs puis appuyez sur Entree",
               (Vector2){card_x + 20, card_y + card_h - 85},
               12, 1, COL_MUTED);


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
                    sauvegarderCSV(employes, nb_employes);
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

            // ─── Clear field when it becomes active ───
           // ─── Clear field only when user starts typing ─
if (champ_mod != champ_mod_prev) {
    champ_mod_prev = champ_mod;
}

int key = GetCharPressed();
if (key > 0 && champ_mod >= 0) {
    switch(champ_mod) {
        case 0: if (strlen(mod_nom)    == strlen(employes[employe_selectionne].nom))    mod_nom[0]    = '\0'; break;
        case 1: if (strlen(mod_prenom) == strlen(employes[employe_selectionne].prenom)) mod_prenom[0] = '\0'; break;
        case 2: if (strlen(mod_poste)  == strlen(employes[employe_selectionne].poste))  mod_poste[0]  = '\0'; break;
    }
}
        }





// ══════════════════════════════════════
// ÉCRAN STATISTIQUES
// ══════════════════════════════════════
if (ecran_actuel == ECRAN_STATS) {

    DrawTextEx(font, "Statistiques", (Vector2){24, 68},
               16, 1, COL_MUTED);

    // ─── Back button ──────────────────
    if (GuiButton((Rectangle){W - 160, 10, 135, 38},
                  "Retour liste")) {
        ecran_actuel = ECRAN_LISTE;
    }

    // ─── Calculate stats ──────────────
    float total  = 0.0f;
    float min_s  = nb_employes > 0 ? employes[0].salaire_net : 0;
    float max_s  = 0.0f;
    int   idx_min = 0;
    int   idx_max = 0;

    for (int i = 0; i < nb_employes; i++) {
        total += employes[i].salaire_net;
        if (employes[i].salaire_net < min_s) {
            min_s   = employes[i].salaire_net;
            idx_min = i;
        }
        if (employes[i].salaire_net > max_s) {
            max_s   = employes[i].salaire_net;
            idx_max = i;
        }
    }

    float moyenne = nb_employes > 0 ? total / nb_employes : 0;

    // ─── No employees ─────────────────
    if (nb_employes == 0) {
        DrawTextEx(font, "Aucun employe enregistre.",
                   (Vector2){W/2 - 130, H/2},
                   16, 1, COL_MUTED);
    } else {

        int cx     = W / 2;
        int card_w = (W > 900) ? W - 200 : W - 60;
        int card_x = (W - card_w) / 2;
        int cy     = 100;

        // ─── Title card ───────────────
        DrawRectangleRounded(
            (Rectangle){card_x, cy, card_w, 50},
            0.04f, 8, COL_HEADER);
        DrawTextEx(font, "Masse salariale globale",
                   (Vector2){card_x + 20, cy + 14},
                   16, 1, WHITE);

        char buf[60];
        sprintf(buf, "%d employe(s)", nb_employes);
        Vector2 sz = MeasureTextEx(font, buf, 14, 1);
        DrawTextEx(font, buf,
                   (Vector2){card_x + card_w - sz.x - 20, cy + 16},
                   14, 1, (Color){148, 163, 184, 255});

        // ─── 4 stat cards ─────────────
        int cw4  = (card_w - 60) / 4;
        int ch4  = 110;
        int cy4  = cy + 70;

        // Total
        DrawRectangleRounded(
            (Rectangle){card_x, cy4, cw4, ch4},
            0.08f, 8, COL_CARD);
        DrawRectangleLinesEx(
            (Rectangle){card_x, cy4, cw4, ch4},
            1.5f, COL_BORDER);
        DrawTextEx(font, "Total",
                   (Vector2){card_x + 12, cy4 + 12},
                   13, 1, COL_MUTED);
        sprintf(buf, "%.2f", total);
        DrawTextEx(font, buf,
                   (Vector2){card_x + 12, cy4 + 38},
                   17, 1, COL_ACCENT);
        DrawTextEx(font, "TND",
                   (Vector2){card_x + 12, cy4 + 62},
                   12, 1, COL_MUTED);

        // Moyenne
        DrawRectangleRounded(
            (Rectangle){card_x + cw4 + 20, cy4, cw4, ch4},
            0.08f, 8, COL_CARD);
        DrawRectangleLinesEx(
            (Rectangle){card_x + cw4 + 20, cy4, cw4, ch4},
            1.5f, COL_BORDER);
        DrawTextEx(font, "Moyenne",
                   (Vector2){card_x + cw4 + 32, cy4 + 12},
                   13, 1, COL_MUTED);
        sprintf(buf, "%.2f", moyenne);
        DrawTextEx(font, buf,
                   (Vector2){card_x + cw4 + 32, cy4 + 38},
                   17, 1, COL_ACCENT);
        DrawTextEx(font, "TND / employe",
                   (Vector2){card_x + cw4 + 32, cy4 + 62},
                   12, 1, COL_MUTED);

        // Min
        DrawRectangleRounded(
            (Rectangle){card_x + (cw4 + 20)*2, cy4, cw4, ch4},
            0.08f, 8, (Color){255, 245, 245, 255});
        DrawRectangleLinesEx(
            (Rectangle){card_x + (cw4 + 20)*2, cy4, cw4, ch4},
            1.5f, (Color){254, 202, 202, 255});
        DrawTextEx(font, "Minimum",
                   (Vector2){card_x + (cw4+20)*2 + 12, cy4 + 12},
                   13, 1, COL_MUTED);
        sprintf(buf, "%.2f", min_s);
        DrawTextEx(font, buf,
                   (Vector2){card_x + (cw4+20)*2 + 12, cy4 + 38},
                   17, 1, COL_DANGER);
        DrawTextEx(font, employes[idx_min].nom,
                   (Vector2){card_x + (cw4+20)*2 + 12, cy4 + 62},
                   12, 1, COL_MUTED);

        // Max
        DrawRectangleRounded(
            (Rectangle){card_x + (cw4 + 20)*3, cy4, cw4, ch4},
            0.08f, 8, (Color){240, 253, 244, 255});
        DrawRectangleLinesEx(
            (Rectangle){card_x + (cw4 + 20)*3, cy4, cw4, ch4},
            1.5f, (Color){134, 239, 172, 255});
        DrawTextEx(font, "Maximum",
                   (Vector2){card_x + (cw4+20)*3 + 12, cy4 + 12},
                   13, 1, COL_MUTED);
        sprintf(buf, "%.2f", max_s);
        DrawTextEx(font, buf,
                   (Vector2){card_x + (cw4+20)*3 + 12, cy4 + 38},
                   17, 1, COL_SUCCESS);
        DrawTextEx(font, employes[idx_max].nom,
                   (Vector2){card_x + (cw4+20)*3 + 12, cy4 + 62},
                   12, 1, COL_MUTED);

        // ─── Detail table ─────────────
        int ty = cy4 + ch4 + 30;

        DrawRectangle(card_x, ty, card_w, 32, COL_HDR_ROW);
        DrawTextEx(font, "Employe",
                   (Vector2){card_x + 20, ty + 9},
                   13, 1, WHITE);
        DrawTextEx(font, "Poste",
                   (Vector2){card_x + card_w*0.35f, ty + 9},
                   13, 1, WHITE);
        DrawTextEx(font, "Salaire net",
                   (Vector2){card_x + card_w*0.65f, ty + 9},
                   13, 1, WHITE);
        DrawTextEx(font, "% du total",
                   (Vector2){card_x + card_w*0.82f, ty + 9},
                   13, 1, WHITE);

        for (int i = 0; i < nb_employes; i++) {
            int ry  = ty + 32 + i * 40;
            Color bg = (i % 2 == 0) ? COL_CARD : COL_ROW_ALT;
            DrawRectangle(card_x, ry, card_w, 40, bg);
            DrawRectangle(card_x, ry + 39, card_w, 1, COL_BORDER);

            // Name
            char full[100];
            sprintf(full, "%s %s", employes[i].nom,
                    employes[i].prenom);
            DrawTextEx(font, full,
                       (Vector2){card_x + 20, ry + 12},
                       13, 1, COL_TEXT);

            // Poste
            char poste_cut[22];
            strncpy(poste_cut, employes[i].poste, 21);
            poste_cut[21] = '\0';
            if (strlen(employes[i].poste) > 21)
                strcpy(poste_cut + 18, "...");
            DrawTextEx(font, poste_cut,
                       (Vector2){card_x + card_w*0.35f, ry + 12},
                       13, 1, COL_MUTED);

            // Salaire net
            sprintf(buf, "%.2f TND", employes[i].salaire_net);
            DrawTextEx(font, buf,
                       (Vector2){card_x + card_w*0.65f, ry + 12},
                       13, 1, COL_SUCCESS);

            // Percentage bar
            float pct = total > 0
                      ? (employes[i].salaire_net / total) * 100.0f
                      : 0;
            int bar_x = card_x + card_w * 0.82f;
            int bar_w = card_w * 0.14f;

            DrawRectangle(bar_x, ry + 14, bar_w, 12,
                          COL_BORDER);
            DrawRectangle(bar_x, ry + 14,
                          (int)(bar_w * pct / 100.0f), 12,
                          COL_ACCENT);

            sprintf(buf, "%.1f%%", pct);
            DrawTextEx(font, buf,
                       (Vector2){bar_x + bar_w + 8, ry + 12},
                       11, 1, COL_MUTED);
        }
    }
}




// ─── Popup choix format ───────────────────
if (popup_save == 1) {

    DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 150});

    int pw = 340;
    int ph = 220;
    int px = (W - pw) / 2;
    int py = (H - ph) / 2;

    DrawRectangleRounded(
        (Rectangle){px, py, pw, ph},
        0.08f, 8, COL_CARD);
    DrawRectangleLinesEx(
        (Rectangle){px, py, pw, ph},
        2.0f, COL_ACCENT);

    DrawTextEx(font, "Enregistrer la fiche",
               (Vector2){px + 20, py + 18},
               17, 1, COL_TEXT);
    DrawTextEx(font, "Choisissez le format :",
               (Vector2){px + 20, py + 46},
               13, 1, COL_MUTED);

    // Fiche .txt
    if (GuiButton((Rectangle){px + 15, py + 80, 90, 38},
                  "Fiche .txt")) {
        if (save_index >= 0)
            sauvegarderFiche(&employes[save_index]);
            popup_save = 0;
            strcpy(success_msg, "Fiche .txt sauvgardee !");
            success_timer = 3.0f;
    }

    // Liste .csv
    if (GuiButton((Rectangle){px + 125, py + 80, 90, 38},
                  "Liste .csv")) {
        sauvegarderCSV(employes, nb_employes);
        popup_save = 0;
        strcpy(success_msg, "Fiche .txt sauvgardee !");
        success_timer = 3.0f;
    }

    // Fiche PDF
    if (GuiButton((Rectangle){px + 235, py + 80, 90, 38},
                  "Fiche PDF")) {
        if (save_index >= 0) {
            Employe *ep = &employes[save_index];
            char cmd[1024];
            sprintf(cmd,
                "python C:\\EasySalaire\\saves\\export_pdf.py "
                "\"%s\" \"%s\" \"%s\" %.2f %.2f %.2f %.2f %.2f %.2f",
                ep->nom, ep->prenom, ep->poste,
                ep->salaire_base, ep->heures_sup * 1.5f, ep->prime,
                ep->cnss, ep->ir, ep->salaire_net);
            system(cmd);
        }
        popup_save = 0;
        strcpy(success_msg, "Fiche PDF generee !");
        success_timer = 3.0f;
    }

    // Annuler
    if (GuiButton((Rectangle){px + 100, py + 140, 130, 36},
                  "Annuler")) {
        popup_save = 0;
    }

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        !CheckCollisionPointRec(mouse,
         (Rectangle){px, py, pw, ph})) {
        popup_save = 0;
    }
}

    // ══════════════════════════════════════
// POPUP CONFIRMATION SUPPRESSION
// ══════════════════════════════════════
if (popup_confirm == 1) {

    // Dark overlay
    DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 150});

    // Popup card
    int pw = 360;
    int ph = 200;
    int px = (W - pw) / 2;
    int py = (H - ph) / 2;

    DrawRectangleRounded(
        (Rectangle){px, py, pw, ph},
        0.08f, 8, COL_CARD);
    DrawRectangleLinesEx(
        (Rectangle){px, py, pw, ph},
        2.0f, COL_DANGER);

    // Warning icon area
    DrawRectangleRounded(
        (Rectangle){px + pw/2 - 25, py + 18, 50, 50},
        0.3f, 8, (Color){254, 226, 226, 255});
    DrawTextEx(font, "!",
               (Vector2){px + pw/2 - 8, py + 28},
               30, 1, COL_DANGER);

    // Title
    DrawTextEx(font, "Confirmer la suppression",
               (Vector2){px + 20, py + 82},
               16, 1, COL_TEXT);

    // Employee name
    if (employe_selectionne >= 0) {
        char confirm_msg[100];
        sprintf(confirm_msg, "Supprimer %s %s ?",
                employes[employe_selectionne].nom,
                employes[employe_selectionne].prenom);
        DrawTextEx(font, confirm_msg,
                   (Vector2){px + 20, py + 108},
                   13, 1, COL_MUTED);
    }

    DrawTextEx(font, "Cette action est irreversible.",
               (Vector2){px + 20, py + 128},
               12, 1, COL_DANGER);

    // Annuler button
    if (GuiButton((Rectangle){px + 20, py + 152, 145, 36},
                  "Annuler")) {
        popup_confirm = 0;
    }

    // Confirmer button
    DrawRectangleRounded(
        (Rectangle){px + 185, py + 152, 155, 36},
        0.3f, 8, COL_DANGER);
    if (GuiButton((Rectangle){px + 185, py + 152, 155, 36},
                  "Oui, supprimer")) {
        supprimerEmploye(employes, &nb_employes,
                         employe_selectionne);
        sauvegarderCSV(employes, nb_employes);
        strcpy(success_msg, "Employe supprime !");
        success_timer = 3.0f;
        employe_selectionne = -1;
        ecran_actuel        = ECRAN_LISTE;
        popup_confirm       = 0;
    }

    // Close if click outside
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        !CheckCollisionPointRec(mouse,
         (Rectangle){px, py, pw, ph})) {
        popup_confirm = 0;
    }
}


// ══════════════════════════════════════
// SUCCESS MESSAGE
// ══════════════════════════════════════
if (success_timer > 0.0f) {
    success_timer -= GetFrameTime();

    // Fade out effect
    float alpha = success_timer > 0.5f ? 1.0f : success_timer * 2.0f;
    unsigned char a = (unsigned char)(alpha * 255);

    // Measure text width for centering
    Vector2 size = MeasureTextEx(font, success_msg, 15, 1);
    int sw = (int)size.x + 40;
    int sh = 44;
    int sx = (W - sw) / 2;
    int sy = H - 70;

    // Background
    DrawRectangleRounded(
        (Rectangle){sx, sy, sw, sh},
        0.3f, 8,
        (Color){22, 163, 74, a});

    // Text
    DrawTextEx(font, success_msg,
               (Vector2){sx + 20, sy + 13},
               15, 1,
               (Color){255, 255, 255, a});
}






        EndDrawing();
    }
    UnloadFont(font);

    CloseWindow();
    return 0;
}
