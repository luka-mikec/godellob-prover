int HOW_MANY_OPERATORS = 0;

#include <sstream>
#include <fstream>
#include "tree.h"
#include "generator.h"
#include <sys/time.h>
using namespace std;

void rek_ispis(dummy_struct* r, string pref, bool l = true)
{
    cout << pref << (l ? "L: " : "R: ") << r->data1 << endl;
    if (r->l) rek_ispis(r->l, pref + "\t", true);
    if (r->d) rek_ispis(r->d, pref + "\t", false);
}

bool show = true;
bool modal_mode = false;
int  modal_logic = 0;

wff* istina; // _|_ -> _|_, najjednostavnije za gen.

bool prop_istovrijedne(wff* f, wff* g)
{
    wff* q1 = new wff(wff::neg, new wff(wff::bicond, f, g));
    tree s3;
    s3.modal_mode = false;
    s3.build_for(q1);
    q1->a->a = q1->a->b = 0;
    delete q1;
    return s3.closed_branch;
}

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

bool add_prune(wff* item)
{
    bool fail = false;

    vector<wff*> polje;
    item->collect_subwffs(polje);
    vector<char> slova;
    for (wff* el : polje)
        if (el->syntactically_negation_of())
            if (el->a->type == wff::prop)
                if (find(slova.begin(), slova.end(), el->a->p) == slova.end())
                    slova.push_back(el->a->p);
    if (slova.size())
    for (char slovo : slova)
    {
        fail = true;
        for (wff* el : polje)
        {
            if (el->unary() )
                if (el->a->p == slovo)
                    fail = false;
            if (el->binary())
            {
                if (el->b->p == slovo)
                    fail = false;
                if (el->a->p == slovo && el->b->type != wff::falsum)
                    fail = false;
            }
         }
        if (fail) return fail;
    }
    return fail;
}

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



__syscall_slong_t measure(wff* f, tree& s)
{
    timespec ts, ps;
    clock_gettime(CLOCK_REALTIME, &ts);
    s.build_for(f);
    clock_gettime(CLOCK_REALTIME, &ps);
    return ps.tv_nsec - ts.tv_nsec;
}

void konstr(wff* f, string ulaz)
{
   stringstream Ulaz; Ulaz << ulaz; f->feed(Ulaz);

   cout << "Formula: " << f;

   tree s;
   s.modal_logic = (modal_logic == 0 ? tree::GL : modal_logic == 1 ? tree::K4 : tree::K);

   auto res = measure(f, s);

   if (show)
   {
       cout << endl << "Translation: " << f << endl << "Tree:\n";
       cout << s;
   }
   else cout << "\t |-> " << (s.closed_branch ? "X" : "O") << endl;
   cout << "Calc took " << (res / 1000000.) << "ms" << endl;
}

void auto_prover(string outputloc)
{
    ofstream baza(outputloc);

    vector<wff*> mfs;
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
     vector<wff*> nevaljane;
     vector<wff*> valjane;
     int step = mfs.size() / 2500;
     if (!step) step = 1;
     step = 1;

     __syscall_slong_t totaltime = 0;
     timespec ts, ps;
     clock_gettime(CLOCK_REALTIME, &ts);

     bool semi_k4_prune = false;

     stringstream additiv; additiv << "&&B>BpBBp>BpBBpBB>BpBBp";
     wff *dodatak;
     if (semi_k4_prune) dodatak = new wff(additiv);

     for (int c = 0; c < mfs.size(); )
     {
         wff* &item = mfs[c++];
         if (c % step == 0 || c == mfs.size())
             cout << "\r> (2/2) Building trees " << (100 * c / mfs.size())
                  << "% [" << c << "/" << mfs.size() << "]";

         cout.flush();

         bool fail = false;
         tree s;


         wff* negacija;
         if (semi_k4_prune)
             negacija= new wff(wff::neg,
                                 new wff(wff::cond,
                                         dodatak->deep_copy(),
                                         item->deep_copy()));
         else  negacija= new wff(wff::neg, item->deep_copy());

         negacija->flatten();

         auto t = measure(negacija, s);
         /*if (t > 1000000)
         {
            cout << t << endl;
            cout << negacija << endl;
         }*/

         if (s.closed_branch)
         {

             vector<wff*>temp;
             item->collect_subwffs(temp);
             int vel1 = temp.size();
             for (int k = 0; k < valjane.size(); ++k)
             {
                 wff* item2 = valjane[k];
                 bool sadrzan = prop_normalno_ekvivalentne(item, item2);
                 if (!sadrzan && item->type == wff::box)
                 {
                     sadrzan = prop_normalno_ekvivalentne(item->a, item2);
                      if (!sadrzan && item->a->type == wff::box)
                      {
                         sadrzan = prop_normalno_ekvivalentne(item->a->a, item2);
                      }
                 }
                 if (sadrzan)
                 {
                     vector<wff*>temp2;
                     item2->collect_subwffs(temp2);
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
                 tree josjedno;
                 josjedno.modal_logic = tree::K;
                 josjedno.build_for(negacija);
                 fail = josjedno.closed_branch;
             }
             if (!fail) fail = pre_prune(item);
             if (!fail) fail = add_prune(item);
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

     clock_gettime(CLOCK_REALTIME, &ps);
     totaltime = ps.tv_sec - ts.tv_sec;
     cout << endl << "Took " << totaltime << "s" << endl;

     int c = 0;
     if (!valjane.empty())
     {
         cout << "\n\nFirst 100 valid: " << endl;
         for (wff* item : valjane)
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
   istina = new wff;
   istina->type = wff::cond;
   istina->a = new wff;
   istina->b = new wff;

   wff::print_style = 1; // use 2 if console supports unicode

   if (HOW_MANY_OPERATORS)
        auto_prover("output2");

    cout << "> use 'help' for introduction \n";
    while (1)
    {
        cout << "? ";
        wff *f = new wff;
        string ulaz = "";
#ifdef EMSCRIPTEN
        cin >> ulaz;
        if (ulaz.length() > 3)
            if (ulaz.length() >= 4)
                if (ulaz.substr(0, 4) == "view" || ulaz.substr(0, 4) == "auto")
                {
                    ulaz += " ";
                    string r;
                    cin >> r;
                    ulaz += r;
                }
        if (!(cin))
#else
        if (!getline(cin, ulaz))
#endif
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
                wff* tmp= new wff;
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
            else if (ulaz.substr(0, 4) == "inst") {cout << wff::deleted << "/" << wff::instantiated_new << '(' << wff::instantiated_copies << ')' << endl; }
            else if (ulaz.substr(0, 4) == "flat") {stringstream ss; ss << "=ab"; f->feed(ss); f->flatten();  }
            else if (ulaz.substr(0, 4) == "auto") {stringstream ss; ss << ulaz; ss >> ulaz; ss >> HOW_MANY_OPERATORS; auto_prover("output2"); }
            else if (ulaz.substr(0, 4) == "setl") {stringstream ss; ss << ulaz; ss >> ulaz; ss >> modal_logic;}
            else if (ulaz.substr(0, 4) == "view") {stringstream ss; ss << ulaz; ss >> ulaz; ss >> wff::print_style; if (wff::print_style < 0 || wff::print_style > 2) wff::print_style = 0; }
            else konstr(f, ulaz);
        else
        {
            konstr(f, ulaz);
        }

        delete f;

    }
    return 0;
}

