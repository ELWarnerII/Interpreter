#include "extra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// For arithmetic operators, this is the maximum length of a long, printed
// out as a decimal (with a sign).
#define MAX_NUMBER 20

// Maximum variable length
#define MAX_VAR 20

// ASCII Values for invalid variable characters
#define LEFT_BRACKET 123
#define RIGHT_BRACKET 125
#define POUND 35

//////////////////////////////////////////////////////////////////////
// Variable expressions

/** Representation for an arbitrary variable operator.  The eval
    pointer decides what it computes. */
typedef struct {
  char *(*eval)( Expr *oper, Context *ctxt );
  void (*destroy)( Expr *oper );

  // One operand expressions.
  char *op1;
} VariableExpr;


/** Free memory for any type of VariableExpr instance. */
static void destroyVariable( Expr *expr )
{
  VariableExpr *this = (VariableExpr *)expr;
 
  free( this->op1 );
  // Then the VariableExpr struct itself.
  free( this );
}

/** Construct a VariableExpr representation and fill in the parts
    that are common to all SetExpr instances. */
static VariableExpr *buildVariableExpr( char const *op1 )
{
  VariableExpr *this = (VariableExpr *) malloc( sizeof( VariableExpr ) );
  this->destroy = destroyVariable;

  int len = strlen(op1);
  this->op1 = (char *)malloc( len + 1 );
  strcpy(this->op1, op1);
  this->op1[len] = '\0';

  return this;
}


//////////////////////////////////////////////////////////////////////
// Set expressions

/** Representation for an arbitrary set operator.  The eval
    pointer decides what it computes. */
typedef struct {
  char *(*eval)( Expr *oper, Context *ctxt );
  void (*destroy)( Expr *oper );

  // Two operand expressions.
  char *op1;
  Expr *op2;
} SetExpr;


/** Free memory for any type of SetExpr instance. */
static void destroySet( Expr *expr )
{
  SetExpr *this = (SetExpr *)expr;

  // Free our operand subexpressions.
  free(this->op1);
  this->op2->destroy( this->op2 );

  // Then the SetExpr struct itself.
  free( this );
}

/** Construct a SetExpr representation and fill in the parts
    that are common to all SetExpr instances. */
static SetExpr *buildSetExpr( char const *op1, Expr *op2 )
{
  SetExpr *this = (SetExpr *) malloc( sizeof( SetExpr ) );
  this->destroy = destroySet;

  int len = strlen(op1);
  this->op1 = (char *)malloc( len + 1 );
  strcpy(this->op1, op1);
  this->op1[len] = '\0';
  this->op2 = op2;

  return this;
}


//////////////////////////////////////////////////////////////////////
// Unary expressions

/** Representation for an arbitrary unary operator.  The eval
    pointer decides what it computes. */
typedef struct {
  char *(*eval)( Expr *expr, Context *ctxt );
  void (*destroy)( Expr *expr );

  // One operand expression.
  Expr *op;
} UnaryExpr;


/** Free memory for any type of UnaryExpr instance. */
static void destroyUnary( Expr *expr )
{
  UnaryExpr *this = (UnaryExpr *)expr;

  // Free our operand subexpression.
  this->op->destroy( this->op );

  // Then the UnaryExpr struct itself.
  free( this );
}


/** Construct a UnaryExpr representation and fill in the parts
    that are common to all UnaryExpr instances. */
static UnaryExpr *buildUnaryExpr( Expr *op)
{
  UnaryExpr *this = (UnaryExpr *) malloc( sizeof( UnaryExpr ) );
  this->destroy = destroyUnary;

  this->op = op;

  return this;
}


//////////////////////////////////////////////////////////////////////
// Binary expressions

/** Representation for an arbitrary binary operator.  The eval
    pointer decides what it computes. */
typedef struct {
  char *(*eval)( Expr *oper, Context *ctxt );
  void (*destroy)( Expr *oper );

  // Two operand expressions.
  Expr *op1, *op2;
} BinaryExpr;


