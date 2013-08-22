#ifndef STABLO_H
#define STABLO_H

/* modal tree construction for checking formula validity
 * supported logics: K, GL
 * half-supported: K4
 */

#include "wff.h"
using namespace std;

struct tree
{
    // data
    tree* mother = 0;
    int modal_depth = 0;
    vector<wff*> formulas;
    vector<tree*> trees;

    // metadata
    bool modal_mode = true;
    enum {K, K4, GL} modal_logic = GL;
    bool closed_branch = false;
    int start_solving_from = 0;

    // input/output
    friend ostream& operator<<(ostream& out, const tree &s);

    // tree operations
    void push(wff *l, wff *d);
    void push(wff *f, bool also_flatten = false);

    // helpers
    void collect_subtrees(vector<wff*> &cres);
    void collect_boxed_formulas(vector<wff* > &result, int lev, bool include_parents = true);

    // logic things
    bool check_contradictions(wff *f, int raz);
    bool open_window(wff* mf, vector<wff*> &boxed);
    void solve_formula(wff *f);
    void solve();

    // ctor, dctor
    tree() {}
    tree(tree* mom) {mother = mom; modal_mode = mom->modal_mode;
                     modal_logic = mom->modal_logic;  modal_depth = mom->modal_depth;}

    void build_for(wff *f) {formulas.clear(); push(f, true); solve(); }

    ~tree() {for (auto mf : formulas)   delete mf;
             for (auto mf : trees)      delete mf;}


    // debug
    bool check_integrity();

};

#endif // STABLO_H
