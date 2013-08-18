int HOW_MANY_OPERATORS = 0;

;

#include <sstream>
#include <fstream>
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
    cout << pref << (l ? "L: " : "R: ") << r->data1 << endl;
    if (r->l) rek_ispis(r->l, pref + "\t", true);
    if (r->d) rek_ispis(r->d, pref + "\t", false);
}


bool show = true;
bool glmode = false;

modalna_formula* istina; // _|_ -> _|_, najjednostavnije za gen.

bool prop_istovrijedne(modalna_formula* f, modalna_formula* g)
{
    modalna_formula* q1 = new modalna_formula;
    q1->tip = 3;
    q1->a = new modalna_formula;
    q1->a->tip = 7;
    q1->a->a = f->kopija();
    q1->a->b = g->kopija();
    q1->flatten();
    stablo s3;
    s3.glmode = false;
    s3.izgradi_za(q1);
    delete q1;
    return s3.zatvorena;
}

bool pre_prune(modalna_formula* mf)
{

    // -1. ako je jednaka istini, ok
    if (istina->je_jednaka(mf))
        return false; // najjednostavniji oblik istine se ne eliminira

    // 0.0 formule u kojima izrazi nisu leksikografski poredani (_|_ > p)
    // formule u kojima je ~p izraženo kao p <-> _|_
    // formule u kojima se javlja ocita dvostruka negacija
    if (mf->tip > 4)
        if (mf->a->tip < 2 && mf->b->tip < 2)
        {
            if (mf->a->tip < mf->b->tip)
                return true;
            if (mf->a->p > mf->b->p)
                return true;

        }

    if (mf->tip > 3)
        if (mf->b->tip == 0 || mf->a->tip == 0)
            if (mf->tip != 4)
                return true;
    if (mf->tip == 4)
        if (mf->b->tip == 4)
            if (mf->b->b->tip == 0)
                return true;




    // 0.5 ako je jednaka tvrdnji o dokazivosti istine (_|_ -> _|_)
    modalna_formula* sto_imam = mf;
    int lev = 0;
    while (sto_imam->tip == 2)
    {
        ++lev;
        sto_imam = sto_imam->a;
    }
    if (istina->je_jednaka(sto_imam)) // ne treba && lev jer taj slucaj prije
        return true;

    // 1. ako sadrzi tautologiju (jer tada postoji manja koja ju sadrzi)
    modalna_formula* negacija = new modalna_formula;
    negacija->a = mf->kopija();
    negacija->tip = 3;
    negacija->otkud = 101;
    stablo s;
    s.glmode = false;
    negacija->flatten();
    s.izgradi_za(negacija);
    delete negacija;
    if (s.zatvorena) return true;

    // 2. ako je oblika [][]...[]_|_ -> [][]...[]
    //    ili [][]...[]F /</-> [][]...[]G, i F <-> G
    if (mf->tip == 4 || mf->tip == 7)
    {
        int level = 0;
        modalna_formula* sto_imam = mf->a;
        while (sto_imam->tip == 2)
        {
            ++level;
            sto_imam = sto_imam->a;
        }
        if (level)
        {
            stablo s2;
            s2.glmode = false;
            modalna_formula* kop = sto_imam->kopija();
            kop->flatten();
            s2.izgradi_za(kop);
            delete kop;
            //if (s2.zatvorena)
            //{
                int level2 = 0;
                modalna_formula* sto_imam2 = mf->b;
                while (sto_imam2->tip == 2)
                {
                    ++level2;
                    sto_imam2 = sto_imam2->a;
                }
                if (level2 >= level && s2.zatvorena &&
                        (mf->tip == 4 || level2 == level))
                    return true;
                if (level2 == level)
                {
                    bool zatv = prop_istovrijedne(sto_imam, sto_imam2);

                    if (zatv)
                        return true;
                }
            //}
        }
    }

    // pretp da su svi pointeri validni
    bool v1 = false, v2 = false;
    if (mf->tip > 1)
        v1 = pre_prune(mf->a);
    if (v1) return true;
    if (mf->tip > 3)
        v2 = pre_prune(mf->b);
    return v1 || v2;
}

bool rek(modalna_formula *mf)
{

    bool v1 = false, v2 = false;
    if (mf->tip > 1)
        v1 = rek(mf->a);
    if (v1) return true;
    if (mf->tip > 3)
        v2 = rek(mf->b);
    return v1 || v2;
}

