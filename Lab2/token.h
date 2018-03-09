#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType {
  word,
  input_redirection,  //"<"
  output_redirection, //">"
  daemon_ampersand,   //"&"
  pipe_bar,           //"|"
  newline             //"\\\n"
} TokenType;

//TODO: fix this enum
typedef enum ParserTokenType {
  simple_list,
  pipeline,
  pipeline2,
  simple_command,
  simple_command_element,
  redirection,
  newline_list
} ParserTokenType;


typedef struct Token {
  char *token_string;
  TokenType token_type;
} Token;

typedef struct TokenList {
  Token token;
  struct TokenList *next;
} TokenList;

typedef enum ParserTokenType {

} ParserTokenType;

typedef struct ParserTokenList ParserTokenList;

typedef struct ParserToken { //consists of a TokenList and/or a ParserTokenList.
  int status; //or something to differentiate between ParserTokens that only have a token list and ParserTokens that are (also) composed of other ParserTokens.
  TokenList *token_list;
  ParserTokenList *parser_token_list;
  ParserTokenType parser_token_type;
} ParserToken;



#endif