/** Free memory for any type of BinaryExpr instance. */
static void destroyBinary( Expr *expr )
{
  BinaryExpr *this = (BinaryExpr *)expr;

  // Free our operand subexpressions.
  this->op1->destroy( this->op1 );
  this->op2->destroy( this->op2 );

  // Then the BinaryExpr struct itself.
  free( this );
}

/** Construct a BinaryExpr representation and fill in the parts
    that are common to all BinaryExpr instances. */
static BinaryExpr *buildBinaryExpr( Expr *op1, Expr *op2 )
{
  BinaryExpr *this = (BinaryExpr *) malloc( sizeof( BinaryExpr ) );
  this->destroy = destroyBinary;

  this->op1 = op1;
  this->op2 = op2;

  return this;
}


//////////////////////////////////////////////////////////////////////
// Trinary expressions

/** Representation for an arbitrary trinary operator.  The eval
    pointer decides what it computes. */
typedef struct {
  char *(*eval)( Expr *oper, Context *ctxt );
  void (*destroy)( Expr *oper );

  // Three operand expressions.
  Expr *op1, *op2, *op3;
} TrinaryExpr;


/** Free memory for any type of TrinaryExpr instance. */
static void destroyTrinary( Expr *expr )
{
  TrinaryExpr *this = (TrinaryExpr *)expr;

  // Free our operand subexpressions.
  this->op1->destroy( this->op1 );
  this->op2->destroy( this->op2 );
  this->op3->destroy( this->op3 );

  // Then the TrinaryExpr struct itself.
  free( this );
}

/** Construct a TrinaryExpr representation and fill in the parts
    that are common to all TrinaryExpr instances. */
static TrinaryExpr *buildTrinaryExpr( Expr *op1, Expr *op2, Expr *op3 )
{
  TrinaryExpr *this = (TrinaryExpr *) malloc( sizeof( TrinaryExpr ) );
  this->destroy = destroyTrinary;

  this->op1 = op1;
  this->op2 = op2;
  this->op3 = op3;

  return this;
}


/** For instances of VariableExpr that declare variables, this
    is the funciton they call for eval. */
static char *evalVariable( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  VariableExpr *this = (VariableExpr *)expr;

  // Evaluate our operand
  int len = strlen(this->op1);
  char *only = (char *)malloc( len + 1 );
  strcpy(only, this->op1);
  only[len] = '\0';

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  len = strlen(getVariable(ctxt, only));
  char *result = (char *)malloc( len + 1 );
  strcpy(result, getVariable(ctxt, only));
  result[len] = '\0';
  
  // We're done with the value returned by our subexpression.
  free( only ); 
  return result;
}


/** For instances of SetExpr that sets variables, this
    is the funciton they call for eval. */
static char *evalSet( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  SetExpr *this = (SetExpr *)expr;

  // Evaluate our two operands
  int len = strlen(this->op1);
  char *left = (char *)malloc( len + 1 );
  strcpy(left, this->op1);
  left[len] = '\0';
  
  char *right = this->op2->eval( this->op2, ctxt );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  len = strlen(right);
  char *result = (char *)malloc( len + 1 );
  strcpy(result, right);
  result[len] = '\0';
  
  setVariable(ctxt, left, right);
  
  // We're done with the values returned by our two subexpressions.
  free( left );
  free( right );
  
  return result;
}


/** For instances of BinaryExpr that do addition, this
    is the funciton they call for eval. */
