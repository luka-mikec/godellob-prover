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

        for (int j= 0; j < ugradeni.size(); ++j)
        {
            dummy_struct* gdje_dajem = ugradeni[j];

            if (!gdje_dajem->l)
            {
                dummy_struct* head_ugr = ugradeni.front()->cpy();
                vector<dummy_struct*> novi_ugr;
                pokupi(head_ugr, novi_ugr);
                novi_ugr[j]->l = koga_uzimam->cpy();
                novi_ugr.push_back(novi_ugr[j]->l);

                vector<dummy_struct*> novi_preost = kopiraj_skup_op(preostali);
                novi_preost.erase(novi_preost.begin() + i);

                generiraj_strukture(novi_ugr, novi_preost, strukture);
            }
            if (!gdje_dajem->d)
            {
                dummy_struct* head_ugr = ugradeni.front()->cpy();
                vector<dummy_struct*> novi_ugr;
                pokupi(head_ugr, novi_ugr);
                novi_ugr[j]->d = koga_uzimam->cpy();
                novi_ugr.push_back(novi_ugr[j]->d);

                vector<dummy_struct*> novi_preost = kopiraj_skup_op(preostali);
                novi_preost.erase(novi_preost.begin() + i);

                generiraj_strukture(novi_ugr, novi_preost, strukture);
            }
        }
    }

}




















