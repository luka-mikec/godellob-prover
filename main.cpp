#include "stablo.h"

using namespace std;




class okvir
{

};

class model
{
    okvir f;
};

int main()
{
    string test = "x>y";

    while (1)
    {
        cout << "? ";
        modalna_formula *f = new modalna_formula;
        f->feed(cin);
        /*cin.ignore();
        modalna_formula *g = new modalna_formula;
        g->feed(cin);
        cout << f->je_negacija(g);*/

        stablo s;

        s.izgradi_za(f);

        cout << "Ulaz: " << f << endl << "Stablo:\n" << s << endl;

        cin.ignore();

    }

    return 0;
}

