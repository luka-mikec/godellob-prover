#include "prover.h"

wff* istina; // _|_ -> _|_, najjednostavnije za gen.
bool verbose;

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

bool equivalent_formulas(wff* f, wff* g, bool modally, tree::modal_logic_type logic = tree::K)
{
    wff* q1 = new wff(wff::neg, new wff(wff::bicond, f, g));
    tree s3;
    s3.modal_mode = modally;
    if (modally) s3.modal_logic = logic;
    s3.build_for(q1);
    q1->a->a = q1->a->b = 0;
    delete q1;
    return s3.closed_branch;
}

bool implies(wff* f, wff* g, bool modally, tree::modal_logic_type logic = tree::K)
{
    wff* q1 = new wff(wff::neg, new wff(wff::cond, f, g));
    tree s3;
    s3.modal_mode = modally;
    if (modally) s3.modal_logic = logic;
    s3.build_for(q1);
    q1->a->a = q1->a->b = 0;
    delete q1;
    //cout << s3 << endl;
    return s3.closed_branch;
}

bool impl_net_print_formulas = true;
struct implication_net
{
    bool could_be_base = true;
    vector<implication_net*> children;
    vector<wff*> klasa;
    wff* rep;
    int id;
    static int ids;

    //implication_net(vector<wff*> w) : klasa(w) { if (w.size()) rep = w[0]; }
    implication_net(wff* w)  {klasa.push_back(w); rep = w; ++ids; id = ids; }
    void print(int level = 0) {
        cout << string(level, '\t') << id << endl;
        if (impl_net_print_formulas )
            for (wff* f : klasa) cout << string(level, '\t') << f << endl; cout << endl;
        for (auto n : children) n->print(level + 1);
    }
};

int implication_net::ids = 0;

bool implies_by_necesit(wff* item2, wff* item)
{
   wff* post_check = 0;

   wff* currently_checking = item;
   bool contained = false;
   bool do_nec_check = true;

   if (currently_checking->type != wff::box)
   {
       do_nec_check = false;
       if (currently_checking->binary())
           post_check = currently_checking;
       else
           return false;
   }
   else
   {
       currently_checking = currently_checking->a;
   }

   while (do_nec_check) // find if F <=> G, for some already contained G
   {
       contained = //equivalent_formulas(currently_checking, item2, true, tree::K);
                    currently_checking->commutes_syntactically_equals(item2);

       if (currently_checking->binary() && !contained) // is it a conseq. by |- P => |- []P
           post_check = currently_checking;

       if (currently_checking->type != wff::box) break;
       currently_checking = currently_checking->a;
   }

   bool fail = false;
   if (!contained && post_check)
   {
       wff* sto_imam = post_check->a, * sto_imam2 = post_check ->b;
       while (sto_imam->type == wff::box && sto_imam2->type == wff::box)
       {
           sto_imam = sto_imam->a;
           sto_imam2 = sto_imam2->a;
           wff* minimizirana = new wff(post_check->type, sto_imam, sto_imam2);
           /*for (int j = 0; j < pruned->size(); ++j)
           {
               if (j == to_skip) continue;

               if (minimizirana->commutes_syntactically_equals(pruned->at(j)))
               {
                   fail = true; break;
               }
           }*/
           if (minimizirana->commutes_syntactically_equals(item2))
           {
               fail = true; break;
           }
           minimizirana->a = 0; minimizirana->b = 0; delete minimizirana;
           if (fail) break;
       }
   }
   return contained || fail;

}