bool add_prune(modalna_formula* item)
{
    bool fail = false;

   /* if (rek(item))
    {
        return true;
    }*/

    vector<modalna_formula*> polje;
    item->pokupi_djecu(polje);
    vector<char> slova;
    for (modalna_formula* el : polje)
        if (el->je_negacija())
            if (el->a->tip == 1)
                if (find(slova.begin(), slova.end(), el->a->p) == slova.end())
                    slova.push_back(el->a->p);
    if (slova.size())
    for (char slovo : slova)
    {
        fail = true;
        for (modalna_formula* el : polje)
        {
            if (el->tip > 1 && el->tip <= 3)
                if (el->a->p == slovo)
                    fail = false;
            if (el->tip > 3)
            {
                if (el->b->p == slovo)
                    fail = false;
                if (el->a->p == slovo && el->b->tip != 0)
                    fail = false;
            }
         }
        if (fail)
            return fail;
    }
    return fail;
}

bool prop_normalno_ekvivalentne(modalna_formula *&f, modalna_formula *&g)
{
    if (f->je_jednaka(g)) return true;
    if (prop_istovrijedne(f, g)) return true;

    if (f->tip == 2 && g->tip == 2)
        return prop_normalno_ekvivalentne(f->a, g->a);
    if (f->tip == 4 && g->tip == 4)
    {
        return prop_normalno_ekvivalentne(f->a, g->a) &&
                prop_normalno_ekvivalentne(f->b, g->b);
    }
    return false;
}

void konstr(modalna_formula* f, string ulaz)
{
    stringstream Ulaz;
   Ulaz << ulaz; //jaojao.c_str();
   f->feed(Ulaz);
   cout << "Formula: " << f;
   bool zan = pre_prune(f);
   bool zan2 = add_prune(f);
   f->flatten();
   stablo s;
   //s.glmode = false;
   s.izgradi_za(f);
   if (show)
   {
       cout << endl << "Translation: " << f << endl << "Tree:\n";
       cout << s;
       //cout << "\"Interestigness\": " << ((zan ? 0 : 1)*2 + (zan2 ? 0 : 1)) << "/3" << endl;
   }
   else cout << "\t |-> " << (s.zatvorena ? "X" : "O") << endl;

}

void auto_prover(string outputloc)
{
    ofstream baza(outputloc);

    vector<modalna_formula*> mfs;
     cout << endl << "> (1/2) Generating formulas 0%"; cout.flush();
     int max = HOW_MANY_OPERATORS + 1;
     for (int k = 1; k < max; ++k)
     {
         auto res = generiraj_formule(k, 1);
         mfs.insert(mfs.end(), res.begin(), res.end());
         cout << "\r> (1/2) Generating formulas " << (100 * k / (max - 1)) << "%"; cout.flush();
     }
     cout << "\r> (1/2) Generating formulas completed, size: " << mfs.size() << endl;
     cout << endl << "> ...";
     vector<modalna_formula*> nevaljane;
     vector<modalna_formula*> valjane;
     int step = mfs.size() / 2500;
     if (!step) step = 1;
     step = 1;
     for (int c = 0; c < mfs.size(); )
     {
         modalna_formula* &item = mfs[c++];
         if (c % step == 0 || c == mfs.size())
             cout << "\r> (2/2) Building trees " << (100 * c / mfs.size()) << "% [" << c << "/" << mfs.size() << "]";

         cout.flush();

         //cout << item << endl;

         bool fail =  pre_prune(item);
         if (fail) continue;

         stablo s;
         modalna_formula* negacija = new modalna_formula;
         negacija->tip = 3;
         negacija->a = item->kopija();
         negacija->flatten();

         s.izgradi_za(negacija);

         if (s.zatvorena)
         {

             vector<modalna_formula*>temp;
             item->pokupi_djecu(temp);
             int vel1 = temp.size();
             for (int k = 0; k < valjane.size(); ++k)
             {
                 modalna_formula* item2 = valjane[k];
                 bool sadrzan = prop_normalno_ekvivalentne(item, item2);
                 if (!sadrzan && item->tip == 2)
                 {
                     sadrzan = prop_normalno_ekvivalentne(item->a, item2);
                      if (!sadrzan && item->a->tip == 2)
                      {
                         sadrzan = prop_normalno_ekvivalentne(item->a->a, item2);
                      }
                 }
                 if (sadrzan)
                 {
                     vector<modalna_formula*>temp2;
                     item2->pokupi_djecu(temp2);
                     if (vel1 > temp2.size()) fail = true;
                     else
                     {
                         valjane.erase(valjane.begin() + k);
                         k--;
                     }
                 }

             }
             if (!fail)
             {
                 fail = add_prune(item);
             }
             if (!fail) // nije isti if jer je fail volatile
             {
                 /*stringstream jj; jj << item; string jaojao = jj.str();
                 for (auto &a : jaojao)
                 {   if (a == '#') a = '0'; if (a == 'B') a = '1'; if (a == '~') a = '2';
                     if (a == '>') a = '3'; if (a == '&') a = '4'; if (a == '|') a = '5';
                     if (a == '=') a = '6'; if (a == '(') a = '7'; if (a == ')') a = '8';
                     if (a == ' ') a = '9'; }
                 system(("./upl.sh "+ jaojao +" > /dev/null").c_str());*/
                 valjane.push_back(item);
                 baza << item << endl;
                 baza.flush();
             }
          }
         else
         {
             nevaljane.push_back(item);
         }
         delete negacija;
     }

     int c = 0;
     if (!valjane.empty())
     {
         cout << "\n\nFirst 100 valid: " << endl;
         for (modalna_formula* item : valjane)
         {
             {
                 cout << item << endl;
                 ++c;
             }
             if (c > 100) break;
         }
     }

     cout << endl << "> " << mfs.size() - nevaljane.size() << "T, " << nevaljane.size() << "F" << endl << endl;

     cout << "> cleaning up... \n";
     for (auto item : mfs)
         delete item;
}

