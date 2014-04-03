#ifndef GENERATOR_H
#define GENERATOR_H

#include "wff.h"
using namespace std;

struct dummy_struct
{
    dummy_struct *l = 0, *d = 0;
    int data1 = 0;
    char data2 = 'x';
    char data3 = 'x';
    dummy_struct() {}
    dummy_struct(int data) {data1 = data;}
    dummy_struct* cpy() { dummy_struct *n = new dummy_struct;
                          n->l = (l ? l->cpy() : 0);  n->d = (d ? d->cpy() : 0);
                          n->data1 = data1; n->data2 = data2; n->data3 = data3;
                          return n; }
    wff* u_modalnu() {
        wff *mf = new wff;
        mf->type = data1 == 1 ? wff::cond :
                   data1 == 2 ? wff::bicond :
                   data1 == 3 ? wff::vee :
                   wff::wedge;
        if (l) mf->a = l->u_modalnu();
        else { mf->a = new wff;
            mf->a->type = (data2 == '0' ? wff::falsum : wff::prop);
            mf->a->p = data2; }
        if (d) mf->b = d->u_modalnu();
        else { mf->b = new wff; mf->b->type = (data3 == '0' ? wff::falsum : wff::prop); mf->b->p = data3; }
        return mf;
    }

    bool ekval(dummy_struct* drugi) {
        if (data1 == drugi->data1 && data2 == drugi->data2 && data3 == drugi->data3)
        {
            if (!l != !drugi->l || !d != !drugi->d)
                return false;
            if (l)
                if (! l->ekval(drugi->l))
                    return false;
            if (d)
                if (! d->ekval(drugi->d))
                    return false;
            return true;
        }
        else return false;
    }
};
void rek_ispis(dummy_struct* r, string pref, bool l = true);

// efektivno generira sve particije multiskupa s card. = granauk, tipovaop različitih elemenata
void generiraj_operatore(int tipovaop, int granauk, int trtip,
                         vector<vector<dummy_struct*> >& skupovi, vector<dummy_struct*> &trenutni_skup);

void generiraj_strukture(vector<dummy_struct*> ugradeni, vector<dummy_struct*> preostali, vector <dummy_struct*> &strukture);

void generiraj_formule(vector<wff*> &rez, int kompleksnost, int operatori, int mdb, int VARIABLE_COUNT);



#endif // GENERATOR_H
