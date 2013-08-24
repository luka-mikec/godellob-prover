/*
 *pre_prune: superseeded by better generator, and K-validity checks


bool pre_prune(wff* mf)
{
    // -1. allow truths
    if (istina->syntactically_equals(mf))
        return false; // najjednostavniji oblik istine se ne eliminira

    // 0.0 commutative operations with unsorted contents
    if (mf->binary() && mf->type != wff::cond)
        if (mf->a->primitive() && mf->b->primitive())
        {
            if (mf->a->type < mf->b->type) return true;
            if (mf->a->p > mf->b->p)        return true;
        }

    // falsum outside of conditional
    if (mf->binary())
        if (mf->b->type == wff::falsum || mf->a->type == wff::falsum)
            if (mf->type != wff::cond) return true;

    // double negation
    if (mf->type == wff::cond)
        if (mf->b->type == wff::cond)
            if (mf->b->b->type == wff::falsum) return true;

    // [][][]...[](_|_ -> _|_), since that's T
    wff* sto_imam = mf;
    int lev = 0;
    while (sto_imam->type == wff::box)
    {   ++lev;
        sto_imam = sto_imam->a;
    }
    if (istina->syntactically_equals(sto_imam)) return true;

    // 1. ako sadrzi tautologiju (jer tada postoji manja koja ju sadrzi)
    wff* negacija = new wff(wff::neg, mf);
    tree s;
    s.modal_mode = false;
    s.build_for(negacija);
    negacija->a = 0; delete negacija;
    if (s.closed_branch) return true;

    // of form [][]...[]_|_ -> [][]...[]something
    // or [][]...[]F /</-> [][]...[]G, i F <-> G
    if (mf->type == wff::cond || mf->type == wff::bicond)
    {
        int level = 0;
        wff* sto_imam = mf->a;
        while (sto_imam->type == wff::box)
        {   ++level;
            sto_imam = sto_imam->a;
        }
        if (level)
        {
            tree s2;
            s2.modal_mode = false;
            s2.build_for(sto_imam);
            int level2 = 0;
            wff* sto_imam2 = mf->b;
            while (sto_imam2->type == wff::box)
            {   ++level2;
                sto_imam2 = sto_imam2->a;
            }
            if (level2 >= level && s2.closed_branch &&
                    (mf->type == wff::cond || level2 == level))
                return true;
            if (level2 == level)
            {
                bool zatv = prop_istovrijedne(sto_imam, sto_imam2);
                if (zatv) return true;
            }
        }
    }

    bool v1 = false, v2 = false;
    if (mf->operates())
        v1 = pre_prune(mf->a);
    if (v1) return true;
    if (mf->binary())
        v2 = pre_prune(mf->b);
    return v1 || v2;
}

// this is just a special case of K-validity
bool prop_normalno_ekvivalentne(wff *&f, wff *&g)
{
    if (f->syntactically_equals(g)) return true;
    if (prop_istovrijedne(f, g)) return true;

    if (f->type == wff::box && g->type == wff::box)
        return prop_normalno_ekvivalentne(f->a, g->a);
    if (f->type == wff::cond && g->type == wff::cond)
    {
        return prop_normalno_ekvivalentne(f->a, g->a) &&
                prop_normalno_ekvivalentne(f->b, g->b);
    }
    return false;
}



  */
