#include "wff.h"
/*
 * wff impl., and:
 *  init_styles(), loads possible output styles (ascii, unicode etc.)
 *  operator<<, for outputing to ostreams
 */


int wff::print_style = 2;
uintmax_t wff::instantiated_new, wff::deleted,
          wff::instantiated_copies;

void wff::flatten()
{
    if (type == wedge)
    {
        type = neg;
        a = new wff(cond, a, new wff(neg, b));
        b = 0;
    }
    else if (type == vee)
    {
        type = cond;
        a = new wff(neg, a);
    }
    else if (type == bicond)
    {
        wff *pog1, *pog2, *neg1, *pogtot;
        type = neg;
        pog1 = new wff(cond, a->deep_copy(), b->deep_copy());
        pog2 = new wff(cond, b, a);
        neg1 = new wff(neg, pog2);
        pogtot = new wff(cond, pog1, neg1);
        a = pogtot;
        b = 0;
    }

    if (type == box || type == neg || type == cond) // type is now one of these
        a->flatten();
    if (type == cond)
        b->flatten();
}

void wff::beautify()
{
     // converting x -> y and x <-> y, where x or y = #

    if (standard() && b->type == falsum)
    {
        delete b; b = 0;
        type = neg;
    }
    if (type == bicond && a->type == falsum)
    {
        delete a; a = b; b = 0;
        type = neg;
    }
    if (standard() && a->syntactically_equals(b))
    {
        delete a; delete b; a = b = 0;
        type = prop;
        p = 'T';
    }


    while (type == neg && a->type != prop)
    {
        if (a->type == neg)
        {
            *this = *(a->a);
        }
        if (a->type == falsum)
        {
            type = prop;
            p = 'T';
        }
        if (a->type == box)
        {
            type = box;
            actually_is_diamond = !actually_is_diamond;
            a->type = neg;
        }
        if (a->type == cond)
        {
            type = wedge;
            b = new wff(neg, a->b);
            a = a->a;
        }
        if (a->type == bicond)
        {
            type = bicond;
            b = new wff(neg, a->b);
            a = new wff(neg, a->a);
        }
    }

    if (type == cond && a->type == neg)
    {
        type = vee;
        a = a->a;
    }

    if (operates()) // type is now one of these
        a->beautify();
    if (binary())
        b->beautify();
}

void wff::feed(istream& mother)
{
    char c = ' ';
    while (c == ' ') (c = mother.get());

    if (c == '#')
        return;
    if (c == 'B' || c == '~')
    {
        type = (c == 'B' ? box : neg);
        a = new wff(mother);
    }
    else if (c == '>')
    {
        type = cond;
        a = new wff(mother); b = new wff(mother);
    }
    else if (c == '&')
    {
        type = wedge;
        a = new wff(mother); b = new wff(mother);
    }
    else if (c == '+')
    {
        type = vee;
        a = new wff(mother); b = new wff(mother);
    }
    else if (c == '=')
    {
        type = bicond;
        a = new wff(mother); b = new wff(mother);
    }
    else
    {
        type = prop;
        p = c;
    }
}

void wff::feed(string mother)
{
    stringstream ss(mother);
    feed(ss);
}

bool wff::syntactically_equals(wff *f)
{
   if (f->type != type) return false;

    switch (type)
    {
    case falsum:     break;
    case prop:     if (f->p != p) return false; break;
    case box:      if (! a->syntactically_equals(f->a)) return false; break;
    case neg:      if (! a->syntactically_equals(f->a)) return false; break;
    case cond: case wedge: case vee: case bicond:
                if (! a->syntactically_equals(f->a)) return false;
                if (! b->syntactically_equals(f->b)) return false;
                break;
    }
    return true;
}

bool wff::commutes_syntactically_equals(wff *f)
{
    if (f->type != type) return false;

    switch (type)
    {
    case falsum:     break;
    case prop:     if (f->p != p) return false; break;
    case box:      if (! a->commutes_syntactically_equals(f->a)) return false; break;
    case neg:      if (! a->commutes_syntactically_equals(f->a)) return false; break;
    case cond:
        if (! a->commutes_syntactically_equals(f->a)) return false;
        if (! b->commutes_syntactically_equals(f->b)) return false;
        break;
    case bicond:  case wedge: case vee:
        if (! a->commutes_syntactically_equals(f->a) || ! b->commutes_syntactically_equals(f->b))
            if (! a->commutes_syntactically_equals(f->b) || ! b->commutes_syntactically_equals(f->a))
                return false;
        break;
    }
    return true;
}

bool wff::is_a_subst_instance_of(wff *f, map<char, wff*>* changes)
{
    if (changes == 0)
        changes = new map<char, wff*>();

    if (f->type != prop)
    {
        if (type != f->type)
            return false;
        else
        {
            if (type == wff::falsum && f->type == wff::falsum)
                return true;
            if (unary()) return a->is_a_subst_instance_of(f->a, changes);
            if (binary()) return a->is_a_subst_instance_of(f->a, changes)
                    && b->is_a_subst_instance_of(f->b, changes);
        }
    }

    if (changes->find(f->p) != changes->end())
        return syntactically_equals(changes->at(f->p));
    else
        return (*changes)[f->p] = this;
}

string wff::get_prefix_rep()
{
    switch (type)
    {
    case falsum:
        return "0";
    case prop:
        return string(1, p);
    case box:
        return "1" + a->get_prefix_rep();
    case neg:
        return "2" + a->get_prefix_rep();
    case cond:
        return "3" + a->get_prefix_rep() + b->get_prefix_rep();
    case wedge:
        return "4" + a->get_prefix_rep() + b->get_prefix_rep();
    case vee:
        return "5" + a->get_prefix_rep() + b->get_prefix_rep();
    case bicond:
        return "6" + a->get_prefix_rep() + b->get_prefix_rep();
    }
}

