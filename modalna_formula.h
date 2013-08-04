#ifndef MODALNA_FORMULA_H
#define MODALNA_FORMULA_H

#include <iostream>

using namespace std;

struct modalna_formula
{
    int tip = 0; // 0 _|_, 1 prop var, 2 [], 3 ~, 4 ->

    modalna_formula *a = 0, *b = 0;

    char p = 'x';

    void feed(istream& mother)
    {
        char c = ' ';
        while (c == ' ') (c = mother.get());

        if (c == '#')
            return;
        if (c == 'B' || c == '~')
        {
            tip = (c == 'B' ? 2 : 3);
            a = new modalna_formula;
            a->feed(mother);
        }
        else if (c == '>')
        {
            tip = 4;
            a = new modalna_formula; b = new modalna_formula;
            a->feed(mother); b->feed(mother);
        }
        else
        {
            tip = 1;
            p = c;
        }
    }

    bool je_jednaka(modalna_formula *f)
    {
       if (f->tip != tip) return false;

        switch (tip)
        {
        case 0:     break;
        case 1:     if (f->p != p) return false; break;
        case 2:     if (! a->je_jednaka(f->a)) return false; break;
        case 3:     if (! a->je_jednaka(f->a)) return false; break;
        case 4:      if (! a->je_jednaka(f->a)) return false;
                     if (! b->je_jednaka(f->b)) return false; break;
                    break;
        }
        return true;
    }

    bool je_negacija(modalna_formula *f)
    {
        modalna_formula *a = new modalna_formula;
        a->tip = 3;
        a->a = this;
        if (f->je_jednaka(a)) return true;

        a->a = f;
        if (je_jednaka(a)) return true;

        return false;
    }

    modalna_formula *kopija()
    {
        modalna_formula *dvojnik = new modalna_formula;
        dvojnik->tip = tip;
        switch (tip)
        {
        case 0:     break;
        case 1:     dvojnik->p = p; break;
        case 2:     dvojnik->a = a->kopija(); break;
        case 3:     dvojnik->a = a->kopija(); break;
        case 4:     dvojnik->a = a->kopija();
                    dvojnik->b = b->kopija();
                    break;
        }
    }


    ~modalna_formula()
    {
        delete a; delete b;
    }

    friend ostream& operator<<(ostream& out, modalna_formula *f);
};

#endif // MODALNA_FORMULA_H
