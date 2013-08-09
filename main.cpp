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


bool show = true;


void konstr(modalna_formula* f, string ulaz)
{
    stringstream Ulaz;
   Ulaz << ulaz; //jaojao.c_str();
   f->feed(Ulaz);
   cout << "Ulaz: " << f;
   f->flatten();
   stablo s;
   s.izgradi_za(f);
   if (show)
   {
       cout << endl << "U minimalnom jeziku: " << f << endl << "Stablo:\n";
       cout << s << endl;
   }
   else cout << "\t |-> " << (s.zatvorena ? "X" : "O") << endl;

}

int main()
{
   vector<modalna_formula*> mfs;
    cout << endl << "> (1/2) generiranje formula 0%"; cout.flush();
    int max = 3;
    for (int k = 1; k < max; ++k)
    {
        auto res = generiraj_formule(k, 2);
        mfs.insert(mfs.end(), res.begin(), res.end());
        cout << "\r> (1/2) generiranje formula " << (100 * k / (max - 1)) << "%"; cout.flush();
    }
    cout << "\r> (1/2) generiranje formula: " << mfs.size() << endl;
    cout << endl << "> ...";
    vector<modalna_formula*> nevaljane;
    vector<modalna_formula*> valjane;
    int c = 0;
    for (modalna_formula* item : mfs)
    { ++c;
        cout << "\r> (2/2) gradnja modela " << (100 * c / mfs.size()) << "% [" << c << "/" << mfs.size() << "]";

        cout.flush();
        stablo s;
        modalna_formula* negacija = new modalna_formula;
        negacija->tip = 3;
        negacija->a = item->kopija();
        negacija->flatten();

        s.izgradi_za(negacija);


        //if (c == 439) cout << endl << endl << s << endl << endl;
        if (s.zatvorena)
        {
            valjane.push_back(item);
         }
        else
        {
            nevaljane.push_back(item);
        }
    }

    if (!valjane.empty())
        for (modalna_formula* item : valjane)
        {
            cout << endl << item;
        }

    cout << endl << "> " << mfs.size() - nevaljane.size() << "T, " << nevaljane.size() << "F" << endl;


    cout << "> help za pomoc\n";
    while (1)
    {
        cout << "? ";
        modalna_formula *f = new modalna_formula;
        string ulaz = "";
        if (!getline(cin, ulaz))
        {
            break;
        }
        if (ulaz.size() == 0) break;


        if (ulaz.length() >= 4)
            if (ulaz.substr(0, 4) == "help")
            {
                cout << "Formule se unose prefiksno, dakle umjesto a > b, treba > a b.\n\n" \
                        "Dostupni operatori: # kontradikcija (bez arg.), ~ negacija, B bew/dokazivo, \n" \
                        "& konjunkcija, > kondicional, + disjunkcija, = bikondicional. \n\n" \
                        "Sve ostalo duljine 1 znaka osim praznina je propozicijsko slovo.\n\n" \
                        "Primjeri formula [za provjeru valjanosti dodati ~ ispred svake]: \n\t";
                modalna_formula* tmp= new modalna_formula;
                string ulaz = "=B#B~B~p";
                stringstream tmp2; tmp2 << ulaz; tmp->feed(tmp2); cout << tmp << ", upisati: " << ulaz << "\n\t";
                ulaz = "=B=p~BpB=p~B#";
                tmp2 << ulaz; tmp->feed(tmp2); cout << tmp << ", upisati: " << ulaz << endl;
                delete tmp;
            }
            else if (ulaz.substr(0, 4) == "exit") exit(0);
            else if (ulaz.substr(0, 4) == "show") {show = true; }
            else if (ulaz.substr(0, 4) == "hide") {show = false; }
            else if (ulaz.substr(0, 4) == "inst") {cout << modalna_formula::obrisano << "/" << modalna_formula::instancirano << endl; }
            else konstr(f, ulaz);
        else
        {
            konstr(f, ulaz);
        }

       /* for (auto el : modalna_formula::adresice)
        {
            cout << el << " : " << el->otkud << endl;
        }*/

        delete f;

        //cin.ignore();

    }

    return 0;
}

