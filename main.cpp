#include <sstream>
#include "stablo.h"
#include "generator.h"

using namespace std;




class okvir
{

};

class model
{
    okvir f;
};

void rek_ispis(dummy_struct* r, string pref, bool l = true)
{
    cout << pref << (l ? "L: " : "D: ") << r->data1 << endl;
    if (r->l) rek_ispis(r->l, pref + "\t", true);
    if (r->d) rek_ispis(r->d, pref + "\t", false);
}

int main()
{
    string test = "x>y";
    /*vector<dummy_struct*> trs;
    vector<vector<dummy_struct*>> skupovi;
    generiraj_operatore(3, 3, 0, skupovi, trs);
    for (auto skup : skupovi)
    {
        for (auto opr : skup)
        {
            cout << opr->data1;
        }
        cout << endl;
    }

    vector<dummy_struct*> ugr;

    vector<dummy_struct*> preost = skupovi[5];
    vector<dummy_struct*> strukture;
    generiraj_strukture(ugr, preost, strukture);
    for (auto str : strukture)
    {
        rek_ispis(str, "");
    }

    vector<modalna_formula*> mfs = generiraj_formule(3);
    vector<modalna_formula*> nevaljane;
    modalna_formula* testna = new modalna_formula;
    stringstream ss; ss << "~>&p&ppp";
    testna->feed(ss);
    for (modalna_formula* item : mfs)
    {
        stablo s;
        modalna_formula* negacija = new modalna_formula;
        negacija->tip = 3;
        negacija->a = item;
        negacija->flatten();
        s.izgradi_za(negacija);
        if (s.zatvorena)
            cout << item << endl;
        else
            nevaljane.push_back(item);
    }
    cout << endl << nevaljane.size() << "F, " << mfs.size() - nevaljane.size() << endl;

    for (modalna_formula* item : nevaljane)
    {
       // cout << item << "\t";
    }*/

    while (1)
    {
        cout << "? ";
        modalna_formula *f = new modalna_formula;
        f->feed(cin);
        cout << "Ulaz: " << f << endl ;
        f->flatten();
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

