#include "tree.h"

/* tree.h impl, and:
 * - operator<<, for outputing to ostreams
 */

void tree::collect_subtrees(vector<wff*> &cres)
{
    if (formulas.size())
        for (wff* mf : formulas)
            mf->collect_subwffs(cres);
    if (trees.size())
        for (tree* st : trees)
            st->collect_subtrees(cres);
}

void tree::push(wff *f, bool also_flatten)
{
    if (trees.size())
        for (auto gr : trees)
            gr->push(f, also_flatten);
    else
        formulas.push_back(f->deep_copy());
    formulas.back()->flatten();
}

void tree::push(wff *l, wff *d)
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
        novo->closed_branch = check_contradictions(l, modal_depth);
        trees.push_back(novo);

        novo = new tree(this);
        novo->formulas.push_back(d->deep_copy());
        novo->closed_branch = check_contradictions(d, modal_depth);
        trees.push_back(novo);
    }
}

bool tree::check_contradictions(wff *f, int raz /*= 0*/)
{
    if (f->type == wff::falsum) return true;
    if (!formulas.empty())
    for (auto gr : formulas)
        if (f->syntactically_negation_of(gr))
            return true;
    if (mother != 0 && mother->modal_depth == raz)
        return mother->check_contradictions(f, raz);
    return false;
}

void tree::collect_boxed_formulas(vector<wff* > &result, int lev, bool include_parents)
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
        if (mother->modal_depth == lev) // optim
            mother->collect_boxed_formulas(result, lev);
}

void tree::solve_formula(wff *f)
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
            closed_branch = check_contradictions(f, modal_depth); break;
        case wff::neg:
            closed_branch = check_contradictions(f, modal_depth);
            if (!closed_branch)
            {
                if (f->a->type == wff::neg)
                    push(f->a->a);
                else if (f->a->type == wff::cond)
                {
                    push(f->a->a);
                    wff *b = new wff(wff::neg, f->a->b);
                    push(b);
                    b->a = 0; delete b;
                }
            }
            break;

        case wff::cond:
           closed_branch = check_contradictions(f, modal_depth);
            if (!closed_branch)
            {
                wff *a = new wff(wff::neg, f->a);
                if (f->b->type == wff::falsum)
                    push(a);
                else
                    push(a, f->b);
                a->a = 0; delete a;
            }
            break;
        }
    }
}

bool tree::open_window(wff* mf, vector<wff*> &boxed) // for modal logics
{
    if (closed_branch) return true;

    bool sve_zat = true;
    bool no_pl_branches = true;
    if (!trees.empty())
        if (trees.front()->modal_depth == modal_depth)
             no_pl_branches = false;

    if (!no_pl_branches) // there are (pl-)subbranches, so do the work there
    {
        for (auto s : trees)
            if (!s->closed_branch)
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

    tree* window = new tree(this);

    if (modal_logic == K4 || modal_logic == GL)
    {
        if (!boxed.empty()) // add all previous []F formulas
        for (wff* f: boxed) window->push(f);
    }
    if (!boxed.empty()) // add F for every previous []F formulas
    for (wff* f: boxed)
    {
        bool contained = false;

        for (auto tr : window->formulas)
            for (auto it = boxed.begin(); it != boxed.end(); ++it)
                if ((*it)->syntactically_equals(f->a))
                    {contained = true; break;}

        if (!contained) window->push(f->a);
    }

    wff * negation = new wff(wff::neg, mf->a->a);
    window->push(negation); // for ~[]specific_F, add ~specific_F
    if (modal_logic == GL)
        window->push(mf->a);    // for ~[]F, add []F
    negation->a = 0; delete negation;

    window->modal_depth = modal_depth + 1;
    if (modal_logic == K4 || modal_logic == GL) // first n are [], so no _|_
        window->start_solving_from = boxed.size();
    trees.push_back(window);
    window->solve();
    if (window->closed_branch)
        closed_branch = true;
    return window->closed_branch;
}

void tree::solve()
{
    if (closed_branch)
        return;

    int sorted_head = 0; // sorted to speed up, solve everything before ->
    for (int i = start_solving_from; i < formulas.size(); ++i)
    {
        if (sorted_head < formulas.size())
        {
            sort(formulas.begin() + sorted_head, formulas.end(), [](wff *a, wff *b)
            {
                if (a->type == b->type) return false;
                if (a->type == wff::falsum) return true;
                if (b->type == wff::falsum) return false;
                if (a->type == wff::cond) return false;
                if (b->type == wff::cond) return true;
                return (a->type < b->type);
            });
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
            s->solve();
        if (!s->closed_branch) svezat = false;
    }
    closed_branch = closed_branch || svezat;

    if (!modal_mode) return;
    if (closed_branch) return;

    vector<wff* > negations_of_box;
    for (auto i = formulas.begin(); i != formulas.end(); ++i)
        if ((*i)->type == wff::neg)
            if ((*i)->a->type == wff::box)
                negations_of_box.push_back(*i);

    vector<wff* > boxed_formulas;
    collect_boxed_formulas(boxed_formulas, modal_depth);

    if (!negations_of_box.empty())
    {
        bool allcl = true;
        for (auto *mf : negations_of_box)
            if (!open_window(mf, boxed_formulas))
                allcl = false;
        if (allcl) closed_branch = true;
    }
}

bool tree::check_integrity()
{   if (trees.size()) {
        int tip1 = trees[0]->modal_depth;
        for (auto t : trees) if (t->modal_depth != tip1) return false;
        int chk = 0;
        for (auto t : trees) {
            if (!t->check_integrity()) return false;
            if (t->modal_depth != modal_depth && t->closed_branch)
            { if (chk == 1) {cout << this << endl;return false;}
                chk = 1;}}} return true;
}

string motherlevel(const tree &s)
{
    if (s.mother == 0)
        return "";
    else
        return motherlevel(*s.mother) + "\t" + (s.mother->modal_depth != s.modal_depth ? "|" : "");
}

ostream& operator<<(ostream& out, const tree &s)
{
    for (int i = 0; i < s.formulas.size(); ++i)
        out << motherlevel(s) << s.formulas[i] << endl;

    for (int i = 0; i < s.trees.size(); ++i)
        out << *s.trees[i];

    out << motherlevel(s);
    out << (s.closed_branch ? "[X]" : "[ ]") << endl << endl;

    return out;
}
