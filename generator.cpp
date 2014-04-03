#include "generator.h"
//#define VARIABLE_COUNT 2 /* bot + broj varijabli */

vector<dummy_struct*> kopiraj_skup_op(vector<dummy_struct*> &skup)
{
    vector<dummy_struct*> novi;
    for (int i = 0; i < skup.size(); ++i)
        novi.push_back(skup[i]->cpy());
    return novi;
}

void rek_ispis(dummy_struct* r, string pref, bool l)
{
    cout << pref << (l ? "L: " : "R: ") << r->data1 << endl;
    if (r->l) rek_ispis(r->l, pref + "\t", true);
    if (r->d) rek_ispis(r->d, pref + "\t", false);
}

void pokupi(dummy_struct* root, vector<dummy_struct*> &polje)
{
    polje.push_back(root);
    if (root->l) pokupi(root->l, polje);
    if (root->d) pokupi(root->d, polje);
}

void generiraj_operatore(int tipovaop, int granauk, int trtip,
                         vector<vector<dummy_struct*> >& skupovi, vector<dummy_struct*> &trenutni_skup)
{
    int preostalo_max = granauk - trenutni_skup.size();

    if (tipovaop - 1 > trtip)
        for (int i = 0; i <= preostalo_max; ++i)
        {
            vector<dummy_struct*> nadskup = kopiraj_skup_op(trenutni_skup);
            for (int k = 0; k < i; ++k) nadskup.push_back(new dummy_struct(trtip + 1));

            generiraj_operatore(tipovaop, granauk, trtip + 1, skupovi, nadskup);
            vec_del(nadskup);
        }
    else
    {
        while (trenutni_skup.size() < granauk)
            trenutni_skup.push_back(new dummy_struct(trtip + 1));
        skupovi.push_back(kopiraj_skup_op(trenutni_skup));
        return;
    }
}

void generiraj_strukture(vector<dummy_struct*> ugradeni, vector<dummy_struct*> preostali, vector <dummy_struct*> &strukture)
{
    if (preostali.empty())
    {
        for (auto str : strukture)
        {
            if (str->ekval(ugradeni.front()))
                return;
        }
        strukture.push_back(ugradeni.front()->cpy());
        return;
    }

    for (int i = 0; i < preostali.size(); ++i)
    {
        dummy_struct* koga_uzimam = preostali[i];
        if (ugradeni.empty())
        {
            ugradeni.push_back(koga_uzimam);
            vector<dummy_struct*> novi_preost = kopiraj_skup_op(preostali);
            novi_preost.erase(novi_preost.begin() + i);

            generiraj_strukture(ugradeni, novi_preost, strukture);

            ugradeni = vector<dummy_struct*>();
            continue;
        }

        dummy_struct* head_ugr = ugradeni.front()->cpy();
        vector<dummy_struct*> novi_ugr;
        pokupi(head_ugr, novi_ugr);

        for (int j= 0; j < novi_ugr.size(); ++j)
        {
            dummy_struct* gdje_dajem = novi_ugr[j];
            if (!gdje_dajem->l)
            {
                dummy_struct* head_ugr = novi_ugr.front()->cpy();
                vector<dummy_struct*> novi_ugr2;
                pokupi(head_ugr, novi_ugr2);

                auto stogod = koga_uzimam->cpy();
                novi_ugr2[j]->l = stogod;

                novi_ugr2.push_back(novi_ugr2[j]->l);

                vector<dummy_struct*> temp;
                pokupi(novi_ugr2.front(), temp);
                if (temp.size() - 1 != novi_ugr2.size())
                {
                    pokupi(novi_ugr2.front(), temp);
                }

                vector<dummy_struct*> novi_preost = kopiraj_skup_op(preostali);
                novi_preost.erase(novi_preost.begin() + i);

                generiraj_strukture(novi_ugr2, novi_preost, strukture);
            }
            if (!gdje_dajem->d)
            {
                dummy_struct* head_ugr = novi_ugr.front()->cpy();
                vector<dummy_struct*> novi_ugr2;
                pokupi(head_ugr, novi_ugr2);

                novi_ugr2[j]->d = koga_uzimam->cpy();
                novi_ugr2.push_back(novi_ugr2[j]->d);

                /*if (j >= novi_ugr2.size())
                {
                    koga_uzimam->cpy();
                }*/
                vector<dummy_struct*> temp;
                pokupi(novi_ugr2.front(), temp);
                if (temp.size() - 1 != novi_ugr2.size())
                {
                    pokupi(novi_ugr2.front(), temp);
                }

                vector<dummy_struct*> novi_preost = kopiraj_skup_op(preostali);
                novi_preost.erase(novi_preost.begin() + i);

                generiraj_strukture(novi_ugr2, novi_preost, strukture);
            }
        }
    }

}



