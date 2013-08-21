#ifndef STABLO_H
#define STABLO_H

#include "wff.h"
using namespace std;

struct tree // svako stabalce, svoja grana je!
{
    bool glmode = true;
    tree* mother = 0;
    int modal_depth = 0; // koliko duboko u stablu s obzirom na modalne svjetove
    vector<wff*> formulas;
    vector<tree*> trees;

    bool closed_branch = false;

    void collect_subtrees(vector<wff*> &cres)
    {
        for (wff* mf : formulas)
            mf->collect_subwffs(cres);
        if (trees.size())
            for (tree* st : trees)
                st->collect_subtrees(cres);
    }

    void push(wff *f)
    {
        if (trees.size())
            for (auto gr : trees)
                gr->push(f);
        else
            formulas.push_back(f->deep_copy());
    }

    tree() {}
    tree(tree* mom) {mother = mom; glmode = mom->glmode; modal_depth = mom->modal_depth;}

    void push(wff *l, wff *d)
    {
        if (trees.size() > 0) {
            for (auto gr : trees)
                if (!gr->closed_branch)
                    gr->push(l, d);
        }
        else
        {
            tree* novo = new tree(this);
            novo->formulas.push_back(l->deep_copy());
            novo->closed_branch = kontrad_na_gore(l, modal_depth);
            trees.push_back(novo);

            novo = new tree(this);
            novo->formulas.push_back(d->deep_copy());
            novo->closed_branch = kontrad_na_gore(d, modal_depth);
            trees.push_back(novo);
        }
    }

    bool kontrad_na_gore(wff *f, int raz /*= 0*/)
    {
        if (f->type == wff::falsum) return true;
        if (!formulas.empty())
        for (auto gr : formulas)
        {
            if (f->syntactically_negation_of(gr))
                return true;
        }
        if (mother != 0 && mother->modal_depth == raz)
            return mother->kontrad_na_gore(f, raz);
        return false;
    }

    void collect_boxed_formulas(vector<wff* > &result, int lev, bool include_parents = true)
    {
        if (!formulas.empty())
        for (auto gr : formulas)
            if (gr->type == wff::box)
            {
                bool sadrzi = false;
                if (result.size())
                for (auto ff : result)
                    if (gr->syntactically_equals(ff))
                    {sadrzi = true; break;}
                if (!sadrzi)
                    result.push_back(gr);
            }
        if (mother != 0 && include_parents)
            if (mother->modal_depth == lev) // ne bi bila greska, ali da ne bloata nepotrebno
                mother->collect_boxed_formulas(result, lev);
    }

    void solve_formula(wff *f)
    {
        // formulu kod sebe sredimo samo ako nema djece
        if (trees.size()) {
            for (auto gr : trees)
                if (!gr->closed_branch)
                    gr->solve_formula(f);
        }
        else if (!closed_branch)
        {
            switch (f->type)
            {
            case wff::falsum:  closed_branch = true; break;
            case wff::prop:
            case wff::box:
                closed_branch = kontrad_na_gore(f, modal_depth); break;
            case wff::neg:
                closed_branch = kontrad_na_gore(f, modal_depth);
                if (!closed_branch)
                {
                    if (f->a->type == wff::neg)
                        push(f->a->a);
                    else if (f->a->type == wff::cond)
                    {
                        push(f->a->a);
                        wff *b = new wff(wff::neg, f->a->b);
                        push(b);
                        //b->otkud = 2; // DBG
                        b->a = 0; delete b;
                    }
                }
                break;

            case wff::cond:
               closed_branch = kontrad_na_gore(f, modal_depth);
                if (!closed_branch)
                {
                    wff *a = new wff(wff::neg, f->a), *b = f->b;
                    push(a, b);
                    //a->otkud = 3; // DBG
                    a->a = 0; delete a;
                }
                break;
            }
        }
    }

