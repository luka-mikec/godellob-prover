#include "modalna_formula.h"

extern int fancy = 0;

vector<map<string, string> > styles;

uintmax_t modalna_formula::instancirano, modalna_formula::obrisano;
vector<modalna_formula*> modalna_formula::adresice;

bool _init_styles = false;

void init_styles()
{
    if (!_init_styles)
        _init_styles = true;

    map<string, string> s1, s2, s3;
    s1["#"] = "#"; s1["B"] = "B"; s1["~"] = "~";
    s1[">"] = ">"; s1["&"] = "&"; s1["|"] = "|"; s1["="] = "=";

    s2["#"] = "_|_"; s2["B"] = "[]"; s2["~"] = "~";
    s2[">"] = "->"; s2["&"] = "&"; s2["|"] = "||"; s2["="] = "<->";

    s3["#"] = "\u22A5"; s3["B"] = "\u25A1"; s3["~"] = "\u00AC";
    s3[">"] = "\u2192"; s3["&"] = "\u2228"; s3["|"] = "\u2227"; s3["="] = "\u2194";

    styles.push_back(s1);
    styles.push_back(s2);
    styles.push_back(s3);

    for (map<string, string>& s : styles)
        for (auto& par : s) if (par.first != "#" && par.first != "B" && par.first != "~")
            par.second = " " + par.second + " ";
}

ostream& operator<<(ostream& out, modalna_formula *f)
{    
    if (!_init_styles)
        init_styles();

    switch (f->tip)
    {
    case 0:     out << styles[fancy]["#"]; break;
    case 1:     out << f->p; break;
    case 2:     out << styles[fancy]["B"]; if (f->a) if (f->a->tip <= 3) out << f->a;
                                    else out << "(" << f->a << ")";
                             else   out << "null"; break;
    case 3:     out << styles[fancy]["~"]; if (f->a)   if (f->a->tip <= 3) out << f->a;
                                     else out << "(" << f->a << ")";
                            else   out << "null"; break;
    case 4:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << styles[fancy][">"];
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    case 5:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << styles[fancy]["&"];
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;

    case 6:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << styles[fancy]["|"];
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    case 7:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << styles[fancy]["="];
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    }

    return out;
}
