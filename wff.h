#ifndef MODALNA_FORMULA_H
#define MODALNA_FORMULA_H

/* modal formula (well, the well formed one :D)
 */

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

struct wff
{
    // type and content
    enum types // 0 _|_, 1 prop var, 2 [], 3 ~, 4 ->;     5 &&, 6 ||, 7 <->
    {
        falsum, prop, box, neg, cond, wedge, vee, bicond
    } type = falsum;
    wff *a = 0, *b = 0;
    char p = 'x';

    // input/output
    static int print_style;
    bool set_style(int s); // 0 primitive, 1 default, 2 unicode if supported
    string get_prefix_rep(); // in portable prefix notation
    friend ostream& operator<<(ostream& out, wff *f); // text representation in print_style
    void feed(istream& mother); // load formula (in prefix notation)
    friend istream& operator>>(istream& in, wff *f); // same as feed

    // statistics
    void show_mem(string pref);

    // helpers
    void flatten(); // reducing languange to {~, ->}, Props and #
    bool syntactically_equals(wff *f);
    bool syntactically_negation_of(wff *f);
    bool syntactically_negation_of(); // of anything
    void collect_subwffs(vector<wff*> &field); // collects sub-wffs
    static vector<wff*> deep_copy_set(vector<wff*> &skup);
    static vector<wff*> shallow_copy_set(vector<wff*> &skup);
    bool primitive()    {return type == falsum || type == prop;}
    bool unary()        {return type == box || type == neg;}
    bool binary()       {return type == cond || type == wedge || type == vee || type == bicond;}
    bool operates()     {return unary() || binary();}

    // ctor/dctor, memory
    static uintmax_t instantiated_new, instantiated_copies, deleted;
    wff *deep_copy();
    wff() {++instantiated_new;}
    wff (istream& where) {++instantiated_new; feed(where);}
    wff (types t, char letter) {type = t; p = letter; ++instantiated_new;}
    wff (types t, wff* child1 = 0, wff* child2 = 0) {
        type = t; a = child1; b = child2; ++instantiated_new;}
    ~wff();

};

template <class T>
void vec_del(vector<T>v )
{
    for (typename vector<T>::iterator i = v.begin(); i != v.end(); ++i)
        delete *i;
}

#endif // MODALNA_FORMULA_H
