#include "prover.h"
using namespace std;



bool show = true;
bool modal_mode = false;
int  modal_logic = 1;
extern bool verbose;

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



int main()
{
    /*
    cout << (new wff("&Bpp"))->is_a_subst_instance_of(new wff("p")) << endl;
    cout << (new wff("&B&qrp"))->is_a_subst_instance_of(new wff("&Bpp")) << endl;
    cout << (new wff("&B&qr&qr"))->is_a_subst_instance_of(new wff("&Bpp")) << endl;
    exit(0);
    */

    int bin_ops = 2;
    int modal_depth = 2;
    int var_cnt = 1 + 2;
    bool subst = false;

    auto_prover("sz"+scast<string>(bin_ops)+"_cond_bicond_md"+scast<string>(modal_depth)+"_vars"+scast<string>(var_cnt)+"_"+(subst ? "subst" : "norm"), bin_ops, 2, modal_depth, var_cnt, subst);

   istina = new wff;
   istina->type = wff::cond;
   istina->a = new wff;
   istina->b = new wff;

   //wff::print_style = 1; // use 2 if console supports unicode

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
                cout << "Use prefix notation, instead of a > b, use > a b.\n\n" \
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
                     << "output the tree - off\nview n\t0 one-char primitive view, 1 default, 2 fancy unicode output\ninst\tsome memory usage statsn\nauto\tform_size operator_diversity modal_depth var_diversity\n"
                        << "\n\nLuka Mikec 2013, luka.mikec1-at-gmail-dot-com\n\n";
                delete tmp;
            }
            else if (ulaz.substr(0, 4) == "exit") exit(0);
            else if (ulaz.substr(0, 4) == "show") {show = true; }
            else if (ulaz.substr(0, 4) == "hide") {show = false; }
            else if (ulaz.substr(0, 4) == "verb") {verbose = !verbose; }
            else if (ulaz.substr(0, 4) == "inst") {cout << wff::deleted << "/" << wff::instantiated_new << '(' << wff::instantiated_copies << ')' << endl; }
            else if (ulaz.substr(0, 4) == "flat") {stringstream ss; ss << "=ab"; f->feed(ss); f->flatten();  }
            else if (ulaz.substr(0, 4) == "auto") {stringstream ss; ss << ulaz; ss >> ulaz;
                int formula_complexity, modal_depth, operator_diversity, VARIABLE_COUNT;
                ss >> formula_complexity >> operator_diversity >> modal_depth >> VARIABLE_COUNT;
                auto_prover("output2", formula_complexity, operator_diversity, modal_depth, VARIABLE_COUNT); }
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

/*k thms
dulji k-ekviv
necesitacija
distrib necesitacija
# -> bilosto
x -> x, osim za # -> #
koje sadrze (x -> #) -> #
# <-> x, x <-> #, x <-> x*/
