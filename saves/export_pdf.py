import sys
from fpdf import FPDF

# ─── Get arguments ────────────────────────────
if len(sys.argv) < 10:
    print("Usage: export_pdf.py nom prenom poste base hsup prime cnss ir net")
    sys.exit(1)

nom    = sys.argv[1]
prenom = sys.argv[2]
poste  = sys.argv[3]
base   = float(sys.argv[4])
hsup   = float(sys.argv[5])
prime  = float(sys.argv[6])
cnss   = float(sys.argv[7])
ir     = float(sys.argv[8])
net    = float(sys.argv[9])

brut   = base + hsup + prime

# ─── Colors ───────────────────────────────────
NAVY   = (26,  43,  74)
ACCENT = (37,  99,  235)
GREEN  = (22,  163, 74)
RED    = (220, 38,  38)
LIGHT  = (234, 242, 255)
WHITE  = (255, 255, 255)
GRAY   = (100, 116, 139)

# ─── PDF Setup ────────────────────────────────
pdf = FPDF()
pdf.add_page()
pdf.set_auto_page_break(auto=True, margin=15)

# ─── Header ───────────────────────────────────
pdf.set_fill_color(*NAVY)
pdf.rect(0, 0, 210, 35, 'F')

pdf.set_font("Helvetica", "B", 22)
pdf.set_text_color(*WHITE)
pdf.set_xy(10, 8)
pdf.cell(0, 12, "EasySalaire", ln=False)

pdf.set_font("Helvetica", "", 11)
pdf.set_xy(10, 22)
pdf.cell(0, 8, "Bulletin de Paie", ln=False)

# ─── Info card ────────────────────────────────
pdf.set_fill_color(*LIGHT)
pdf.rect(10, 42, 190, 32, 'F')

pdf.set_font("Helvetica", "B", 11)
pdf.set_text_color(*ACCENT)
pdf.set_xy(14, 44)
pdf.cell(0, 7, "Informations personnelles")

pdf.set_font("Helvetica", "", 10)
pdf.set_text_color(30, 41, 59)

pdf.set_xy(14, 52)
pdf.cell(30, 6, "Nom :")
pdf.set_font("Helvetica", "B", 10)
pdf.cell(60, 6, nom)

pdf.set_font("Helvetica", "", 10)
pdf.cell(25, 6, "Prenom :")
pdf.set_font("Helvetica", "B", 10)
pdf.cell(60, 6, prenom)

pdf.set_font("Helvetica", "", 10)
pdf.set_xy(14, 60)
pdf.cell(30, 6, "Poste :")
pdf.set_font("Helvetica", "B", 10)
pdf.cell(0, 6, poste)

# ─── Salary card ──────────────────────────────
pdf.set_fill_color(255, 255, 255)
pdf.rect(10, 82, 190, 70, 'F')
pdf.set_draw_color(226, 232, 240)
pdf.rect(10, 82, 190, 70)

pdf.set_font("Helvetica", "B", 11)
pdf.set_text_color(*ACCENT)
pdf.set_xy(14, 85)
pdf.cell(0, 7, "Calcul du salaire")

def row(pdf, label, value, color, bold=False):
    pdf.set_text_color(*GRAY)
    pdf.set_font("Helvetica", "", 10)
    pdf.cell(80, 7, label)
    pdf.set_text_color(*color)
    pdf.set_font("Helvetica", "B" if bold else "", 10)
    pdf.cell(0, 7, value, ln=True)
    pdf.set_x(14)

pdf.set_xy(14, 94)
row(pdf, "Salaire base :",
    f"{base:.2f} TND", (30, 41, 59))
row(pdf, "Heures supplementaires :",
    f"+ {hsup:.2f} TND", GREEN)
row(pdf, "Prime :",
    f"+ {prime:.2f} TND", GREEN)

# Separator
pdf.set_draw_color(*GRAY)
pdf.line(14, pdf.get_y() + 1, 196, pdf.get_y() + 1)
pdf.ln(3)

row(pdf, "Salaire brut :",
    f"{brut:.2f} TND", (30, 41, 59), bold=True)
row(pdf, "CNSS (9.18%) :",
    f"- {cnss:.2f} TND", RED)
row(pdf, "IR :",
    f"- {ir:.2f} TND", RED)

# ─── Net salary ───────────────────────────────
y = pdf.get_y() + 6
pdf.set_fill_color(220, 252, 231)
pdf.rect(10, y, 190, 18, 'F')
pdf.set_draw_color(134, 239, 172)
pdf.rect(10, y, 190, 18)

pdf.set_xy(14, y + 4)
pdf.set_font("Helvetica", "B", 13)
pdf.set_text_color(30, 41, 59)
pdf.cell(80, 10, "SALAIRE NET :")
pdf.set_text_color(*GREEN)
pdf.cell(0, 10, f"{net:.2f} TND")

# ─── Footer ───────────────────────────────────
pdf.set_fill_color(*NAVY)
pdf.rect(0, 272, 210, 25, 'F')
pdf.set_font("Helvetica", "", 9)
pdf.set_text_color(*WHITE)
pdf.set_xy(10, 278)
pdf.cell(0, 7, "EasySalaire — Systeme de gestion de paie | 2025/2026",
         align="C")

# ─── Save ─────────────────────────────────────
filename = f"C:\\EasySalaire\\saves\\{nom}_{prenom}_fiche.pdf"
pdf.output(filename)
print(f"PDF saved: {filename}")