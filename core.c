#include "core.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// Maximum variable length
#define MAX_VAR 20

// ASCII Values for invalid variable characters
#define LEFT_BRACKET 123
#define RIGHT_BRACKET 125
#define POUND 35

// Node for building our linked list.
struct NodeTag {
  
  // Name of the node.
  char* name;
  // Value in this node.
  char* value;

  // Pointer to the next node.
  struct NodeTag *next;
};

// A short type name to use for a node.
typedef struct NodeTag Node;

Node *makeList(){
  Node *n = (Node *)malloc( sizeof( Node ) );
  n->name = (char *) malloc( sizeof( char ) );
  n->value = (char *) malloc( sizeof( char ) );
  n->name[0] = '\0';
  n->value[0] = '\0';
  n->next = NULL;
  return n;
}


//////////////////////////////////////////////////////////////////////
// Context

struct ContextTag {
  // Head of the linked list.
  Node *head; 
};

Context *makeContext()
{
   // Get in a generic instance of Context
  Context *this = (Context *) malloc( sizeof( Context ) );
  // Fill in our nodes with null.
  this->head = makeList();
  // Return the context
  return (Context *) this;
}

  
char const *getVariable( Context *ctxt, char const *name )
{
  Node *m = ctxt->head;
  while(m != NULL && strcmp(m->name, name) != 0){
      m = m->next;
  } 
  if(m == NULL){
    // If the variable is not in the list, then make a new node.
    Node *n = (Node *)malloc( sizeof( Node ) );

    // Save fields.
    int len = strlen( name );
    n->name = (char *) malloc( len + 1 );
    strcpy(n->name, name);
    n->name[len] = '\0';
    n->value = (char *) malloc( 1 );
    n->value[0] = '\0';
    // Link it to the start of the list.
    n->next = ctxt->head;
    ctxt->head = n;

    // Return empty string.
    return n->value;
    
  } else {
    
    // If the variable is in the list, return its value.
    return m->value;
    
  }
}
 

void setVariable( Context *ctxt, char const *name, char *value )
{
  Node *m = ctxt->head;
  while(m != NULL && strcmp(m->name, name) != 0){
    m = m->next;
  }
  
  if(m == NULL){
    // If the variable is not in the list, then make a new node.
    Node *n = (Node *)malloc( sizeof( Node ) );

    // Save fields.
    int len = strlen( name );
    n->name = (char *) malloc( len + 1 );
    strcpy(n->name, name);
    n->name[len] = '\0';
    len = strlen( value );
    n->value = (char *) malloc( len + 1 );
    strcpy(n->value, value);
    n->value[len] = '\0';

    // Link it to the start of the list.
    n->next = ctxt->head;
    ctxt->head = n;
    
  } else {
    free(m->value);
  
    int len = strlen( value );
    m->value = (char *) malloc( len + 1 );
    strcpy(m->value, value);
    m->value[len] = '\0';
  }
}

void freeContext( Context *ctxt )
{
  Node *m = ctxt->head;
  while(m != NULL){
    ctxt->head = ctxt->head->next;
    free(m->name);
    free(m->value);
    free(m);
    m = ctxt->head;
  }
  free(ctxt);
}

//////////////////////////////////////////////////////////////////////
// Input tokenization

// Current line we're parsing, starting from 1 like most editors.
static int lineCount = 1;

bool nextToken( char *token, FILE *fp )
{
  int ch;

  // Skip whitespace and comments.
  while ( isspace( ch = fgetc( fp ) ) || ch == '#' ) {
    // If we hit the comment characer, skip the whole line.
    if ( ch == '#' )
      while ( ( ch = fgetc( fp ) ) != EOF && ch != '\n' )
        ;

    if ( ch == '\n' )
      lineCount++;
  }
    
  if ( ch == EOF )
    return false;

  // Record the character we'ver read and keep up with the token length.
  int len = 0;
  token[ len++ ] = ch;

  // Handle punctuation.
  if ( ch == '{' || ch == '}' ) {
    token[ len++ ] = '\0';
    return true;
  }

  // Handle non-quoted words.
  if ( ch != '"' ) {
    while ( ( ch = fgetc( fp ) ) != EOF && !isspace( ch ) &&
            ch != '{' && ch != '}' && ch != '"' && ch != '#' ) {
      // Complain if the token is too long.
      if ( len >= MAX_TOKEN ) {
        fprintf( stderr, "line %d: token too long\n", linesRead() );
        exit( EXIT_FAILURE );
      }

      token[ len++ ] = ch;
    }

    // We had to read one character too far to find the end of the token.
    // put it back.
    if ( ch != EOF )
      ungetc( ch, fp );

    token[ len++ ] = '\0';
    return true;
  }

  // Most interesting case, handle strings.

  // Is the next character escaped.
  bool escape = false;

  // Keep reading until we hit the matching close quote.
  while ( ( ch = fgetc( fp ) ) != '"' || escape ) {
    // Error conditions
    if ( ch == EOF || ch == '\n' ) {
      fprintf( stderr, "line %d: %s while reading parsing string literal.\n",
               linesRead(), ch == EOF ? "EOF" : "newline" );
      exit( EXIT_FAILURE );
    }
      
    // On a backslash, we just enable escape mode.
    if ( !escape && ch == '\\' ) {
      escape = true;
    } else {
      // Interpret escape sequences if we're in escape mode.
      if ( escape ) {
        switch ( ch ) {
        case 'n':
          ch = '\n';
          break;
        case 't':
          ch = '\t';
          break;
        case '"':
          ch = '"';
          break;
        case '\\':
          ch = '\\';
          break;
        default:
          fprintf( stderr, "line %d: Invalid escape sequence \"\\%c\"\n",
                   linesRead(), ch );
          exit( EXIT_FAILURE );
        }
        escape = false;
      }

      // Complain if this string, with the eventual close quote, is too long.
      if ( len + 1 >= MAX_TOKEN ) {
        fprintf( stderr, "line %d: token too long\n", linesRead() );
        exit( EXIT_FAILURE );
      }
      token[ len++ ] = ch;
    }
  }
    
  // Store the closing quote and the terminator and return.
  token[ len++ ] = '"';
  token[ len++ ] = '\0';
  return token;
}

int linesRead()
{
  return lineCount;
}

