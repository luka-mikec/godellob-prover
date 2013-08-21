#include "generator.h"


vector<dummy_struct*> kopiraj_skup_op(vector<dummy_struct*> &skup)
{
    vector<dummy_struct*> novi;
    for (int i = 0; i < skup.size(); ++i)
        novi.push_back(skup[i]->cpy());
    return novi;
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
            vec_del(trenutni_skup);
        }
    else
    {
        while (trenutni_skup.size() < granauk)
            trenutni_skup.push_back(new dummy_struct(trtip + 1));
        skupovi.push_back(trenutni_skup);
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



wff* bind(dummy_struct* sintakticka_struktura, string razmjestaj_varijabli)
{
    vector<dummy_struct*> polje;
    int crawler = 0;
    pokupi(sintakticka_struktura, polje);
    for (auto& str : polje)
    {
        if (!str->l) str-> data2 = razmjestaj_varijabli[crawler++];
        if (!str->d) str-> data3 = razmjestaj_varijabli[crawler++];
    }
    return sintakticka_struktura->u_modalnu();
}

int pow(int a, int b)
{
 int r = a;
 while (--b) r *= a;
 return r;
}

vector<wff*> generiraj_formule(int kompleksnost, int skup_operatora)
{
    vector<wff*> rezultat;

    vector<dummy_struct*> trs;
    vector<vector<dummy_struct*> > skupovi_operatora;
    generiraj_operatore(skup_operatora, kompleksnost, 0, skupovi_operatora, trs);



    vector<dummy_struct*> trs_vars;
    vector<vector<dummy_struct*> > skupovi_varijabli;
    generiraj_operatore(/*max kompleksnost + 2 // ovo sad je {_|_, p}*/ 2, kompleksnost + 1, 0, skupovi_varijabli, trs_vars); // kompl + 2 zbog _|_
    vector<string> skupovi_varijabli_sredeno;
    string abeceda = ".0pqsrabcdefghijklmnotuvxyzw0123456789";
    for (auto skup : skupovi_varijabli)
    {
        skupovi_varijabli_sredeno.push_back("");
        for (auto opr : skup)
            skupovi_varijabli_sredeno.back() += abeceda[opr->data1];
    }

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
                    rezultat.push_back(bind(sintakticka_struktura, odabir_varijabli));
                } while (next_permutation(odabir_varijabli.begin(), odabir_varijabli.end()));
            }

        }
    }

    int mdb = 2;
    // e, sad još za sve treba modalitete pobacati
    vector<wff* > konacan_rezultat;
    for (wff* mf : rezultat)
    {
        vector<wff* > podformulice;
        mf->collect_subwffs(podformulice);


        int n = podformulice.size();
        int maxkod = pow(mdb + 1, kompleksnost + kompleksnost + 1);
	//int maxkod = 1; cout << "GRESKA GRESKA";
        for (int i = 0; i < maxkod; ++i)
        {
            int kod = i;
            /*cout << kod % (mdb + 1) << " "
                 << (kod / (mdb + 1)) % (mdb + 1) << " "
                 << ((kod / (mdb + 1)) / (mdb + 1) )% (mdb + 1) << endl;*/
            wff* verzija = mf->deep_copy();
            podformulice = vector<wff* >();
            verzija->collect_subwffs(podformulice);
            for (int j = 0; j < n; ++j)
            {
                auto f = podformulice[j];
                if (f->primitive()) continue;

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
            for (auto *pf : podformulice) // brzi prune
                if (pf->type == wff::cond)
                {
                    if (pf->a->type ==wff::falsum)
                        dalje = false;
                    if (pf->a->syntactically_equals(pf->b))
                        dalje = false;
                    if (!dalje) break;
                }

            if (dalje)
                konacan_rezultat.push_back(verzija);
        }
        //cout << "gotovo";
    }

    vec_del(rezultat);
    return konacan_rezultat;

    // konacno, gradimo formule: za svaku strukturu, svaka perm. varijabli





}

















