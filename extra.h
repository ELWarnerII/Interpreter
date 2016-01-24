/**
  @file basic.h
  @author David Sturgill (dbsturgi)

  Representation for extra language funcitonality, mostly expression types added
  by the student.
*/

#ifndef _EXTRA_H_
#define _EXTRA_H_

#include "core.h"

/** Make an expression that interprets its operands as long ints and
    evaluates to their sum.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that adds the values of op1 and op2
 */
Expr *makeAdd( Expr *op1, Expr *op2 );


/** A sub expression is like and add expression, but it evaluates to the value 
    of its first operand minus the value of its second operand.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that subtracts the value of op2 from op1    
 */
Expr *makeSub( Expr *op1, Expr *op2 );


/** A mul expression is like and add expression, but it evaluates to the 
    product of its operands' values.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that multiplies the values of op1 and op2  
 */
Expr *makeMul( Expr *op1, Expr *op2 );


/** A div expression is like and add expression, but it evaluates to the value 
    of its first operand divided by the value of its second operand.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that divides the value of op1 by op2
 */
Expr *makeDiv( Expr *op1, Expr *op2 );


/** An equal expression takes two expressions as operands. 
    It evaluates to the string "true" (true) if its operands are identical strings. 
    Otherwise, it evaluates to empty string (false).
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeEqual( Expr *op1, Expr *op2 );


/** A less expression takes two expressions as operands. It evaluates its operands,
    converts them to values of type long (using a values of zero for operands that 
    can't be parsed as long values). It evaluates to the string "true" if its first operand
    has a value less than its second. Otherwise, it evaluates to the empty string.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeLess( Expr *op1, Expr *op2 );


/** A not expression takes one expressions as an operand. The not expression evaluates
    to empty string if its operand evaluates to true (anything other than empty string). 
    Otherwise, the not expression evaluates to "true".
    @param op expression for the operand
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeNot( Expr *op );


/** A variable expression is simply the name of any variable. It evaluates to the current
    value of that variable. If the variable hasn't been set to a value, it just evaluates 
    to empty string.
    @param name the variable's name
    @return a new expression object that is either the value of the variable or an empty string
 */
Expr *makeVariable( char const *name );


/** A set expression has two operands, the first is the name of a variable, and the second an expression. 
    When it's evaluated, the set expression evaluates expr and then sets the given variable to whatever 
    this evaluates to. The set expression evaluates to whatever value is assigned.
    @param name the variable's name
    @param expr the expression to which the given variable evaluates to
    @return a new expression object which evaluates to the given expression
 */
Expr *makeSet( char const *name, Expr *expr );


/** An if expression contains two subexpressions, a condition, cond, and a body. Like you'd expect, 
    when it's evaluated, it evaluates its condition. If that evaluates to true (anything other than empty string), 
    it also evaluates its body. The whole expression evaluates to the value of its condition (so,
    true if the body is evaluated and false otherwise).
    @param cond the condition to be evaluated
    @param body the body to be evaluated if condition evaluates to true
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeIf( Expr *cond, Expr *body );


/** A while expression contains two subexpressions, a condition, cond, and a body. When evaluated, 
    it evaluates its condition. If the condition evaluates to true, it evaluates the body and and then 
    evaluates the condition again. This continues until the condition evaluates to false. The whole expression 
    evaluates to the number of times the body was evaluated.
    @param cond the condition to be evaluated
    @param body the body to be evaluated if condition evaluates to true
    @return a new expression object that is the number of times the condition evaluated to true
 */
Expr *makeWhile( Expr *cond, Expr *body );


/** An and expression takes two expressions as operands. It evaluates its first operand. 
    If it evaluates to true (anything other than empty string) it evaluates its second operand. 
    If the second operand also evaluates to true, then the and expression evaluates to "true". 
    Otherwise, the and expression evaluates to empty string. Note that, since it's short circuiting,
    it only evaluates the second operand if the first one evaluates to true.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeAnd( Expr *op1, Expr *op2);


/** An or expression takes two expressions as operands. It evaluates its first operand. 
    If it evaluates to false (empty string) then it evaluates its second operand. If the second operand
    also evaluates to false, then the or expression evaluates to empty string. Otherwise, the or expression 
    evaluates to "true". Since it's short circuiting, it only evaluates the second operand if the 
    first one evaluates to false.
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that is either "true" (true) or an empty string (false)
 */
Expr *makeOr( Expr *op1, Expr *op2);


/** A concat expression takes two expressions as operands. It evaluates its operands and then evaluates 
    to the concatenation of the strings the operands evaluate to..
    @param op1 expression for the left-hand operand
    @param op2 expression for the right-hand operand
    @return a new expression object that is a concatenation of the operands
 */
Expr *makeConcat( Expr *op1, Expr *op2);


/** A substr expression takes three expressions as operands. The first is evaluated and interpreted as a string, 
    and the next two are expected to evaluate to long int values (if they don't parse as long ints, then the substr 
    operation will just interpret them as having a value of zero, just like the other arithmetic operators). 
    The substr expression evaluates to the substring of its first operand stating at the position given as the second operand
    and continuing up to but not including the position given in the third operand. Characters in a string are indexed starting
    from zero, so the first example above would evaluate to "world".
    @param op1 expression for the string operand
    @param op2 expression for the start operand
    @param op3 expression for the end operand
    @return a new expression object that is a substring of op1 from op2 to op3
 */
Expr *makeSubstr( Expr *op1, Expr *op2, Expr *op3);

 #endif
