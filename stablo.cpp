#include "stablo.h"

string motherlevel(const stablo &s)
{
    if (s.mother == 0)
        return "";
    else
        return motherlevel(*s.mother) + "\t" + (s.mother->razina != s.razina ? "|" : "");
}

ostream& operator<<(ostream& out, const stablo &s)
{
    for (int i = 0; i < s.formule_grane.size(); ++i)
    {
        out << motherlevel(s) << s.formule_grane[i] << endl;
    }

    for (int i = 0; i < s.stabla_grane.size(); ++i)
    {
        out << *s.stabla_grane[i];
    }

    out << motherlevel(s);
    out << (s.zatvorena ? "[X]" : "[ ]") << endl << endl;

    return out;
}