void auto_prover(string outputloc, int formula_complexity, int operator_diversity, int modal_depth, int VARIABLE_COUNT, bool subst_instead_of_normality, int start_from)
{
    /*wff* item, *item2;
    stringstream prva, druga; prva << "B>B>BppBp"; druga << ">B>BppBp";
    item = new wff(); item->feed(prva);
    item2 = new wff(); item2->feed(druga);
    vector<wff *> sve {item, item2};

    cout << "a" << implies_by_necesit(item2, item) << endl;
    exit(0);*/



    vector<wff*> mfs;

     cout << endl << "> (1/3) Generating formulas 0%"; cout.flush();
     int max = formula_complexity + 1;
     for (int k = 1; k < max; ++k)
     {
         generiraj_formule(mfs, k, operator_diversity, /*modalna dubina:*/ modal_depth, VARIABLE_COUNT);
         //mfs.insert(mfs.end(), res.begin(), res.end());
         cout << "\r> (1/3) Generating formulas " << (100 * k / (max - 1)) << "%"; cout.flush();
     }

     cout << "\r> (1/3) Generating formulas completed, size: " << mfs.size() << endl;
     cout << endl << "> ...";

     vector<wff*> valjane, pruned, base_formulas; //, nevaljane;

     int step = /*mfs.size()/2500;if(!step)step=1;*/ 1;

     clock_t timer = clock();

     /*bool semi_k4_prune = false;
     stringstream additiv; additiv << "&&B>BpBBp>BpBBpBB>BpBBp";
     wff *dodatak; if (semi_k4_prune) dodatak = new wff(additiv);*/

     float spec_timer = 0;
     int cnt = 0;
     for (int c = start_from; c < mfs.size(); )
     {
         // output status to stdout
         wff* &item = mfs[c++];

         if (!verbose)
         {
         if (c % step == 0 || c == mfs.size())
             cout << "\r> (2/3) Building trees " << (100 * c / mfs.size())
                  << "% [" << c << "/" << mfs.size() << ", "
                  << (float)(clock() - timer) / CLOCKS_PER_SEC << "s spent"
                  << "]";
         cout.flush();
         }

         //cout << endl << item << endl;
         // build negation of formula
         wff* negacija;
         /*if (semi_k4_prune)
             negacija= new wff(wff::neg,
                                 new wff(wff::cond,
                                         dodatak->deep_copy(),
                                         item->deep_copy()));
         else*/
         negacija = new wff(wff::neg, item->deep_copy());

         if (verbose)
            cout << "Checking: " << item << " ";

         // check validity
         tree precheck_tree, check_tree;
         measure(negacija, precheck_tree);
         if (precheck_tree.closed_branch)
         {
            check_tree.modal_logic = tree::K4;
            check_tree.build_for(negacija);
         }

         if (verbose)
            cout << "GL valid: " << precheck_tree.closed_branch
                 << ", K valid: " << check_tree.closed_branch << endl;

         if (!check_tree.closed_branch && precheck_tree.closed_branch)
            valjane.push_back(item);
         delete negacija;
     }

     float totaltime = (float)(clock() - timer) / CLOCKS_PER_SEC;
     cout << endl << "> Took " << totaltime << "s" << endl << endl;

     cout << endl << cnt << endl;

     timer = clock();
     for (int i = 0; i < valjane.size(); ++i)
     {
         cout << "\r> (3/3) Determining interestigness " << (100 * i / valjane.size())
              << "% [" << i << "/" << valjane.size() << ", "
              << (float)(clock() - timer) / CLOCKS_PER_SEC << "s spent]";
         cout.flush();

         wff* item = valjane[i];
         // skip those who have equiv's, as will (was) be declared with flag
         if (item->additional_data)
             continue;



         bool fail = false;
         void* flag = new int(1);
         for (int k = 0; k < valjane.size(); ++k)
         {
             if (i == k) continue;

             wff* item2 = valjane[k];

             if (item->commutes_syntactically_equals(item2))
             {
                 if (item2->additional_data)
                 {
                     fail = true;
                     item->additional_data = flag;
                 }
                 else
                 {
                  /*   if (item2->syntactically_equals(new wff("=B>BppBp")))
                     {
                         cout << "a";
                     }*/
                     item->additional_data = item2->additional_data = flag;
                 }
             }
             if (!fail)
                 if (implies_by_necesit(item2, item))
                     fail = true;
             if (!fail)
                 if (implies(item2, item, false))
                     fail = true;
             if (fail) break;
         }
         delete flag;

         if (!fail) // all checks ok, add
             pruned.push_back(item);
     }




     totaltime = (float)(clock() - timer) / CLOCKS_PER_SEC;
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
             if (c > 25) { cout << "(complete list in " << outputloc << ")" << endl; break;}
         }
     }
     c = 0;
     if (!pruned.empty())
     {
         ofstream baza(outputloc);

         cout << "\n\nFirst 100 valid and interesting: " << endl;
         for (wff* item : pruned)
         {
             if (c == 25) { cout << "(complete list in " << outputloc << ")" << endl; }
             if (c < 25)
                cout << item << endl;

             baza << item << endl;
             ++c;
         }

         cout << "\n\n\n";

         /*for (wff* item : pruned)
         {
             cout << item << "\t"; cout.flush();
             wff* negacija= new wff(wff::neg, item->deep_copy());
             tree check_tree;
             check_tree.modal_logic = tree::K4;
             check_tree.build_for(negacija);
             delete negacija;

             if (check_tree.closed_branch)
             {
                 cout << "4VALID " << endl;
             }
             else
                 cout << " " << endl;
         }*/

         cout << "\nFinding equiv's (out of "<< pruned.size() <<"f's) [";

         tree::modal_logic_type grouper_logic = tree::K4;

         vector< implication_net* > klase;
         for (int a = 0; a < pruned.size(); ++a)
         {
             wff* t = pruned[a];

             bool found_home = false;
             for (int klasa = 0; klasa < klase.size(); ++klasa)
             {
                 bool uvjet;
                 if (subst_instead_of_normality)
                      uvjet = t->is_a_subst_instance_of(klase[klasa]->klasa[0])
                              && (klase[klasa]->klasa[0])->is_a_subst_instance_of(t);
                 else
                     uvjet = equivalent_formulas(klase[klasa]->klasa[0], t, true, grouper_logic);
                 if (uvjet)
                 {
                     klase[klasa]->klasa.push_back(t);
                     found_home = true;
                 }
             }
             if (!found_home)
                 klase.push_back(new implication_net(t));

             cout << "."; cout.flush();
         }
         cout << "]\n";


         int max_n_klasa = 1500;
         bool spajanja[max_n_klasa][max_n_klasa];
         bool min_spajanja[max_n_klasa][max_n_klasa];

         for (int i = 0; i < max_n_klasa; ++i) {
             for (int j = 0; j < max_n_klasa; ++j) {
                 spajanja[i][j] = min_spajanja[i][j] = false;
             }
         }

         baza << endl << endl;
         wff::set_style(3);
         for (int a = 0; a < klase.size(); ++a)
         {
             cout << a;
             baza << a << endl;
             for (wff* form : klase[a]->klasa)
             {
                 wff* nicer = form->deep_copy(); nicer->beautify();
                 cout << " &\t $" << nicer << "$ &\t $" << form << "$ \\\\"
                      << (form == klase[a]->klasa.back() ?  "\\\cline{1-3} \n" : "\n");
                 baza << "\t\t" << form << endl;
             }
            for (int b = 0; b < klase.size(); ++b)
            {
                if (a == b) continue;

                bool uvjet;

                if (subst_instead_of_normality)
                    uvjet = klase[b]->rep-> is_a_subst_instance_of(klase[a]->rep);
                else
                    uvjet = implies(klase[a]->rep, klase[b]->rep, true, grouper_logic);

                if (uvjet)
                {
                    klase[a]->children.push_back(klase[b]);
                    klase[b]->could_be_base = false;
                    //cout << b << " ne zbog " << a << endl;
                    spajanja[a][b] = true;
                    min_spajanja[a][b] = true;
                    // cout << "(" << a << ", "  << b << ") ";
                    // cout << b + 1 << " ";
                    //baza << "\t" << pruned[b] << endl;
                }
            }
            //cout << endl;
         }


         for (int a = 0; a < klase.size(); ++a)
         {

            for (int b = 0; b < klase.size(); ++b)
            {
                for (int c = 0; c < klase.size(); ++c)
                {
                    if (a == b || b == c || a == c) continue;
                    if (spajanja[a][b] && spajanja[b][c])
                        min_spajanja[a][c] = false;
                }
            }
         }

         string dotloc = outputloc + ".dot";
         string pngloc = outputloc + ".eps";
         ofstream dotfile(dotloc);
         dotfile << "digraph impl {\n ";
         for (int a = 0; a < klase.size(); ++a)
         {
            bool bar_1 = false;
            for (int b = 0; b < klase.size(); ++b)
            {
                if (min_spajanja[a][b])
                {
                    dotfile << "\t" << a << " -> " << b << ";\n";
                    bar_1 = true;
                }
            }
            if (!bar_1) cout << "\t" << a << ";\n";
         }
         dotfile << "}\n";
         dotfile.close();

         system(("dot -Teps " + dotloc +" > " + pngloc).c_str());
         system(("xnview " + pngloc + " > /dev/null 2>&1").c_str());
         /*
         for (int a = 0; a < klase.size(); ++a)
         {
            // cout << a  << endl;
            klase[a]->children.clear();
            for (int b = 0; b < klase.size(); ++b)
            {
                if (min_spajanja[a][b])
                {
                    klase[a]->children.push_back(klase[b]);
                    //cout << "\t" << b << endl;
                }
            }
         }
         impl_net_print_formulas = false;
         for (auto k : klase)
            if (k->could_be_base)
                k->print();
          */
     }


     cout << endl << "> " << mfs.size() << " checked, "
          << valjane.size() << " valid, " << pruned.size() << " valid and interesting. " << endl << endl;

     cout << "> cleaning up... \n";
     for (auto item : mfs)
         delete item;
}
