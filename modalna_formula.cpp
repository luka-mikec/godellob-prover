#include "modalna_formula.h"

int modalna_formula::instancirano, modalna_formula::obrisano;
vector<modalna_formula*> modalna_formula::adresice;

ostream& operator<<(ostream& out, modalna_formula *f)
{
    switch (f->tip)
    {
    case 0:     out << "⊥"; break;
    case 1:     out << f->p; break;
    case 2:     out << "☐"; if (f->a) if (f->a->tip <= 3) out << f->a;
                                    else out << "(" << f->a << ")";
                             else   out << "null"; break;
    case 3:     out << "¬"; if (f->a)   if (f->a->tip <= 3) out << f->a;
                                     else out << "(" << f->a << ")";
                            else   out << "null"; break;
    case 4:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << " → ";
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    case 5:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << " ∧ ";
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;

    case 6:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << " ∨ ";
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    case 7:     if (f->a)    if (f->a->tip <= 3) out << f->a;
                          else out << "(" << f->a << ")";
                          out << " ↔ ";
                if (! f->a)   out << "null";
                if (f->b)  if (f->b->tip <= 3) out << f->b;
                            else out << "(" << f->b << ")";
                if (! f->b)   out << "null"; break;
    }

    return out;
}
