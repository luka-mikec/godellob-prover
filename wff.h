#ifndef MODALNA_FORMULA_H
#define MODALNA_FORMULA_H

/* modal formula (well, the well formed one :D)
 */

#include "lukalib.h"
using namespace std;

struct wff
{
    // type and content
    enum types // 0 _|_, 1 prop var, 2 [], 3 ~, 4 ->;     5 &&, 6 ||, 7 <->
    {
        falsum, prop, box, neg, cond, wedge, vee, bicond // diamond print only
    } type = falsum;
    wff *a = 0, *b = 0;
    char p = 'x';
    void *additional_data = 0;
    bool actually_is_diamond = false; // is box actually diamond?

    // input/output
    static int print_style;
    static bool set_style(int s); // 0 primitive, 1 default, 2 unicode if supported
    string get_prefix_rep(); // in portable prefix notation
    friend ostream& operator<<(ostream& out, wff *f); // text representation in print_style
    void feed(istream& mother); // load formula (in prefix notation)
    void feed(string mother);
    friend istream& operator>>(istream& in, wff *f); // same as feed

    // statistics
    void show_mem(string pref);

    // helpers
    void flatten(); // reducing languange to {~, ->}, Props and #
    void beautify(); // expanding languange from {~, ->}, Props and #, to {everything}
    bool syntactically_equals(wff *f);
    bool commutes_syntactically_equals(wff *f);
    bool syntactically_negation_of(wff *f);
    bool syntactically_negation_of(); // of anything
    bool is_a_subst_instance_of(wff *f, map<char, wff*>* changes = 0);
    void collect_subwffs(vector<wff*> &field); // collects sub-wffs
    static vector<wff*> deep_copy_set(vector<wff*> &skup);
    static vector<wff*> shallow_copy_set(vector<wff*> &skup);
    bool primitive()    {return type == falsum || type == prop;}
    bool unary()        {return type == box || type == neg;}
    bool binary()       {return type == cond || type == wedge || type == vee || type == bicond;}
    bool standard()     {return type == cond || type == bicond;}
    bool operates()     {return unary() || binary();}

    // ctor/dctor, memory
    static uintmax_t instantiated_new, instantiated_copies, deleted;
    wff *deep_copy();
    wff() {++instantiated_new;}
    wff (istream& where) {++instantiated_new; feed(where);}
    wff (string where) {++instantiated_new; feed(where); }
    wff (types t, char letter) {type = t; p = letter; ++instantiated_new;}
    wff (types t, wff* child1 = 0, wff* child2 = 0) {
        type = t; a = child1; b = child2; ++instantiated_new;}
    ~wff();

};



#endif // MODALNA_FORMULA_H
