#include "tree.h"

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
