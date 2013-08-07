#ifndef STABLO_H
#define STABLO_H

#include <vector>
#include <iostream>
#include "modalna_formula.h"
using namespace std;

struct stablo // svako stabalce, svoja grana je!
{
    stablo* mother = 0;
    // BEGIN GL
    int razina = 0; // koliko duboko u stablu s obzirom na modalne svjetove
    // END GL
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
            novo->razina = razina;
            novo = new stablo;
            novo->mother = this;
            novo->formule_grane.push_back(d->kopija());
            stabla_grane.push_back(novo);
            novo->razina = razina;
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

    // GL BEGIN
    bool kontrad_na_gore(modalna_formula *f, int raz)
    {
        if (!formule_grane.empty())
        for (auto gr : formule_grane)
        {
            if (f->je_negacija(gr))
                return true;
        }
        if (mother != 0 && mother->razina == raz)
            return mother->kontrad_na_gore(f, raz);
        return false;
    }

    void skupi_modalne(vector<modalna_formula* > &kolekcija, int raz)
    {
        if (!formule_grane.empty())
        for (auto gr : formule_grane)
            if (gr->tip == 2)
                kolekcija.push_back(gr);
        if (mother != 0)
            if (mother->razina == raz)
                mother->skupi_modalne(kolekcija, raz);
    }
    // GL END



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
            case 2:     zatvorena = zatvorena ||kontrad_na_gore(f/*GL BEGIN*/, razina /*GL END*/);

                break; // zasad ne postoji pravilo rastavljanja -> a nece ni postojat LOLolo
            case 3:     zatvorena = zatvorena ||kontrad_na_gore(f);
                        if (!zatvorena)
                        {
                            // GL BEGIN
                            if (f->a->tip == 2)
                            {
                                // zasad nista, na kraju rjesavanja cemo provjeriti ovaj tip
                            }
                            // GL END
                            else if (f->a->tip == 3)
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

    // GL BEGIN
    bool otvori_prozor(modalna_formula* mf, vector<modalna_formula*> &kolekcija)
    {

        bool sve_zat = true;
        if (!stabla_grane.empty())
        {
            for (auto s : stabla_grane)
            {
                if (!zatvorena && s->razina == razina)
                    sve_zat = sve_zat && s->otvori_prozor(mf, kolekcija);
            }
            return sve_zat;
        }

        cout << "otvaram za " << mf << endl;
        stablo * novo = new stablo;
        modalna_formula * negacija = new modalna_formula;
        negacija->tip = 3;
        negacija->a = mf->a->a;
        novo->dodaj_formulu(negacija);
        novo->dodaj_formulu(mf->a);
        if (!kolekcija.empty())
        for (modalna_formula* f: kolekcija)
        {
            novo->dodaj_formulu(f);
            novo->dodaj_formulu(f->a);
        }
        novo->razina = razina + 1;
        novo->mother = this;
        stabla_grane.push_back(novo);
        novo->rijesi_svoje();
        return novo->zatvorena;
    }

    // GL END

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

        // GL BEGIN // TODO: prebaciti izvršavanje u djecu, jer može biti da imamo otvorene grane u djeci
                    //       i ktome da u svakoj grani posebno postoji protuslovlje, ali ne u meni [mozda i nije al za svaki sl]
        if (!zatvorena)
        {
            vector<modalna_formula* > negacije_modalnih;
            for (auto i = formule_grane.begin(); i != formule_grane.end(); ++i)
                if ((*i)->tip == 3)
                    if ((*i)->a->tip == 2)
                        negacije_modalnih.push_back(*i);

            vector<modalna_formula* > kolekcija;
            skupi_modalne(kolekcija, razina);

            if (negacije_modalnih.empty()) return;

            int index = stabla_grane.size();

            for (auto *mf : negacije_modalnih)
            {
                otvori_prozor(mf, kolekcija);
            }

            svezat = true;
            for (int i = index; i < stabla_grane.size(); ++i)
            {
                if (stabla_grane[i]->zatvorena == false) svezat = false;
            }
            zatvorena = zatvorena || svezat;
        }
        // GL END
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
