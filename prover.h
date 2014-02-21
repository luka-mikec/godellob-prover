#ifndef PROVER_H
#define PROVER_H

#include <fstream>
#include <ctime>
#include "tree.h"
#include "generator.h"

extern wff* istina; // _|_ -> _|_, najjednostavnije za gen.
extern bool verbose;
float measure(wff* f, tree& s);
bool equivalent_formulas(wff* f, wff* g, bool modally = false);
void auto_prover(string outputloc, int formula_complexity = 2, int operator_diversity = 1, int modal_depth = 2, int start_from = 0);

#endif // PROVER_H
