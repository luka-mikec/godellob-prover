#include "prover.h"

wff* istina; // _|_ -> _|_, najjednostavnije za gen.

float measure(wff* f, tree& s)
{
    clock_t t;
    t = clock();
    s.build_for(f);
    t = clock() - t;
    return (float)t / CLOCKS_PER_SEC;
}


bool prune_1(wff* item) // for every p that appears only as ~p, remove (i.e. if there is ~p, p required)
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

bool equivalent_formulas(wff* f, wff* g, bool modally)
{
    wff* q1 = new wff(wff::neg, new wff(wff::bicond, f, g));
    tree s3;
    s3.modal_mode = modally;
    if (modally) s3.modal_logic = tree::K;
    s3.build_for(q1);
    q1->a->a = q1->a->b = 0;
    delete q1;
    return s3.closed_branch;
}

void auto_prover(string outputloc, int formula_complexity, int operator_diversity, int start_from)
{
    ofstream baza(outputloc);

    vector<wff*> mfs;
     cout << endl << "> (1/3) Generating formulas 0%"; cout.flush();
     int max = formula_complexity + 1;
     for (int k = 1; k < max; ++k)
     {
         generiraj_formule(mfs, k, operator_diversity);
         //mfs.insert(mfs.end(), res.begin(), res.end());
         cout << "\r> (1/3) Generating formulas " << (100 * k / (max - 1)) << "%"; cout.flush();
     }
     cout << "\r> (1/3) Generating formulas completed, size: " << mfs.size() << endl;
     cout << endl << "> ...";

     vector<wff*> valjane, pruned; //, nevaljane;

     int step = /*mfs.size()/2500;if(!step)step=1;*/ 1;

     clock_t timer = clock();

     bool semi_k4_prune = false;
     stringstream additiv; additiv << "&&B>BpBBp>BpBBpBB>BpBBp";
     wff *dodatak; if (semi_k4_prune) dodatak = new wff(additiv);

     float spec_timer = 0;
     int cnt = 0;
     for (int c = start_from; c < mfs.size(); )
     {
         // output status to stdout
         wff* &item = mfs[c++];
         if (c % step == 0 || c == mfs.size())
             cout << "\r> (2/3) Building trees " << (100 * c / mfs.size())
                  << "% [" << c << "/" << mfs.size() << ", "
                  << (float)(clock() - timer) / CLOCKS_PER_SEC << "s spent"
                  << "]";
         cout.flush();

         //cout << endl << item << endl;
         // build negation of formula
         wff* negacija;
         if (semi_k4_prune)
             negacija= new wff(wff::neg,
                                 new wff(wff::cond,
                                         dodatak->deep_copy(),
                                         item->deep_copy()));
         else  negacija= new wff(wff::neg, item->deep_copy());


         // check validity
         tree precheck_tree, check_tree;
         measure(negacija, precheck_tree);
         if (precheck_tree.closed_branch)
         {
            check_tree.modal_logic = tree::K;
            check_tree.build_for(negacija);
         }

         if (!check_tree.closed_branch && precheck_tree.closed_branch)
            valjane.push_back(item);
         delete negacija;
     }

     float totaltime = (float)(clock() - timer) / CLOCKS_PER_SEC;
     cout << endl << "> Took " << totaltime << "s" << endl << endl;

     cout << endl << cnt << endl;

     timer = clock();
     int c = 0;
     for (auto item : valjane)
     {
         cout << "\r> (3/3) Determining interestigness " << (100 * c / valjane.size())
              << "% [" << ++c << "/" << valjane.size() << ", "
              << (float)(clock() - timer) / CLOCKS_PER_SEC << "s spent]";
         cout.flush();

         bool fail = false;

         /*stringstream strs(">B>B##BB#");wff* test = new wff(strs);
         if (test->syntactically_equals(item))cout << "break";*/

         if (!fail) // is it already contained?
         {
             wff* post_check = 0;
             for (int k = 0; k < pruned.size(); ++k)
             {
                 wff* item2 = pruned[k];
                 wff* currently_checking = item;
                 bool contained = false;

                 while (!contained) // find if F <=> G, for some already contained G
                 {
                     contained = equivalent_formulas(currently_checking, item2, true);

                     if (currently_checking->binary() && !contained) // is it a conseq. by |- P => |- []P
                         post_check = currently_checking;

                     if (currently_checking->type != wff::box) break;
                     currently_checking = currently_checking->a;
                 }

                 if (!contained && post_check)
                 {
                     wff* sto_imam = post_check->a, * sto_imam2 = post_check ->b;
                     while (sto_imam->type == wff::box && sto_imam2->type == wff::box)
                     {
                         sto_imam = sto_imam->a;
                         sto_imam2 = sto_imam2->a;
                         wff* minimizirana = new wff(post_check->type, sto_imam, sto_imam2);
                         for (int j = 0; j < pruned.size(); ++j)
                         {
                             if (minimizirana->syntactically_equals(pruned[j]))
                             {fail = true; break;}
                         }
                         minimizirana->a = 0; minimizirana->b = 0; delete minimizirana;
                         if (fail) break;
                     }
                 }

                 if (contained) // then choose the smaller one
                 {
                     vector<wff*>temp;
                     item->collect_subwffs(temp);

                     vector<wff*>temp2;
                     item2->collect_subwffs(temp2);
                     if (temp.size() > temp2.size())
                         fail = true;
                     else
                     {
                         //cout<<endl<<"del: "<<*(pruned.begin()+k)<<"b/c: "<<item<<endl;
                         pruned.erase(pruned.begin() + k--);
                     }
                     break;
                 }
             }
         }
         if (!fail) // all checks ok, add
         {
             /*stringstream jj; jj << item; string jaojao = jj.str(); // ne zaboravi dodat stari stil!
             for (auto &a : jaojao)
             {   if (a == '#') a = '0'; if (a == 'B') a = '1'; if (a == '~') a = '2';
                 if (a == '>') a = '3'; if (a == '&') a = '4'; if (a == '|') a = '5';
                 if (a == '=') a = '6'; if (a == '(') a = '7'; if (a == ')') a = '8';
                 if (a == ' ') a = '9'; }
             system(("./upl.sh "+ jaojao +" > /dev/null").c_str());*/
             pruned.push_back(item);
         }

     }

     totaltime = (float)(clock() - timer) / CLOCKS_PER_SEC;
     cout << endl << "Took " << totaltime << "s" << endl;

     c = 0;
     if (!valjane.empty())
     {
         cout << "\n\nFirst 100 valid and interesting: " << endl;
         for (wff* item : valjane)
         {
             {
                 cout << item << endl;
                 ++c;
             }
             if (c > 25) { cout << "(complete list in " << outputloc << ")" << endl; break;}
         }
     }
     c = 0;
     if (!pruned.empty())
     {
         cout << "\n\nFirst 100 valid and interesting: " << endl;
         for (wff* item : pruned)
         {
             if (c == 25) { cout << "(complete list in " << outputloc << ")" << endl; }
             if (c < 25)
                cout << item << endl;

             baza << item << endl;
             ++c;
         }
     }

     cout << endl << "> " << mfs.size() << " checked, "
          << valjane.size() << " valid, " << pruned.size() << " valid and interesting. " << endl << endl;

     cout << "> cleaning up... \n";
     for (auto item : mfs)
         delete item;
}