bool wff::syntactically_negation_of(wff *f)
{
    wff *a = new wff(neg, this); //a->otkud = 1; DBG
    if (f->syntactically_equals(a)) {a->a = 0; delete a; return true;}

    a->a = f;
    if (syntactically_equals(a)) {a->a = 0; delete a; return true;}

    a->a = 0; delete a;
    return false;
}

bool wff::syntactically_negation_of() // of anything
{
    if (type == wff::neg) return true;
    if (type == cond && b) return (b->type == falsum);
    return false;
}

wff *wff::deep_copy()
{
    ++instantiated_copies;
    wff *res = new wff(type);
    res->additional_data = additional_data;
    switch (type)
    {
    case falsum:     break;
    case prop:     res->p = p; break;
    case box:     res->a = a->deep_copy(); break;
    case neg:     res->a = a->deep_copy(); break;
    case cond: case wedge: case vee: case bicond:
                res->a = a->deep_copy();
                res->b = b->deep_copy();
                break;
    }
    return res;
}

void wff::collect_subwffs(vector<wff*> &field) // collects sub-wffs
{
    field.push_back(this);
    if (a) a->collect_subwffs(field);
    if (b) b->collect_subwffs(field);
}

vector<wff*> wff::shallow_copy_set(vector<wff*> &skup)
{   vector<wff*> novi;
    for (int i = 0; i < skup.size(); ++i)
        novi.push_back(skup[i]);
    return novi;
}

vector<wff*> wff::deep_copy_set(vector<wff*> &skup)
{   vector<wff*> novi;
    for (int i = 0; i < skup.size(); ++i)
        novi.push_back(skup[i]->deep_copy());
    return novi;
}

void wff::show_mem(string pref)
{
    cout << pref << (void*)this << endl; cout.flush();
    if (operates()) if (a) a->show_mem(pref + " ");
    if (binary()) if (b) b->show_mem(pref + " ");
}


wff::~wff()
{
    ++deleted;

    if (operates() && a) { delete a;}
    if (binary() && b) {delete b;}
}


bool _init_styles = false;
vector<map<string, string> > styles;

void init_styles()
{
    if (!_init_styles)
        _init_styles = true;
    else
        return;

    map<string, string> s1, s2, s3, s4;
    s1["#"] = "#"; s1["B"] = "B"; s1["~"] = "~";
    s1[">"] = ">"; s1["&"] = "&"; s1["|"] = "|"; s1["="] = "="; s1["D"] = "C";
    s1["T"] = "T";

    s2["#"] = "_|_"; s2["B"] = "[]"; s2["~"] = "~";
    s2[">"] = "->"; s2["&"] = "&"; s2["|"] = "||"; s2["="] = "<->"; s1["D"] = "<>";
    s2["T"] = "T";

    s3["#"] = "\u22A5"; s3["B"] = "\u25A1"; s3["~"] = "\u00AC";
    s3[">"] = "\u2192"; s3["&"] = "\u2227"; s3["|"] = "\u2228"; s3["="] = "\u2194"; s3["D"] = "\u25CA";
    s3["T"] = "T";

    s4["#"] = "\\bot"; s4["B"] = "\\square "; s4["~"] = "\\neg ";
    s4[">"] = "\\to"; s4["&"] = "\\wedge"; s4["|"] = "\\vee"; s4["="] = "\\leftrightarrow"; s4["D"] = "\\Diamond ";
    s4["T"] = "\\top";

    styles.push_back(s1);
    styles.push_back(s2);
    styles.push_back(s3);
    styles.push_back(s4);

    for (map<string, string>& s : styles)
        for (auto& par : s) if (par.first != "#" && par.first != "B" && par.first != "~" && par.first != "D")
            par.second = " " + par.second + " ";
}

bool wff::set_style(int s)
{
    init_styles();
    if (s < 0 || s >= styles.size())
        return false;
    print_style = s;
    return true;
}

istream& operator>>(istream& out, wff *f)
{
    f->feed(out);
}

ostream& operator<<(ostream& out, wff *f)
{    
    if (!_init_styles)
        init_styles();

    switch (f->type)
    {
    case wff::falsum:
        out << styles[wff::print_style]["#"]; break;
    case wff::prop:
        out << (f->p == 'T' ? styles[wff::print_style]["T"] : string(1, f->p)); break;
    case wff::box:
        out << styles[wff::print_style][f->actually_is_diamond ? "D" : "B"];
        if (f->a) if (!f->a->binary()) out << f->a;
                  else out << "(" << f->a << ")";
        else   out << "<null>"; break;
    case wff::neg:
        out << styles[wff::print_style]["~"];
        if (f->a)   if (!f->a->binary()) out << f->a;
                    else out << "(" << f->a << ")";
        else   out << "<null>"; break;
    default:
            {
                if (f->a)    if (!f->a->binary()) out << f->a;
                             else out << "(" << f->a << ")";
                else         out << "<null>";

                string obj="?";
                if (f->type == wff::cond) obj = ">";
                if (f->type == wff::wedge) obj = "&";
                if (f->type == wff::vee) obj = "|";
                if (f->type == wff::bicond) obj = "=";
                out << styles[wff::print_style][obj];

                if (f->b)  if (!f->b->binary()) out << f->b;
                           else out << "(" << f->b << ")";
                else       out << "<null>"; break;
            }
    }

    return out;
}
