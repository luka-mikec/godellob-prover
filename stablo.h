#ifndef STABLO_H
#define STABLO_H

#include <vector>
#include <iostream>
#include "modalna_formula.h"
using namespace std;

struct stablo // svako stabalce, svoja grana je!
{
    stablo* mother = 0;
    vector<modalna_formula*> formule_grane;
    vector<stablo*> stabla_grane;

    bool zatvorena = false;

    void dodaj_formulu(modalna_formula *f)
    {
        if (stabla_grane.size() > 0)
        {
            for (auto gr : stabla_grane)
            {
                gr->dodaj_formulu(f);
            }
        }
        else formule_grane.push_back(f->kopija());
    }

    void dodaj_stablo(modalna_formula *l, modalna_formula *d)
    {
        if (stabla_grane.size() > 0)
        {
            for (auto gr : stabla_grane)
            {
                gr->dodaj_stablo(l, d);
            }
        }
        else
        {
            stablo* novo = new stablo;
            novo->mother = this;
            novo->formule_grane.push_back(l->kopija());
            stabla_grane.push_back(novo);
            novo = new stablo;
            novo->mother = this;
            novo->formule_grane.push_back(d->kopija());
            stabla_grane.push_back(novo);
        }
    }

    bool kontrad_na_gore(modalna_formula *f)
    {
        if (!formule_grane.empty())
        for (auto gr : formule_grane)
        {
            if (f->je_negacija(gr))
                return true;
        }
        if (mother != 0)
            return mother->kontrad_na_gore(f);
        return false;
    }

    void rijesi_formulu(modalna_formula *f)
    {
        // formulu kod sebe rijesavamo samo ako nema djece
        if (stabla_grane.size() > 0)
        {
            for (auto gr : stabla_grane)
            {
                if (!gr->zatvorena)
                gr->rijesi_formulu(f);
            }
        }
        else
        {
            switch (f->tip)
            {
            case 0:     zatvorena = true; break;
            case 1:     zatvorena = zatvorena ||kontrad_na_gore(f); break; // ovo zatvorena || tu i ispod bi trebalo biti nepotrebno
            case 2:     zatvorena = zatvorena ||kontrad_na_gore(f); break; // zasad ne postoji pravilo rastavljanja
            case 3:     zatvorena = zatvorena ||kontrad_na_gore(f);
                        if (!zatvorena)
                        {
                            if (f->a->tip == 3)
                            {
                                dodaj_formulu(f->a->a);
                            }
                            else if (f->a->tip == 4)
                            {
                                dodaj_formulu(f->a->a);
                                modalna_formula *b = new modalna_formula;
                                b->tip = 3;
                                b->a = f->a->b->kopija();
                                dodaj_formulu(b);
                            }
                        }
                        break;

            case 4:     zatvorena = zatvorena || kontrad_na_gore(f);
                        if (!zatvorena)
                        {
                            modalna_formula *a = new modalna_formula, *b = f->b->kopija();
                            a->tip = 3;
                            a->a = f->a->kopija();

                            dodaj_stablo(a, b);
                        }
                        break;
            }

        }
    }

    void rijesi_svoje()
    {
        for (int i = 0; i < formule_grane.size(); ++i)
        {
            if (!zatvorena)
                rijesi_formulu( formule_grane[i]);
        }

        if (!stabla_grane.empty())
        for (auto s : stabla_grane)
        {
            if (!zatvorena)
                s->rijesi_svoje();
        }

        bool svezat = !stabla_grane.empty();
        if (!stabla_grane.empty())
        for (auto s : stabla_grane)
        {
            if (s->zatvorena == (0 == 1))
            {
                svezat = false;
                break;
            }
        }
        zatvorena = zatvorena || svezat;
    }


    void izgradi_za(modalna_formula *f)
    {
        formule_grane.clear();
        dodaj_formulu(f);
        rijesi_svoje();
    }

    friend ostream& operator<<(ostream& out, const stablo &s);
};

#endif // STABLO_H