wff* bind(dummy_struct* sintakticka_struktura, string razmjestaj_varijabli, int &crawler)
{
    //vector<dummy_struct*> polje;
    //int crawler = 0;
    //pokupi(sintakticka_struktura, polje);

    if (!sintakticka_struktura->l)
        sintakticka_struktura-> data2 = razmjestaj_varijabli[crawler++];
    else bind(sintakticka_struktura->l, razmjestaj_varijabli, crawler);
    if (!sintakticka_struktura->d)
        sintakticka_struktura-> data3 = razmjestaj_varijabli[crawler++];
    else bind(sintakticka_struktura->d, razmjestaj_varijabli, crawler);

    return sintakticka_struktura->u_modalnu();
}

int pow(int a, int b)
{
 int r = a;
 while (--b) r *= a;
 return r;
}

void generiraj_formule(vector<wff*>& konacan_rezultat, int kompleksnost, int skup_operatora, int mdb, int VARIABLE_COUNT)
{
    vector<wff* > rezultat;

    // npr vraca {[->, ->], [->, &&], [&&, ->], [&&, &&]}
    vector<dummy_struct*> trs;
    vector<vector<dummy_struct*> > skupovi_operatora;
    generiraj_operatore(skup_operatora, kompleksnost, 0, skupovi_operatora, trs);

    // npr vraca {[p, p], [p, #], [#, p], [#, #]}
    vector<dummy_struct*> trs_vars;
    vector<vector<dummy_struct*> > skupovi_varijabli;
    generiraj_operatore(/* ovo sad je {_|_, p}*/ VARIABLE_COUNT, kompleksnost + 1, 0, skupovi_varijabli, trs_vars); // kompl + 2 zbog _|_

    // npr. vraca {"pp", "p#", "#p", "##"}
    vector<string> skupovi_varijabli_sredeno;
    string abeceda = ".0pqsrabcdefghijklmnotuvxyzw0123456789";
    for (auto skup : skupovi_varijabli)
    {
        skupovi_varijabli_sredeno.push_back("");
        for (auto opr : skup)
            skupovi_varijabli_sredeno.back() += abeceda[opr->data1];
       // cout << skupovi_varijabli_sredeno.back() << endl;

    }

    // za svaki odabir (ne struktura :D) operatora stvara sva stabla
    // i sva stabla binda sa svakim odabirom varijabli,
    // npr (->)  o [#, p] = (# -> p)
    for (auto struktura_operatora : skupovi_operatora)
    {
        vector<dummy_struct*> ugr;
        vector<dummy_struct*> preost = struktura_operatora;
        vector<dummy_struct*> strukture;
        generiraj_strukture(ugr, preost, strukture);
        for (auto sintakticka_struktura : strukture)
        {
            for (auto odabir_varijabli : skupovi_varijabli_sredeno)
            {
                do {
                    // we will remove bindings that contain p_1, ..., p_n in which
                    // the smallest prefix (i.e. p1 p1 p2 p3 ...) isn't sorted (i.e. p1 p3 p2 ...)
                    char old_max = 0, first_breach_at = 0;
                    set<char> used;
                    for (auto opr : odabir_varijabli)
                    {
                        if (opr == '0') // 0 is irrelevant (0 = falsum due to luka being lazy)
                            continue;
                        used.insert(opr);
                        if (opr < old_max) {
                            if (first_breach_at == 0) first_breach_at=opr;
                        }
                        else
                            old_max = opr;
                    }

                    int crawler = 0;
                    rezultat.push_back(bind(sintakticka_struktura, odabir_varijabli, crawler));

                    if ((first_breach_at != 0 && first_breach_at < old_max)
                        ||
                       (old_max && used.size() + 1 < abeceda.find(old_max)))
                    {
                        rezultat.back()->additional_data = new int(0xff);
                        // rembember bad formula, but will need it for checks
                    }

                } while (next_permutation(odabir_varijabli.begin(), odabir_varijabli.end()));
            }

        }
    }

    // e, sad još za sve treba modalitete pobacati

    for (wff* mf : rezultat)
    {
        vector<wff* > podformulice;

        int maxkod = pow(mdb + 1, kompleksnost + kompleksnost + 1);

        for (int i = 0; i < maxkod; ++i)
        {
            int kod = i;
            /*cout << kod % (mdb + 1) << " "
                 << (kod / (mdb + 1)) % (mdb + 1) << " "
                 << ((kod / (mdb + 1)) / (mdb + 1) )% (mdb + 1) << endl;*/
            wff* verzija = mf->deep_copy();
            podformulice = vector<wff* >();
            verzija->collect_subwffs(podformulice);
            for (int j = 0; j < podformulice.size(); ++j)
            {
                auto f = podformulice[j];
                if (f->primitive()) continue; // because op's do the work

                int kolko = kod % (mdb + 1);
                kod /= (mdb + 1);
                //if (f->b->tip == wff::box || f->a->tip == wff::box)
                //    cout << " [something bad happened!] ";
                while (kolko > 0)
                {

                    wff* stara = f->a;
                    f->a = new wff;
                    f->a->type = wff::box;
                    f->a->a = stara;
                    --kolko;
                }

                kolko = kod % (mdb + 1);
                kod /= (mdb + 1);
                while (kolko > 0)
                {

                    wff* stara = f->b;
                    f->b = new wff;
                    f->b->type = wff::box;
                    f->b->a = stara;
                    --kolko;
                }
            }
            int kolko = kod % (mdb + 1);
            while (kolko > 0)
            {
                //if (kolko > 2)
                //    cout << "break";
                wff* nova = new wff;
                nova->type = wff::box;
                nova->a = verzija;
                verzija = nova;
                --kolko;
            }

            bool dalje = true;
            //cout << verzija << endl;
            vector<wff* > brzobrzo;
            verzija->collect_subwffs(brzobrzo);
            for (wff *pf : podformulice) // brzi prune
            {
                if (pf->type == wff::cond)
                {
                    if (pf->a->type == wff::cond && pf->b->type == wff::falsum)
                        if (pf->a->b->type == wff::falsum)
                            dalje = false;
                    if (pf->a->type == wff::falsum && pf->b->type != wff::falsum)
                        dalje = false;
                    if (pf->a->syntactically_equals(pf->b) &&
                        !(pf->a->type == wff::falsum && pf->b->type == wff::falsum))
                        dalje = false;
                    if (!dalje) break;
                }
                else if (pf->binary())
                {
                    if (pf->a->type == wff::falsum || pf->b->type == wff::falsum)
                        dalje = false;
                    if (dalje && pf->a->commutes_syntactically_equals(pf->b))
                        dalje = false;
                    if (dalje && pf->a->type == wff::prop && pf->b->type != wff::prop)
                        dalje = false;
                    if (dalje) // komutativni operator s vecom desnom stranom
                    {
                        vector<wff *> left, right;
                        pf->a->collect_subwffs(left);
                        pf->b->collect_subwffs(right);
                        if (left.size() < right.size())
                            dalje = false;
                    }
                    if (!dalje) break;
                }
                if (pf->type == wff::bicond && dalje)
                {
                    if (pf->a->type == wff::bicond) // (b<->a)<->a = a = (a<->b)<->a
                    {
                        if (pf->a->a->commutes_syntactically_equals(pf->b))
                            dalje = false;
                        if (dalje &&
                                pf->a->b->commutes_syntactically_equals(pf->b))
                            dalje = false;
                    }
                    else if (pf->a->type == wff::cond) // (b->a)<->a = (b->#)->a;
                    {
                        if (pf->a->b->commutes_syntactically_equals(pf->b))
                            dalje = false;
                    }
                }
                else if (pf->type == wff::cond && dalje) // a->(a->b), a->(a<->b), a->(b<->a), a->(b->a)
                {
                    if (pf->b->standard())
                    {
                        if (pf->b->a->commutes_syntactically_equals(pf->a))
                            dalje = false;
                        if (dalje &&
                                pf->b->b->commutes_syntactically_equals(pf->a))
                            dalje = false;
                    }
                }
                if (dalje && pf->binary()) // T -> x, T <-> x, ali za <-> sredeno kroz ()
                {
                    if (pf->a->type == wff::cond && pf->b->type == wff::cond)
                        if (pf->a->b->type == wff::falsum && pf->b->type == wff::cond
                                && pf->b->b->type == wff::falsum)
                            dalje = false; // (->#)</->(->#)

                    if (dalje && pf->a->standard())
                        if (pf->a->a->type == pf->a->b->type == wff::falsum)
                            dalje = false;
                    if (dalje && pf->b->standard())
                        if (pf->b->a->type == pf->b->b->type == wff::falsum)
                            dalje = false;
                }
            }


            if (dalje)
                konacan_rezultat.push_back(verzija);
        }
        //cout << "gotovo";
    }

    vec_del(rezultat);
}

















