#include "generator.h"

template <class T>
void vec_del(vector<T>v )
{
    for (typename vector<T>::iterator i = v.begin(); i != v.end(); ++i)
        delete *i;
}

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

void pokupi(modalna_formula* root, vector<modalna_formula*> &polje)
{
    polje.push_back(root);
    if (root->a) pokupi(root->a, polje);
    if (root->b) pokupi(root->b, polje);
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



modalna_formula* bind(dummy_struct* sintakticka_struktura, string razmjestaj_varijabli)
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

vector<modalna_formula*> generiraj_formule(int kompleksnost, int skup_operatora)
{
    vector<modalna_formula*> rezultat;

    vector<dummy_struct*> trs;
    vector<vector<dummy_struct*>> skupovi_operatora;
    generiraj_operatore(skup_operatora, kompleksnost, 0, skupovi_operatora, trs);

    vector<dummy_struct*> trs_vars;
    vector<vector<dummy_struct*>> skupovi_varijabli;
    generiraj_operatore(/*kompleksnost + // ovo sad je {_|_, p}*/ 2, kompleksnost + 1, 0, skupovi_varijabli, trs_vars); // kompl + 2 zbog _|_
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
    vector<modalna_formula* > konacan_rezultat;
    for (modalna_formula* mf : rezultat)
    {
        vector<modalna_formula* > podformulice;
        pokupi(mf, podformulice);

        int n = podformulice.size();
        int maxkod = int(pow((double)mdb + 1., (double)kompleksnost + kompleksnost + 1)+0.1);
        for (int i = 0; i < maxkod; ++i)
        {
            int kod = i;
            modalna_formula* verzija = mf->kopija();
            for (int j = 1; j <= n; ++j)
            {
                if (podformulice[j - 1]->tip <= 1) continue;

                int kolko = kod % (mdb + 1);
                kod /= (mdb + 1);
                while (kolko > 0)
                {
                    modalna_formula* stara = verzija->a;
                    verzija->a = new modalna_formula;
                    verzija->a->tip = 2;
                    verzija->a->a = stara;
                    --kolko;
                }

                kolko = kod % (mdb + 1);
                kod /= (mdb + 1);
                while (kolko > 0)
                {
                    modalna_formula* stara = verzija->b;
                    verzija->b = new modalna_formula;
                    verzija->b->tip = 2;
                    verzija->b->a = stara;
                    --kolko;
                }
            }
            int kolko = kod % (mdb + 1);
            while (kolko > 0)
            {
                modalna_formula* nova = new modalna_formula;
                nova->tip = 2;
                nova->a = verzija;
                verzija = nova;
                --kolko;
            }
            konacan_rezultat.push_back(verzija);
        }
    }

    return konacan_rezultat;

    // konacno, gradimo formule: za svaku strukturu, svaka perm. varijabli





}

