int main()
{
   istina = new modalna_formula;
   istina->tip = 5;
   istina->a = new modalna_formula;
   istina->b = new modalna_formula;

   fancy = 1; // use 2 if console supports unicode

   if (HOW_MANY_OPERATORS)
        auto_prover("output2");

    cout << "> use 'help' for introduction \n";
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
                cout << "Use prefix notation, instead a > b, use > a b.\n\n" \
                        "Available operators: # contradiction, ~ negation, B provable, \n" \
                        "& conjunction, > conditional, + disjunction, = biconditional. \n\n" \
                        "Everything else is a propositional letter.\n\n" \
                        "Examples [to check validity add ~ in front of the rest of the formula]: \n\t";
                modalna_formula* tmp= new modalna_formula;
                string ulaz = "=B#B~B~p";
                stringstream tmp2; tmp2 << ulaz; tmp->feed(tmp2); cout << tmp << ", enter: " << ulaz << "\n\t";
                ulaz = "=B=p~BpB=p~B#";
                tmp2 << ulaz; tmp->feed(tmp2); cout << tmp << ", enter: " << ulaz << endl;
                cout << endl << "Other commands:\nexit\nshow\toutput the tree - on\nhide\t"
                     << "output the tree - off\nview n\t0 one-char primitive view, 1 default, 2 fancy unicode output\ninst\tsome memory usage statsn\n\n"
                     << "To use autoprover, change the '0' in first line of main.cpp to "
                     << "the number of operators you'd like your formulas to contain."
                        << "\n\nLuka Mikec 2013, luka.mikec1-at-gmail-dot-com\n\n";
                delete tmp;
            }
            else if (ulaz.substr(0, 4) == "exit") exit(0);
            else if (ulaz.substr(0, 4) == "show") {show = true; }
            else if (ulaz.substr(0, 4) == "hide") {show = false; }
            else if (ulaz.substr(0, 4) == "inst") {cout << modalna_formula::obrisano << "/" << modalna_formula::instancirano << endl; }
            else if (ulaz.substr(0, 4) == "flat") {stringstream ss; ss << "=ab"; f->feed(ss); f->flatten();  }
            else if (ulaz.substr(0, 4) == "auto") {stringstream ss; ss << ulaz; ss >> ulaz; ss >> HOW_MANY_OPERATORS; auto_prover("output2"); }
            else if (ulaz.substr(0, 4) == "view") {stringstream ss; ss << ulaz; ss >> ulaz; ss >> fancy; if (fancy < 0 || fancy > 2) fancy = 0; }
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

