#include "modalna_formula.h"


ostream& operator<<(ostream& out, modalna_formula *f)
{
    switch (f->tip)
    {
    case 0:     out << "#"; break;
    case 1:     out << f->p; break;
    case 2:     out << "[]"; if (f->a->tip <= 3) out << f->a;
                             else out << "(" << f->a << ")"; break;
    case 3:     out << "~";  if (f->a->tip <= 3) out << f->a;
                             else out << "(" << f->a << ")"; break;
    case 4:     if (f->a->tip <= 3) out << f->a;
                else out << "(" << f->a << ")";
                out << " -> ";
                if (f->b->tip <= 3) out << f->b;
                else out << "(" << f->b << ")";
                break;
    case 5:     if (f->a->tip <= 3) out << f->a;
                else out << "(" << f->a << ")";
                out << " && ";
                if (f->b->tip <= 3) out << f->b;
                else out << "(" << f->b << ")";
                break;
    case 6:     if (f->a->tip <= 3) out << f->a;
                else out << "(" << f->a << ")";
                out << " || ";
                if (f->b->tip <= 3) out << f->b;
                else out << "(" << f->b << ")";
                break;
    case 7:     if (f->a->tip <= 3) out << f->a;
                else out << "(" << f->a << ")";
                out << " <-> ";
                if (f->b->tip <= 3) out << f->b;
                else out << "(" << f->b << ")";
                break;
    }
}
