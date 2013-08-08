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
     /*   if (f->tip == 2)
            if (f->a->tip == 1)
            {
                cout << "breakme";
            }*/
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
            // da smanjimo cijelo stablo, odmah provjera je li zatvoreno
            novo->zatvorena = kontrad_na_gore(l, razina);
            novo = new stablo;
            novo->mother = this;
            novo->formule_grane.push_back(d->kopija());
            stabla_grane.push_back(novo);
            novo->razina = razina;
            novo->zatvorena = kontrad_na_gore(d, razina);
        }
    }


    // GL BEGIN
    bool kontrad_na_gore(modalna_formula *f, int raz /*= 0*/)
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

    void skupi_modalne(vector<modalna_formula* > &kolekcija, int raz, bool gore = true)
    {
        if (!formule_grane.empty())
        for (auto gr : formule_grane)
            if (gr->tip == 2)
            {
                bool sadrzi = false;
                if (kolekcija.size())
                for (auto ff : kolekcija)
                {
                    if (gr->je_jednaka(ff))
                    {
                        sadrzi = true; break;
                    }
                }
                if (!sadrzi)
                    kolekcija.push_back(gr);
            }
        if (mother != 0 && gore)
            if (mother->razina == raz) // ne bi bila greska, ali da ne bloata nepotrebno
                mother->skupi_modalne(kolekcija, raz);
    }
    // GL END



    void rijesi_formulu(modalna_formula *f)
    {
        // formulu kod sebe sredimo samo ako nema djece
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
            case 1:     zatvorena = zatvorena ||kontrad_na_gore(f, razina); break; // ovo zatvorena || tu i ispod bi trebalo biti nepotrebno
            case 2:     zatvorena = zatvorena ||kontrad_na_gore(f/*GL BEGIN*/, razina /*GL END*/);

                break; // zasad ne postoji pravilo rastavljanja -> a nece ni postojat LOLolo
            case 3:     zatvorena = zatvorena ||kontrad_na_gore(f, razina);
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
                                delete b;
                            }
                        }
                        break;

            case 4:     zatvorena = zatvorena || kontrad_na_gore(f, razina);
                        if (!zatvorena)
                        {
                            modalna_formula *a = new modalna_formula, *b = f->b->kopija();
                            a->tip = 3;
                            a->a = f->a->kopija();

                            dodaj_stablo(a, b);
                            delete a;
                        }
                        break;
            }

        }
    }

    vector<modalna_formula*> kopiraj_skup_op(vector<modalna_formula*> &skup)
    {
        vector<modalna_formula*> novi;
        for (int i = 0; i < skup.size(); ++i)
            novi.push_back(skup[i]->kopija());
        return novi;
    }

    template <class T>
    void vec_del(vector<T>v )
    {
        for (typename vector<T>::iterator i = v.begin(); i != v.end(); ++i)
            delete *i;
    }

    // GL BEGIN
    bool otvori_prozor(modalna_formula* mf, vector<modalna_formula*> &kolekcija)
    {
        /*if (formule_grane.size())
                if (formule_grane.front()->p == 'p')
        {
            cout << "aaaa";
          string breakmebabe;

        }*/

        bool sve_zat = true;
        if (!stabla_grane.empty())
        {
            for (auto s : stabla_grane)
            {
                if (!zatvorena && !s->zatvorena && s->razina == razina)
                {
                    auto kop = kopiraj_skup_op(kolekcija);
                    s->skupi_modalne(kop, razina, false);
                    bool val = s->otvori_prozor(mf, kop);
                    sve_zat = sve_zat && val;
                    //vec_del(kop);
                }
            }
            return sve_zat;
        }

        //cout << "otvaram za " << mf << endl;
        stablo * novo = new stablo;
        modalna_formula * negacija = new modalna_formula;
        negacija->tip = 3;
        negacija->a = mf->a->a;
        novo->dodaj_formulu(negacija);
        novo->dodaj_formulu(mf->a);
        negacija->a = 0; delete negacija;

        if (!kolekcija.empty())
        for (modalna_formula* f: kolekcija)
        {
            bool prva = false, druga = false;
            for (auto tr : novo->formule_grane)
            {
                if (tr->je_jednaka(f))
                    prva = true;
                if (tr->je_jednaka(f->a))
                    druga = true;
            }
            if (!prva) novo->dodaj_formulu(f);
            if (!druga) novo->dodaj_formulu(f->a);
        }
        novo->razina = razina + 1;
        novo->mother = this;
        stabla_grane.push_back(novo);
        novo->rijesi_svoje();
        return novo->zatvorena;
    }

    // GL END

    bool jesu_li_mi_deca_sva_takva_da_su_zatvorena()
    {
        bool svezat = !stabla_grane.empty();
        bool force_zatv = false;
        for (int i = 0; i < stabla_grane.size(); ++i)
        {
            if (!stabla_grane[i]->zatvorena)
                    //|| stabla_grane[i]->razina != razina // eksperimentalno
               if (!stabla_grane[i]->jesu_li_mi_deca_sva_takva_da_su_zatvorena())
                    svezat = false;
            if (stabla_grane[i]->zatvorena && stabla_grane[i]->razina != razina)
                force_zatv = true;
        }
        return zatvorena || svezat || force_zatv;
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
            /*if (s->formule_grane.front()->tip == 3)
                if (s->formule_grane.front()->a->tip == 3)
                    if (s->formule_grane.front()->a->a->tip == 0)
                        cout << "breakmeup";*/
            if (!zatvorena)
                s->rijesi_svoje();
            else
                cout << "%";
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
        if (!zatvorena) // los pristup: trebala bi posebna f.
                        // koja bi rekurzivno provjeravala. bloated loli
        {
            vector<modalna_formula* > negacije_modalnih;
            for (auto i = formule_grane.begin(); i != formule_grane.end(); ++i)
                if ((*i)->tip == 3)
                    if ((*i)->a->tip == 2)
                        negacije_modalnih.push_back(*i);

            vector<modalna_formula* > kolekcija;
            skupi_modalne(kolekcija, razina);

            //int index = stabla_grane.size();
            if (!negacije_modalnih.empty())
            for (auto *mf : negacije_modalnih)
            {
                otvori_prozor(mf, kolekcija);
            }


            //zatvorena = zatvorena || svezat || force_zatv;
            zatvorena = zatvorena || jesu_li_mi_deca_sva_takva_da_su_zatvorena();
            if (!zatvorena)
            {
                //cout << "BREAKMEUP";
            }
        }
        // GL END
    }


    void izgradi_za(modalna_formula *f)
    {
        formule_grane.clear();
        dodaj_formulu(f);
        rijesi_svoje();
    }

    ~stablo()
    {
        for (auto mf : formule_grane)
            delete mf;
        for (auto mf : stabla_grane)
            delete mf;
    }

    friend ostream& operator<<(ostream& out, const stablo &s);
};

#endif // STABLO_H
