#ifndef MODALNA_FORMULA_H
#define MODALNA_FORMULA_H

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
             //#define dbgmsg
//#undef dbgmsg
using namespace std;

struct modalna_formula
{
    int tip = 0; // 0 _|_, 1 prop var, 2 [], 3 ~, 4 ->;     5 &&, 6 ||, 7 <->

    modalna_formula *a = 0, *b = 0;

    char p = 'x';

    static int instancirano, obrisano;
    static vector<modalna_formula*> adresice;
    modalna_formula()
    {
        ++instancirano;
        //adresice.push_back(this);
    }

    void flatten() // svodenje jezika na {~, ->} u Prop u {#}
    {
        if (tip == 5)
        {
            tip = 3;
            modalna_formula* pogodba = new modalna_formula;
            pogodba->tip = 4;
            pogodba->a = a;
            a = pogodba;
            a->b = new modalna_formula;
            a->b->tip = 3;
            a->b->a = b; // have fun future bug-solving luka
            b = 0;
        }
        else if (tip == 6)
        {
            tip = 4;
            modalna_formula* negacija = new modalna_formula;
            negacija->a = a;
            negacija->tip = 3;
            a = negacija;
        }
        else if (tip == 7)
        {
            modalna_formula *pog1, *pog2, *neg1, *pogtot;
            tip = 3;
            pog1 = new modalna_formula;
            pog1->tip = 4;
            pog1->a = a->kopija();
            pog1->b = b->kopija();
            pog2 = new modalna_formula;
            pog2->tip = 4;
            pog2->a = b->kopija();
            pog2->b = a->kopija();
            neg1 = new modalna_formula;
            neg1->tip = 3;
            neg1->a = pog2;
            pogtot = new modalna_formula;
            pogtot->tip = 4;
            pogtot->a = pog1;
            pogtot->b = neg1;
            b = 0;
            a = pogtot;
        }

        if (tip >= 2 && tip <= 4) // jer je sad sve svedeno na 2 do 4
            a->flatten();
        if (tip == 4)
            b->flatten();
    }

    void pokupi_djecu(vector<modalna_formula*> &polje)
    {
        polje.push_back(this);
        if (a) a->pokupi_djecu(polje);
        if (b) b->pokupi_djecu(polje);
    }


    void feed(istream& mother)
    {
        char c = ' ';
        while (c == ' ') (c = mother.get());

        if (c == '#')
            return;
        if (c == 'B' || c == '~')
        {
            tip = (c == 'B' ? 2 : 3);
            a = new modalna_formula;
            a->feed(mother);
        }
        else if (c == '>')
        {
            tip = 4;
            a = new modalna_formula; b = new modalna_formula;
            a->feed(mother); b->feed(mother);
        }
        else if (c == '&')
        {
            tip = 5;
            a = new modalna_formula; b = new modalna_formula;
            a->feed(mother); b->feed(mother);
        }
        else if (c == '+')
        {
            tip = 6;
            a = new modalna_formula; b = new modalna_formula;
            a->feed(mother); b->feed(mother);
        }
        else if (c == '=')
        {
            tip = 7;
            a = new modalna_formula; b = new modalna_formula;
            a->feed(mother); b->feed(mother);
        }
        else
        {
            tip = 1;
            p = c;
        }
    }

    bool je_jednaka(modalna_formula *f)
    {
       if (f->tip != tip) return false;

        switch (tip)
        {
        case 0:     break;
        case 1:     if (f->p != p) return false; break;
        case 2:     if (! a->je_jednaka(f->a)) return false; break;
        case 3:     if (! a->je_jednaka(f->a)) return false; break;
        case 4: case 5: case 6: case 7:
                    if (! a->je_jednaka(f->a)) return false;
                    if (! b->je_jednaka(f->b)) return false;
                    break;
        }
        return true;
    }

    int otkud = 0;

    bool je_negacija(modalna_formula *f)
    {
        modalna_formula *a = new modalna_formula;
        a->otkud = 1;
        a->tip = 3;
        a->a = this;
        if (f->je_jednaka(a)) {a->a = 0; delete a; return true;}

        a->a = f;
        if (je_jednaka(a)) {a->a = 0; delete a; return true;}

        a->a = 0; delete a;
        return false;
    }

    modalna_formula *kopija()
    {
        modalna_formula *dvojnik = new modalna_formula;
        dvojnik->tip = tip;
        switch (tip)
        {
        case 0:     break;
        case 1:     dvojnik->p = p; break;
        case 2:     dvojnik->a = a->kopija(); break;
        case 3:     dvojnik->a = a->kopija(); break;
        case 4: case 5: case 6: case 7:
                    dvojnik->a = a->kopija();
                    dvojnik->b = b->kopija();
                    break;
        }
        return dvojnik;
    }

    void show_mem(string pref)
    {
#ifdef dbgmsg
        cout << pref << (void*)this << endl;
        cout.flush();
        if (tip > 1) if (a) a->show_mem(pref + " ");
        if (tip > 3) if (b) b->show_mem(pref + " ");
        #endif
    }

    ~modalna_formula()
    {
#ifdef dbgmsg
        cout << "BR: " << (void*)this << " ~ " << this <<
                (otkud ? " temp " : " ") << otkud << endl;
        cout.flush();
        if (!otkud)
               cout << "break";
#endif
        ++obrisano;
       /* auto it = std::find(adresice.begin(),adresice.end(),this);
        if (it != adresice.end())
          adresice.erase(it);*/

        //stringstream quickie;
        //quickie << this;
        if (tip > 1 && a) {a->otkud = otkud; delete a;}
        if (tip > 3 && b) {b->otkud = otkud; delete b;}
    }

    friend ostream& operator<<(ostream& out, modalna_formula *f);
};

#endif // MODALNA_FORMULA_H