    bool open_window(wff* mf, vector<wff*> &boxed)
    {
        if (closed_branch)
            return true;

        bool sve_zat = true;

        bool no_pl_branches = true;
        if (!trees.empty())
            for (auto s : trees)
                if (s->modal_depth == modal_depth)
                    no_pl_branches = false;

        if (!no_pl_branches) // there are (pl-)subbranches, so do the work there
        {

            for (auto s : trees)
                if (!closed_branch && !s->closed_branch && s->modal_depth == modal_depth)
                {
                    auto &kop = boxed;
                    int len = kop.size();
                    s->collect_boxed_formulas(kop, modal_depth, false);
                    bool val = s->open_window(mf, kop);
                    sve_zat = sve_zat && val;
                    if (kop.size() != len)
                        kop.erase(kop.begin() + len, kop.end());
                    if (!sve_zat) break;
                }
            if (sve_zat)
                closed_branch = sve_zat;
            return closed_branch;
        }

        tree * novo = new tree(this);
        wff * negacija = new wff(wff::neg, mf->a->a);
        novo->push(negacija);
        novo->push(mf->a);
        negacija->a = 0; delete negacija;

        if (!boxed.empty())
        for (wff* f: boxed)
        {
            bool prva = false, druga = false;
            for (auto tr : novo->formulas)
                if (tr->syntactically_equals(f))
                    {prva = true; break;}

            for (auto tr : novo->formulas)
                if (tr->syntactically_equals(f->a))
                    {druga = true; break;}

            //if (!prva) novo->push(f);
            //if (!druga) novo->push(f->a);
            if (!prva) novo->formulas.push_back(f);
            if (!druga) novo->formulas.push_back(f->a);
        }
        novo->modal_depth = modal_depth + 1;
        trees.push_back(novo);
        novo->rijesi_svoje();
        if (novo->closed_branch)
            closed_branch = true;
        return novo->closed_branch;
    }

    void rijesi_svoje()
    {
        if (closed_branch)
            return;

        int sorted_head = 0; // sorted to speed up, solve everything before ->
        for (int i = 0; i < formulas.size(); ++i)
        {
           // if (sorted_head < formulas.size())
            {
                auto r = min_element(formulas.begin() + i, formulas.end(), [](wff *a, wff *b)
                {
                    if (a->type == b->type) return false;
                    if (a->type == wff::falsum) return true;
                    if (b->type == wff::falsum) return false;
                    if (a->type == wff::cond) return false;
                    if (b->type == wff::cond) return true;
                    return (a->type < b->type);
                });
                /*if (mother == 0)
                {
                    cout << "\n\nbiram " << *r << endl;
                    for (int j = 0; j < formulas.size(); ++j)
                    {
                        cout << formulas[j] << "\n";
                    }

                }*/
                if (r != formulas.begin() + i)
                {
                    //swap(*r, formulas[i]);
                }
                sorted_head = formulas.size();
            }
            solve_formula( formulas[i]);
            if (closed_branch) return;
        }

        bool svezat = !trees.empty();
        if (!trees.empty())
        for (auto s : trees)
        {
            if (!s->closed_branch) // branch possibly closed on opening
                s->rijesi_svoje();
            if (!s->closed_branch) svezat = false;
        }
        closed_branch = closed_branch || svezat;

        if (!glmode)
            return;

        if (!closed_branch)
        {
            vector<wff* > negacije_modalnih;
            for (auto i = formulas.begin(); i != formulas.end(); ++i)
                if ((*i)->type == wff::neg)
                    if ((*i)->a->type == wff::box)
                        negacije_modalnih.push_back(*i);

            vector<wff* > kolekcija;
            collect_boxed_formulas(kolekcija, modal_depth);

            if (!negacije_modalnih.empty())
            {
                bool allcl = true;
                for (auto *mf : negacije_modalnih)
                    if (!open_window(mf, kolekcija))
                        allcl = false;
                if (allcl) closed_branch = true;
            }
        }
    }

    bool sve_ok()
    {
        if (trees.size())
        {
            int tip1 = trees[0]->modal_depth;
            for (auto t : trees)
            {
                if (t->modal_depth != tip1)
                    return false;
            }
            int chk = 0;
            for (auto t : trees)
            {
                if (!t->sve_ok())
                    return false;

                if (t->modal_depth != modal_depth &&
                        t->closed_branch)
                {
                    if (chk == 1)
                    {
                        cout << this << endl;
                        return false;
                    }
                    chk = 1;
                }
            }

        }
            return true;
    }

    void izgradi_za(wff *f)
    {
        formulas.clear();
        push(f);
        rijesi_svoje();
       /* if (!sve_ok())
            cout << "\n\nsranje\n\n";*/
    }

    ~tree()
    {
     for (auto mf : formulas)
     {
#ifdef dbgmsg
         cout << "Brisem: " << mf << endl;
         mf->show_mem("");
         cout << endl;
#endif
        delete mf;
     }
     for (auto mf : trees)
        delete mf;
    }

    void memchk()
    {
        vector<wff*> cres;
        tree* glavica = this;
        while (glavica->mother != 0) glavica = glavica->mother;
        glavica->collect_subtrees(cres);
        for (wff * mf : cres)
        {
            if (count(cres.begin(), cres.end(), mf) > 1)
                   cout << "BREAKMEUP";
        }

    }

    friend ostream& operator<<(ostream& out, const tree &s);
};

#endif // STABLO_H