static char *evalAdd( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Parse the left and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( left, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;

  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( left );
  free( right );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  sprintf( result, "%ld", a + b );
  return result;
}


/** For instances of BinaryExpr that do subtraction, this
    is the funciton they call for eval. */
static char *evalSub( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Parse the left and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( left, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;

  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( left );
  free( right );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  sprintf( result, "%ld", a - b );
  return result;
}


/** For instances of BinaryExpr that do multiplication, this
    is the funciton they call for eval. */
static char *evalMul( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Parse the left and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( left, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;

  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( left );
  free( right );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  sprintf( result, "%ld", a * b );
  return result;
}


/** For instances of BinaryExpr that do division, this
    is the funciton they call for eval. */
static char *evalDiv( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Parse the left and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( left, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;

  if (b == 0){
    fprintf(stderr, "Runtime Error: divide by zero\n");
    exit( EXIT_FAILURE );
  }
  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( left );
  free( right );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  sprintf( result, "%ld", a / b );
  return result;
}


/** For instances of BinaryExpr that check equivalency, this
    is the funciton they call for eval. */
static char *evalEqual( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );



  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  if(strcmp( left, right ) == 0){ 
    sprintf( result, "true");
  } else {
    result[0] = '\0';
  }
  
  // We're done with the two subexpressions
  free( left );
  free( right );
  
  return result;
}


/** For instances of BinaryExpr that check less than, this
    is the funciton they call for eval. */
static char *evalLess( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Parse the left and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( left, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;

  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( left );
  free( right );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  if(a < b){ 
    sprintf( result, "true");
  } else {
    result[0] = '\0';
  }
  return result;
}


/** For instances of BinaryExpr that check if, this
    is the funciton they call for eval. */
static char *evalIf( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  if(left[0] != '\0'){
    //If left is true then we evaluate right.
    char *right = this->op2->eval( this->op2, ctxt );
    free(right);
  }

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  int len = strlen(left);
  char *result = (char *)malloc( len + 1 );
  strcpy( result, left);
  result[len] = '\0';
  
  // We're done with the our left subexpression.
  free( left );
  
  return result;
}


/** For instances of BinaryExpr that check while, this
    is the funciton they call for eval. */
static char *evalWhile( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  
  //Declare count for times the body is evaluated.
  long count = 0;
  
  while(strcmp(left, "") != 0){
    //If left is true then we evaluate right.
    char *right = this->op2->eval( this->op2, ctxt );
    free(right);
    //We continually evaluate left until it is no longer true.
    left = this->op1->eval( this->op1, ctxt );
    count++;
  }
  
  // We're done with the our left subexpression.
  free( left );
  
  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  sprintf( result, "%ld", count);
  
  return result;
}


/** For instances of BinaryExpr that check and, this
    is the funciton they call for eval. */
static char *evalAnd( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = (char *)malloc( MAX_NUMBER + 1 );
  
  if(left[0] != '\0'){
    free(right);
    right = this->op2->eval( this->op2, ctxt );
  }
  
  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  if(left[0] != '\0' && right[0] != '\0'){
    sprintf(result, "true");
  } else {
    result[0] = '\0';
  }
  
  // We're done with the values returned by our two subexpressions.
  free( left );
  free( right );
  
  return result;
}


/** For instances of BinaryExpr that check or, this
    is the funciton they call for eval. */
static char *evalOr( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = (char *)malloc( MAX_NUMBER + 1 );
   
  if(left[0] == '\0'){   
    free(right);  
    right = this->op2->eval( this->op2, ctxt );
  }
  
  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  if(left[0] != '\0' || right[0] != '\0'){
    sprintf(result, "true");
  } else {
    result[0] = '\0';
  }
  
  // We're done with the values returned by our two subexpressions.
  free( left );
  free( right );
  
  return result;
}


/** For instances of BinaryExpr that do concatenation, this
    is the funciton they call for eval. */
static char *evalConcat( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  BinaryExpr *this = (BinaryExpr *)expr;

  // Evaluate our two operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *right = this->op2->eval( this->op2, ctxt );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  int len = strlen(left) + strlen(right);
  char *result = (char *)malloc( len + 1 );
  
  strcpy(result, left);
  strcat(result, right);
  result[len] = '\0';
  
  // We're done with the values returned by our two subexpressions.
  free( left );
  free( right );
  
  return result;
}


/** For instances of TrinaryExpr that create substrings, this
    is the funciton they call for eval. */
static char *evalSubstr( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  TrinaryExpr *this = (TrinaryExpr *)expr;

  // Evaluate our three operands
  char *left = this->op1->eval( this->op1, ctxt );
  char *middle = this->op2->eval( this->op2, ctxt );
  char *right = this->op3->eval( this->op3, ctxt );

  // Parse the middle and right operands as long ints.  Set them
  // to zero if they don't parse correctly.
  long a, b;
  if ( sscanf( middle, "%ld", &a ) != 1 )
    a = 0;

  if ( sscanf( right, "%ld", &b ) != 1 )
    b = 0;
  
  //If either index is negative, set it to zero.
  if(a < 0){
    a = 0;
  }
  if(b < 0){
    b = 0;
  }
  
  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  int len = b - a;
  if(len < 0){
    len = 0;
  }
  char *result = (char *)malloc( len + 1 );
  
  strncpy(result, left + a, len);
  result[len] = '\0';
  
  // We're done with the values returned by our two subexpressions.
  free( left );
  free( middle );
  free( right );
  
  return result;
}


/** For instances of UnaryExpr that check not, this
    is the funciton they call for eval. */
static char *evalNot( Expr *expr, Context *ctxt )
{
  // Get a pointer to the more specific type this function works with.
  UnaryExpr *this = (UnaryExpr *)expr;

  // Evaluate our operand
  char *only = this->op->eval( this->op, ctxt );

  // Compute the result, store it in a dynamically allocated string
  // and return it to the caller.
  char *result = (char *)malloc( MAX_NUMBER + 1 );
  if(only[0] == '\0'){ 
    sprintf( result, "true");
  } else {
    result[0] = '\0';
  }
  
  // We're done with the values returned by our two subexpressions,
  // We just needed to get them as long ints.
  free( only );
  
  return result;
}


Expr *makeAdd( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do adding.
  this->eval = evalAdd;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeSub( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do subtracting.
  this->eval = evalSub;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeMul( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do multiplication.
  this->eval = evalMul;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeDiv( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do division.
  this->eval = evalDiv;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeEqual( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to check equivalency.
  this->eval = evalEqual;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeLess( Expr *op1, Expr *op2 )
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do check less than.
  this->eval = evalLess;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeNot( Expr *op)
{
  // Get in a generic instance of UnaryExpr
  UnaryExpr *this = buildUnaryExpr( op );

  // Fill in our function to do check less than.
  this->eval = evalNot;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeVariable( char const *name)
{
  // Get in a generic instance of VariableExpr
  VariableExpr *this = buildVariableExpr( name );

  // Fill in our function to do check while.
  this->eval = evalVariable;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeSet( char const *name, Expr *expr)
{
  // Get in a generic instance of SetExpr  
  SetExpr *this = buildSetExpr( name, expr );
  
  // Fill in our function to do check while.
  this->eval = evalSet;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeIf( Expr *cond, Expr *body)
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( cond, body );

  // Fill in our function to do check less than.
  this->eval = evalIf;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeWhile( Expr *cond, Expr *body)
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( cond, body );

  // Fill in our function to do check while.
  this->eval = evalWhile;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeAnd( Expr *op1, Expr *op2)
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do check and.
  this->eval = evalAnd;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeOr( Expr *op1, Expr *op2)
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do check or.
  this->eval = evalOr;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeConcat( Expr *op1, Expr *op2)
{
  // Get in a generic instance of BinaryExpr
  BinaryExpr *this = buildBinaryExpr( op1, op2 );

  // Fill in our function to do concatenation.
  this->eval = evalConcat;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}


Expr *makeSubstr( Expr *op1, Expr *op2, Expr *op3)
{
  // Get in a generic instance of TrinaryExpr
  TrinaryExpr *this = buildTrinaryExpr( op1, op2, op3 );

  // Fill in our function to create substring.
  this->eval = evalSubstr;

  // Return the instance as if it's an Expr (which it sort of is)
  return (Expr *) this;
}