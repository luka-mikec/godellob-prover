This folder contains theorems.

File naming convention:

    <used_operator_1>_..._<used_operator_n>_md<max_modal_depth_per_subformula>_compl<max_number_of_binary_operators_used>
    
    
File content:

 - First line: major theorems 

 - Second line: minor theorems (deducible by combining major ths).

 - GL theorems.
 
 - K4 theorems.
 
If *B* is a substitution instance of *A*, this is shown as: 

    A
      B
      
If *C* follows from *A* by normality (by modal tautologies), this is written as: 

    A
      normal C
      
If *A* has both, subtitution instances are written first.

K4 theorems are considered boring and aren't given names, nor are organized in a tree.
