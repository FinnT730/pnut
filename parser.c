#ifndef SIX_CC

#include <stdio.h>
#include <stdlib.h>
typedef FILE *FILE_ptr;
typedef int *int_ptr;
typedef char *char_ptr;
typedef void *void_ptr;

#endif

#define ast int
#define true 1
#define false 0

#define AVOID_AMPAMP_BARBAR
#define USE_IN_RANGE_FUNCTION_not
#define INLINE_get_ch

#ifdef AVOID_AMPAMP_BARBAR
#define AND &
#define OR |
#else
#define AND &&
#define OR ||
#endif

#ifdef USE_IN_RANGE_FUNCTION
int in_range(int x, int lo, int hi) {
  if (x < lo) return 0;
  if (x > hi) return 0;
  return 1;
}
#else
#define in_range(x, lo, hi) ((x >= lo) AND (x <= hi))
#endif

/* Redefining strcmp because it's not part of the Shell runtime */
int strcmp(char_ptr str1, char_ptr str2) {
  int i = 0;
  while (str1[i] == str2[i]) {
    if (str1[i] == '\0') return 0;
    i += 1;
  }
  return str1[i] - str2[i];
}

int AUTO_KW        = 300;
int BREAK_KW       = 301;
int CASE_KW        = 302;
int CHAR_KW        = 303;
int CONST_KW       = 304;
int CONTINUE_KW    = 305;
int DEFAULT_KW     = 306;
int DEFINE_KW      = 307;
int DO_KW          = 308;
int DOUBLE_KW      = 309;
int ELSE_KW        = 310;
int ENDIF_KW       = 311;
int ENUM_KW        = 312;
int ERROR_KW       = 313;
int EXTERN_KW      = 314;
int FLOAT_KW       = 315;
int FOR_KW         = 316;
int GOTO_KW        = 317;
int IF_KW          = 318;
int IFDEF_KW       = 319;
int IFNDEF_KW      = 320;
int INCLUDE_KW     = 321;
int INT_KW         = 322;
int LONG_KW        = 323;
int REGISTER_KW    = 324;
int RETURN_KW      = 325;
int SHORT_KW       = 326;
int SIGNED_KW      = 327;
int SIZEOF_KW      = 328;
int STATIC_KW      = 329;
int STRUCT_KW      = 330;
int SWITCH_KW      = 331;
int TYPEDEF_KW     = 332;
int UNDEF_KW       = 333;
int UNION_KW       = 334;
int UNSIGNED_KW    = 335;
int VOID_KW        = 336;
int VOLATILE_KW    = 337;
int WHILE_KW       = 338;
int VAR_DECL       = 339;
int FUN_DECL       = 340;

int IDENTIFIER = 400;
int INTEGER    = 401;
int CHARACTER  = 402;
int STRING     = 403;

int AMP_AMP    = 404;
int AMP_EQ     = 405;
int ARROW      = 406;
int BAR_BAR    = 407;
int BAR_EQ     = 408;
int CARET_EQ   = 409;
int EQ_EQ      = 410;
int GT_EQ      = 411;
int LSHIFT_EQ  = 412;
int LSHIFT     = 413;
int LT_EQ      = 414;
int MINUS_EQ   = 415;
int MINUS_MINUS= 416;
int EXCL_EQ    = 417;
int PERCENT_EQ = 418;
int PLUS_EQ    = 419;
int PLUS_PLUS  = 420;
int RSHIFT_EQ  = 421;
int RSHIFT     = 422;
int SLASH_EQ   = 423;
int STAR_EQ    = 424;

int TEXT_TREE = 425;
int TEXT_INTEGER = 426;
int TEXT_CHAR = 427;
int TEXT_STRING = 428;
int TEXT_SUBSTRING = 429;

int IDENTIFIER_INTERNAL = 430;
int IDENTIFIER_STRING = 431;
int IDENTIFIER_DOLLAR = 432;
int IDENTIFIER_EMPTY = 433;
int LOCAL_VAR = 434;
int KIND_LOCAL = 435;
int KIND_PARAM = 436;

void fatal_error(char_ptr msg) {
  printf("%s\n", msg);
  exit(1);
}

/* tokenizer */

int ch;
int tok;
int val;

#define STRING_POOL_SIZE 100000
char string_pool[STRING_POOL_SIZE];
int string_pool_alloc = 0;
int string_start;
int hash;

/* These parameters give a perfect hashing of the C keywords */
#define HASH_PARAM 12443
#define HASH_PRIME 103
#define HEAP_SIZE 100000 /* MUST BE > HASH_PRIME */
int heap[HEAP_SIZE];
int heap_alloc = HASH_PRIME;

int alloc_result;

int alloc_obj(int size) {

  alloc_result = heap_alloc;

  heap_alloc += size;

  if (heap_alloc > HEAP_SIZE) {
    fatal_error("heap overflow");
  }

  return alloc_result;
}

void begin_string() {
  string_start = string_pool_alloc;
  hash = 0;
}

void accum_string() {
  hash = (ch + (hash ^ HASH_PARAM)) % HASH_PRIME;
  string_pool[string_pool_alloc] = ch;
  string_pool_alloc += 1;
  if (string_pool_alloc >= STRING_POOL_SIZE) {
    fatal_error("string pool overflow");
  }
}

int probe;
int probe_start;
int c1;
int c2;
int end_ident_i;

int end_ident() {
  string_pool[string_pool_alloc] = 0; /* terminate string */
  string_pool_alloc += 1; /* account for terminator */

  probe = heap[hash];

  while (probe != 0) {
    probe_start = heap[probe+1];
    end_ident_i = 0;
    c1 = string_pool[string_start+end_ident_i];
    c2 = string_pool[probe_start+end_ident_i];
    while (c1 == c2) {
      if (c1 == 0) {
        string_pool_alloc = string_start; /* undo string allocation */
        return probe;
      }
      end_ident_i += 1;
      c1 = string_pool[string_start+end_ident_i];
      c2 = string_pool[probe_start+end_ident_i];
    }
    hash = probe; /* remember previous ident */
    probe = heap[probe];
  }

  /* a new ident has been found */

  probe = alloc_obj(3);

  heap[hash] = probe; /* add new ident at end of chain */

  heap[probe] = 0; /* no next ident */
  heap[probe+1] = string_start;
  heap[probe+2] = IDENTIFIER;

  return probe;
}

#ifdef INLINE_get_ch

#define get_ch() ch = getchar()

#else

void get_ch() {
  ch = getchar();
}

#endif

void handle_preprocessor_directive() {
  /*TODO*/
  while ((ch != '\n') AND (ch != EOF)) {
    printf("%c", ch);
    get_ch();
  }
  printf(" \n");
}

void get_ident() {

  begin_string();

  while (in_range(ch, 'A', 'Z') OR
         in_range(ch, 'a', 'z') OR
         in_range(ch, '0', '9') OR
         (ch == '_')) {
    accum_string();
    get_ch();
  }

  val = end_ident();
  tok = heap[val+2];
  /*
  printf("tok=%d val=%d %s\n", tok, val, string_pool + heap[val+1]);
  */
}

void init_kw(int tok, char_ptr name) {

  int i = 0;

  begin_string();

  while (name[i] != 0) {
    ch = name[i];
    accum_string();
    i += 1;
  }

  heap[end_ident()+2] = tok;
}

void init_ident_table() {

  int i;

  for (i=0; i<HASH_PRIME; i += 1) {
    heap[i] = 0;
  }

  init_kw(AUTO_KW,     "auto");
  init_kw(BREAK_KW,    "break");
  init_kw(CASE_KW,     "case");
  init_kw(CHAR_KW,     "char");
  init_kw(CONST_KW,    "const");
  init_kw(CONTINUE_KW, "continue");
  init_kw(DEFAULT_KW,  "default");
  init_kw(DEFINE_KW,   "define");
  init_kw(DO_KW,       "do");
  init_kw(DOUBLE_KW,   "double");
  init_kw(ELSE_KW,     "else");
  init_kw(ENDIF_KW,    "endif");
  init_kw(ENUM_KW,     "enum");
  init_kw(ERROR_KW,    "error");
  init_kw(EXTERN_KW,   "extern");
  init_kw(FLOAT_KW,    "float");
  init_kw(FOR_KW,      "for");
  init_kw(GOTO_KW,     "goto");
  init_kw(IF_KW,       "if");
  init_kw(IFDEF_KW,    "ifdef");
  init_kw(IFNDEF_KW,   "ifndef");
  init_kw(INCLUDE_KW,  "include");
  init_kw(INT_KW,      "int");
  init_kw(LONG_KW,     "long");
  init_kw(REGISTER_KW, "register");
  init_kw(RETURN_KW,   "return");
  init_kw(SHORT_KW,    "short");
  init_kw(SIGNED_KW,   "signed");
  init_kw(SIZEOF_KW,   "sizeof");
  init_kw(STATIC_KW,   "static");
  init_kw(STRUCT_KW,   "struct");
  init_kw(SWITCH_KW,   "switch");
  init_kw(TYPEDEF_KW,  "typedef");
  init_kw(UNDEF_KW,    "undef");
  init_kw(UNION_KW,    "union");
  init_kw(UNSIGNED_KW, "unsigned");
  init_kw(VOID_KW,     "void");
  init_kw(VOLATILE_KW, "volatile");
  init_kw(WHILE_KW,    "while");
}

int accum_digit(int base) {
  int digit = 99;
  int MININT = -2147483648;
  int limit;
  if (in_range(ch, '0', '9')) {
    digit = ch - '0';
  } else if (in_range(ch, 'A', 'Z')) {
    digit = ch - 'A' + 10;
  } else if (in_range(ch, 'a', 'z')) {
    digit = ch - 'a' + 10;
  }
  if (digit >= base) {
    return 0; /* character is not a digit in that base */
  } else {
    limit = MININT / base;
    if ((val < limit) OR ((val == limit) AND (digit > limit * base - MININT))) {
      fatal_error("literal integer overflow");
    } else {
      val = val * base - digit;
      get_ch();
    }
    return 1;
  }
}

void get_string_char() {

  val = ch;
  get_ch();

  if (val == '\\') {
    if (in_range(ch, '0', '7')) {
      /*
      Parse octal character, up to 3 digits.
      Note that \1111 is parsed as '\111' followed by '1'
      See https://en.wikipedia.org/wiki/Escape_sequences_in_C#Notes
      */
      val = 0;
      accum_digit(8);
      accum_digit(8);
      accum_digit(8);
      val = -(val % 256); /* keep low 8 bits, without overflowing */
    } else if ((ch == 'x') OR (ch == 'X')) {
      get_ch();
      val = 0;
      /* Allow 1 or 2 hex digits. */
      if (accum_digit(16)) {
        accum_digit(16);
      } else {
        fatal_error("invalid hex escape -- it must have at least one digit");
      }
      val = -(val % 256); /* keep low 8 bits, without overflowing */
    } else {
      if (ch == 'a') {
        val = 7;
      } else if (ch == 'b') {
        val = 8;
      } else if (ch == 'f') {
        val = 12;
      } else if (ch == 'n') {
        val = 10;
      } else if (ch == 'r') {
        val = 13;
      } else if (ch == 't') {
        val = 9;
      } else if (ch == 'v') {
        val = 11;
      } else if ((ch == '\\') OR (ch == '\'') OR (ch == '\"')) {
        val = ch;
      } else {
        fatal_error("unimplemented string character escape");
      }
      get_ch();
    }
  }
}

void get_tok() {

  while (1) {

    if (ch <= ' ') {

      if (ch == EOF) {
        tok = EOF;
        break;
      }

      /* skip whitespace, detecting when it is at start of line */

      if (ch == '\n') tok = ch;
      get_ch();

      while (in_range(ch, 0, ' ')) {
        if (ch == '\n') tok = ch;
        get_ch();
      }

      /* detect '#' at start of line, possibly preceded by whitespace */

      if ((tok == '\n') AND (ch == '#'))
        handle_preprocessor_directive();

      /* will continue while (1) loop */

    } else if (in_range(ch, 'a', 'z') OR
               in_range(ch, 'A', 'Z') OR
               (ch == '_')) {

      get_ident();

      break;

    } else if (in_range(ch, '0', '9')) {

      val = '0' - ch;

      get_ch();

      if (val == 0) { /* val == 0 <=> ch == '0' */
        if ((ch == 'x') OR (ch == 'X')) {
          get_ch();
          val = 0;
          if (accum_digit(16)) {
            while (accum_digit(16)) ch = ch; /* dummy op */
          } else {
            fatal_error("invalid hex integer -- it must have at least one digit");
          }
        } else {
          while (accum_digit(8)) ch = ch; /* dummy op */
        }
      } else {
        while (accum_digit(10)) ch = ch; /* dummy op */
      }

      tok = INTEGER;

      break;

    } else if (ch == '\'') {

      get_ch();
      get_string_char();

      if (ch != '\'') {
        fatal_error("unterminated character literal");
      }

      get_ch();

      tok = CHARACTER;

      break;

    } else if (ch == '\"') {

      get_ch();

      begin_string();

      while ((ch != '\"') AND (ch != EOF)) {
        get_string_char();
        tok = ch;
        ch = val;
        accum_string();
        ch = tok;
      }

      if (ch != '\"') {
        fatal_error("unterminated string literal");
      }

      ch = 0;
      accum_string();

      get_ch();

      val = string_start;
      tok = STRING;

      break;

    } else {

      tok = ch; /* fallback for single char tokens */

      if (ch == '/') {

        get_ch();
        if (ch == '*') {
          get_ch();
          tok = ch; /* remember previous char, except first one */
          while (((tok != '*') OR (ch != '/')) AND (ch != EOF)) {
            tok = ch;
            get_ch();
          }
          if (ch == EOF) {
            fatal_error("unterminated comment");
          }
          get_ch();
          /* will continue while (1) loop */
        } else if (ch == '/') {
          while ((ch != '\n') AND (ch != EOF)) {
            get_ch();
          }
          /* will continue while (1) loop */
        } else {
          if (ch == '=') {
            get_ch();
            tok = SLASH_EQ;
          }
          break;
        }

      } else if (ch == '&') {

        get_ch();
        if (ch == '&') {
          get_ch();
          tok = AMP_AMP;
        } else if (ch == '=') {
          get_ch();
          tok = AMP_EQ;
        }

        break;

      } else if (ch == '|') {

        get_ch();
        if (ch == '|') {
          get_ch();
          tok = BAR_BAR;
        } else if (ch == '=') {
          get_ch();
          tok = BAR_EQ;
        }

        break;

      } else if (ch == '<') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = LT_EQ;
        } else if (ch == '<') {
          get_ch();
          if (ch == '=') {
            get_ch();
            tok = LSHIFT_EQ;
          } else {
            tok = LSHIFT;
          }
        }

        break;

      } else if (ch == '>') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = GT_EQ;
        } else if (ch == '>') {
          get_ch();
          if (ch == '=') {
            get_ch();
            tok = RSHIFT_EQ;
          } else {
            tok = RSHIFT;
          }
        }

        break;

      } else if (ch == '=') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = EQ_EQ;
        }

        break;

      } else if (ch == '!') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = EXCL_EQ;
        }

        break;

      } else if (ch == '+') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = PLUS_EQ;
        } else if (ch == '+') {
          get_ch();
          tok = PLUS_PLUS;
        }

        break;

      } else if (ch == '-') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = MINUS_EQ;
        } else if (ch == '>') {
          get_ch();
          tok = ARROW;
        } else if (ch == '-') {
          get_ch();
          tok = MINUS_MINUS;
        }

        break;

      } else if (ch == '*') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = STAR_EQ;
        }

        break;

      } else if (ch == '%') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = PERCENT_EQ;
        }

        break;

      } else if (ch == '^') {

        get_ch();
        if (ch == '=') {
          get_ch();
          tok = CARET_EQ;
        }

        break;

      } else if ((ch == '~') OR (ch == '.') OR (ch == '?') OR (ch == ',') OR (ch == ':') OR (ch == ';') OR (ch == '(') OR (ch == ')') OR (ch == '[') OR (ch == ']') OR (ch == '{') OR (ch == '}')) {

        tok = ch;

        get_ch();

        break;

      } else {
        printf("ch=%c\n", ch);
        fatal_error("invalid token");
      }
    }
  }
}

/* parser */

int get_op(ast node) {
  return heap[node] & 1023;
}

ast get_nb_children(ast node) {
  return heap[node] >> 10;
}

int get_val(ast node) {
  return heap[node+1];
}

void set_val(ast node, int val) {
  heap[node+1] = val;
}

ast get_child(ast node, int i) {
  return heap[node+i+1];
}

void set_child(ast node, int i, ast child) {
  heap[node+i+1] = child;
}

ast ast_result;

ast new_ast0(int op, int val) {

  ast_result = alloc_obj(2);

  heap[ast_result] = op;
  set_val(ast_result, val);

  return ast_result;
}

ast new_ast1(int op, ast child0) {

  ast_result = alloc_obj(2);

  heap[ast_result] = op + 1024;
  set_child(ast_result, 0, child0);

  return ast_result;
}

ast new_ast2(int op, ast child0, ast child1) {

  ast_result = alloc_obj(3);

  heap[ast_result] = op + 2048;
  set_child(ast_result, 0, child0);
  set_child(ast_result, 1, child1);

  return ast_result;
}

ast new_ast3(int op, ast child0, ast child1, ast child2) {

  ast_result = alloc_obj(4);

  heap[ast_result] = op + 3072;
  set_child(ast_result, 0, child0);
  set_child(ast_result, 1, child1);
  set_child(ast_result, 2, child2);

  return ast_result;
}

ast new_ast4(int op, ast child0, ast child1, ast child2, ast child3) {

  ast_result = alloc_obj(5);

  heap[ast_result] = op + 4096;
  set_child(ast_result, 0, child0);
  set_child(ast_result, 1, child1);
  set_child(ast_result, 2, child2);
  set_child(ast_result, 3, child3);

  return ast_result;
}

void syntax_error(char_ptr msg) {
  printf("syntax error: %s\n", msg);
  fatal_error("syntax error");
}

void missing_feature_error(char_ptr msg) {
  printf("not yet implemented: %s\n", msg);
  fatal_error("syntax error");
}

void expect_tok(int expected_tok) {
  if (tok != expected_tok) {
    syntax_error("unexpected token");
  }
  get_tok();
}

#ifndef SIX_CC
ast parse_comma_expression();
ast parse_cast_expression();
ast parse_compound_statement();
ast parse_conditional_expression();
#endif

ast parse_type() {

  int type_kw = 0;

  while (1) {
    if ((tok == INT_KW) OR (tok == CHAR_KW) OR (tok == SHORT_KW) OR (tok == LONG_KW) OR (tok == SIGNED_KW)) {
      if ((type_kw != 0) AND (type_kw != INT_KW)) {
        syntax_error("inconsistent type");
      } else {
        type_kw = INT_KW;
        get_tok();
      }
    } else if ((tok == UNSIGNED_KW) OR (tok == FLOAT_KW) OR (tok == DOUBLE_KW)) {
      syntax_error("unsupported type");
    } else if (tok == VOID_KW) {
      if (type_kw != 0) {
        syntax_error("inconsistent type");
      } else {
        type_kw = VOID_KW;
        get_tok();
      }
    } else {
      break;
    }
  }

  if (type_kw == 0) {
    syntax_error("type expected");
  }

  return new_ast0(type_kw, 0);
}

int parse_stars() {

  int stars = 0;

  while (tok == '*') {
    stars += 1;
    get_tok();
  }

  return stars;
}

int is_type_starter(int tok) {
  return (tok == INT_KW) OR (tok == CHAR_KW) OR (tok == SHORT_KW) OR (tok == LONG_KW) OR (tok == SIGNED_KW) OR (tok == UNSIGNED_KW) OR (tok == FLOAT_KW) OR (tok == DOUBLE_KW) OR (tok == VOID_KW);
}

ast parse_declaration() {
  ast type;
  int stars;
  int name;
  ast this_type;
  ast result = 0;

  if (is_type_starter(tok)) {

    type = parse_type();
    stars = parse_stars();

    if (stars != 0)
      this_type = new_ast0(get_op(type), stars);

    name = val;

    expect_tok(IDENTIFIER);

    if ((stars == 0) AND (get_op(type) == VOID_KW))
      syntax_error("variable with void type");

    if (tok == '[')
      syntax_error("array declaration only allowed at global level");


    result = new_ast3(VAR_DECL, name, this_type, 0);
  }

  return result;
}

int parse_declaration_list() {
  ast decl = parse_declaration(true);
  ast result = 0;
  ast tail;
  if (decl != 0) {
    result = new_ast2(',', decl, 0);
    tail = result;

    while (tok == ',') {
      get_tok();
      decl = parse_declaration(true);
      if (decl == 0) { break; }

      decl = new_ast2(',', decl, 0);
      set_child(tail, 1, decl);
      tail = decl;
    }
  }

  return result;
}

/* Note: Uses a simplified syntax for definitions */
ast parse_definition(int local) {

  ast type;
  int stars;
  ast init;
  int name;
  ast params;
  ast body;
  ast this_type;
  ast result = 0;

  if (is_type_starter(tok)) {

    type = parse_type();

    while (1) {

      stars = parse_stars();

      this_type = type;
      if (stars != 0) {
        this_type = new_ast0(get_op(type), stars);
      }

      name = val;

      expect_tok(IDENTIFIER);

      if (tok == '(') {

        if (local) {
          syntax_error("function declaration only allowed at global level");
        }

        get_tok();

        params = parse_declaration_list();

        expect_tok(')');

        if (tok == ';') {
          /* forward declaration */
          body = 0;
          get_tok();
        } else {
          body = parse_compound_statement();
        }

        return new_ast4(FUN_DECL, name, this_type, params, body);

      } else {

        if ((stars == 0) AND (get_op(type) == VOID_KW)) {
          syntax_error("variable with void type");
        }

        if (tok == '[') {
          if (local) {
            syntax_error("array declaration only allowed at global level");
          }

          get_tok();
          if (tok == INTEGER) {
            this_type = new_ast2('[', new_ast0(INTEGER, -val), this_type);
            get_tok();
          } else {
            syntax_error("array size must be an integer constant");
          }

          expect_tok(']');
        }

        init = 0;

        if (tok == '=') {
          get_tok();
          init = parse_conditional_expression();
        }

        result = new_ast3(VAR_DECL, name, this_type, init);

        if (tok == ';') {
          get_tok();
          break;
        } else if (tok == ',') {
          get_tok();
          break;
        } else {
          syntax_error("';' or ',' expected");
        }
      }
    }
  }
  return result;
}

ast parse_parenthesized_expression() {

  ast result;

  expect_tok('(');

  result = parse_comma_expression();

  expect_tok(')');

  return result;
}

ast parse_primary_expression() {

  ast result;

  if (tok == IDENTIFIER) {

    result = new_ast0(IDENTIFIER, val);
    get_tok();

  } else if (tok == INTEGER) {

    result = new_ast0(INTEGER, val);
    get_tok();

  } else if (tok == CHARACTER) {

    result = new_ast0(CHARACTER, val);
    get_tok();

  } else if (tok == STRING) {

    result = new_ast0(STRING, val);
    get_tok();
    /*TODO: contiguous strings*/

  } else if (tok == '(') {

    result = parse_parenthesized_expression();

  } else {
    syntax_error("identifier, literal, or '(' expected");
  }

  return result;
}

ast parse_postfix_expression() {

  ast result;
  ast child;

  result = parse_primary_expression();

  while (1) {
    if (tok == '[') {

      get_tok();
      child = parse_comma_expression();
      result = new_ast2('[', result, child);
      expect_tok(']');

    } else if (tok == '(') {

      get_tok();
      if (tok == ')') {
        child = 0;
      } else {
        child = parse_comma_expression();
      }
      result = new_ast2('(', result, child);
      expect_tok(')');

    } else if (tok == '.') {

      syntax_error("Struct/Union not supported");

    } else if (tok == ARROW) {

      syntax_error("Struct/Union not supported");

    } else if ((tok == PLUS_PLUS) OR (tok == MINUS_MINUS)) {

      syntax_error("++/-- not supported");

    } else {
      break;
    }
  }

  return result;
}

ast parse_unary_expression() {

  ast result;
  int op;

  if ((tok == PLUS_PLUS) OR (tok == MINUS_MINUS)) {

    op = tok;
    get_tok();
    result = parse_unary_expression();
    result = new_ast1(op, result);

  } else if ((tok == '&') OR (tok == '*') OR (tok == '+') OR (tok == '-') OR (tok == '~') OR (tok == '!')) {

    op = tok;
    get_tok();
    result = parse_cast_expression();
    result = new_ast1(op, result);

  } else if (tok == SIZEOF_KW) {

    syntax_error("sizeof not supported");

  } else {
    result = parse_postfix_expression();
  }

  return result;
}

ast parse_cast_expression() {
  return parse_unary_expression();
}

ast parse_multiplicative_expression() {

  ast result = parse_cast_expression();
  ast child;
  int op;

  while ((tok == '*') OR (tok == '/') OR (tok == '%')) {

    op = tok;
    get_tok();
    child = parse_cast_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_additive_expression() {

  ast result = parse_multiplicative_expression();
  ast child;
  int op;

  while ((tok == '+') OR (tok == '-')) {

    op = tok;
    get_tok();
    child = parse_multiplicative_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_shift_expression() {

  ast result = parse_additive_expression();
  ast child;
  int op;

  while ((tok == LSHIFT) OR (tok == RSHIFT)) {

    op = tok;
    get_tok();
    child = parse_additive_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_relational_expression() {

  ast result = parse_shift_expression();
  ast child;
  int op;

  while ((tok == '<') OR (tok == '>') OR (tok == LT_EQ) OR (tok == GT_EQ)) {

    op = tok;
    get_tok();
    child = parse_shift_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_equality_expression() {

  ast result = parse_relational_expression();
  ast child;
  int op;

  while ((tok == EQ_EQ) OR (tok == EXCL_EQ)) {

    op = tok;
    get_tok();
    child = parse_relational_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_AND_expression() {

  ast result = parse_equality_expression();
  ast child;

  while (tok == '&') {

    get_tok();
    child = parse_equality_expression();
    result = new_ast2('&', result, child);

  }

  return result;
}

ast parse_exclusive_OR_expression() {

  ast result = parse_AND_expression();
  ast child;

  while (tok == '^') {

    get_tok();
    child = parse_AND_expression();
    result = new_ast2('^', result, child);

  }

  return result;
}


ast parse_inclusive_OR_expression() {

  ast result = parse_exclusive_OR_expression();
  ast child;

  while (tok == '|') {

    get_tok();
    child = parse_exclusive_OR_expression();
    result = new_ast2('|', result, child);

  }

  return result;
}


ast parse_logical_AND_expression() {

  ast result = parse_inclusive_OR_expression();
  ast child;

  while (tok == AMP_AMP) {

    get_tok();
    child = parse_inclusive_OR_expression();
    result = new_ast2(AMP_AMP, result, child);

  }

  return result;
}

ast parse_logical_OR_expression() {

  ast result = parse_logical_AND_expression();
  ast child;

  while (tok == BAR_BAR) {

    get_tok();
    child = parse_logical_AND_expression();
    result = new_ast2(BAR_BAR, result, child);

  }

  return result;
}

ast parse_conditional_expression() {

  ast result = parse_logical_OR_expression();
  ast child1;
  ast child2;

  if (tok == '?') {

    get_tok();
    child1 = parse_comma_expression();
    expect_tok(':');
    child2 = parse_conditional_expression();
    result = new_ast3('?', result, child1, child2);

  }

  return result;
}

ast parse_assignment_expression() {

  ast result = parse_conditional_expression();
  ast child;
  int op;

  if ((tok == '=') OR (tok == PLUS_EQ) OR (tok == MINUS_EQ) OR (tok == STAR_EQ) OR (tok == SLASH_EQ) OR (tok == PERCENT_EQ) OR (tok == LSHIFT_EQ) OR (tok == RSHIFT_EQ) OR (tok == AMP_EQ) OR (tok == CARET_EQ) OR (tok == BAR_EQ)) {

    op = tok;
    get_tok();
    child = parse_assignment_expression();
    result = new_ast2(op, result, child);

  }

  return result;
}

ast parse_comma_expression() {

  ast result = parse_assignment_expression();
  ast child;
  ast tail;

  if (tok == ',') { /* List of 1 elements are not boxed in a cons cell */
    result = new_ast2(',', result, 0); /* Create default cons cell */
    tail = result;
    while (tok == ',') {

      get_tok();
      child = parse_assignment_expression();
      child = new_ast2(',', child, 0); /* New tail cons cell */
      set_child(tail, 1, child);       /* Add new cons cell at end of list */
      tail = child;                    /* Advance tail */

    }
  }

  return result;
}

ast parse_comma_expression_opt() {

  ast result;

  if ((tok == ':') OR (tok == ';') OR (tok == ')')) {
    result = 0;
  } else {
    result = parse_comma_expression();
  }

  return result;
}

ast parse_expression() {
  return parse_comma_expression();
}

ast parse_constant_expression() {
  return parse_expression();
}

ast parse_statement() {

  ast result;
  ast child1;
  ast child2;
  ast child3;
  int start_tok;

  if (tok == IF_KW) {

    get_tok();
    result = parse_parenthesized_expression();
    child1 = parse_statement();

    if (tok == ELSE_KW) {
      get_tok();
      child2 = parse_statement();
    } else {
      child2 = 0;
    }

    result = new_ast3(IF_KW, result, child1, child2);

  } else if (tok == SWITCH_KW) {

    get_tok();
    result = parse_parenthesized_expression();
    child1 = parse_statement();

    result = new_ast2(SWITCH_KW, result, child1);

  } else if (tok == CASE_KW) {

    get_tok();
    result = parse_constant_expression();
    expect_tok(':');
    child1 = parse_statement();

    result = new_ast2(CASE_KW, result, child1);

  } else if (tok == DEFAULT_KW) {

    get_tok();
    expect_tok(':');
    result = parse_statement();

    result = new_ast1(DEFAULT_KW, result);

  } else if (tok == WHILE_KW) {

    get_tok();
    result = parse_parenthesized_expression();
    child1 = parse_statement();

    result = new_ast2(WHILE_KW, result, child1);

  } else if (tok == DO_KW) {

    get_tok();
    result = parse_statement();
    expect_tok(WHILE_KW);
    child1 = parse_parenthesized_expression();
    expect_tok(';');

    result = new_ast2(DO_KW, result, child1);

  } else if (tok == FOR_KW) {

    get_tok();
    expect_tok('(');
    result = parse_comma_expression_opt();
    expect_tok(';');
    child1 = parse_comma_expression_opt();
    expect_tok(';');
    child2 = parse_comma_expression_opt();
    expect_tok(')');
    child3 = parse_statement();

    result = new_ast4(FOR_KW, result, child1, child2, child3);

  } else if (tok == GOTO_KW) {

    get_tok();
    expect_tok(IDENTIFIER);
    result = new_ast0(GOTO_KW, val);
    expect_tok(';');

  } else if (tok == CONTINUE_KW) {

    get_tok();
    expect_tok(';');

    result = new_ast0(CONTINUE_KW, 0);

  } else if (tok == BREAK_KW) {

    get_tok();
    expect_tok(';');

    result = new_ast0(BREAK_KW, 0);

  } else if (tok == RETURN_KW) {

    get_tok();
    result = parse_comma_expression_opt();
    expect_tok(';');

    result = new_ast1(RETURN_KW, result);

  } else if (tok == '{') {

    result = parse_compound_statement();

  } else {

    start_tok = tok;

    result = parse_comma_expression_opt();

    if ((tok == ':') AND (start_tok != '(') AND (get_op(result) == IDENTIFIER)) {

      child1 = parse_statement();

      result = new_ast2(':', result, child1);

    } else {

      expect_tok(';');

    }
  }

  return result;
}

ast parse_compound_statement() {

  ast result = 0;
  ast child1;
  ast tail;

  expect_tok('{');

  /* TODO: Simplify this */
  if ((tok != '}') AND (tok != EOF)) {
    if (is_type_starter(tok)) {
      child1 = parse_definition(1);
    } else {
      child1 = parse_statement();
    }
    result = new_ast2('{', child1, 0);
    tail = result;
    while ((tok != '}') AND (tok != EOF)) {
      if (is_type_starter(tok)) {
        child1 = parse_definition(1);
      } else {
        child1 = parse_statement();
      }
      child1 = new_ast2('{', child1, 0);
      set_child(tail, 1, child1);
      tail = child1;
    }
  }

  expect_tok('}');

  return result;
}

void print_string_char(int c) {
  if (c == 7) printf("\\a");
  else if (c == 8) printf("\\b");
  else if (c == 12) printf("\\f");
  else if (c == 10) printf("\\n");
  else if (c == 13) printf("\\r");
  else if (c == 9) printf("\\t");
  else if (c == 11) printf("\\v");
  else if ((c == '\\') OR (c == '\'') OR (c == '\"')) printf("\\%c", c);
  else if ((c < 32) OR (c > 126)) printf("\\%d%d%d", c>>6, (c>>3)&7, c&7);
  else putchar(c);
}

/* codegen */

#define text int
#define TEXT_POOL_SIZE 1000000
int text_pool[TEXT_POOL_SIZE];
int text_alloc = 1; /* Start at 0 because 0 is the empty text */

#ifndef SIX_CC
/* Place prototype of mutually recursive functions here */
text comp_array_lvalue(ast node);
text comp_lvalue(ast node);
text comp_fun_call_code(ast node, ast assign_to);
void comp_fun_call(ast node, ast assign_to);
void comp_body(ast node);
void comp_statement(ast node, int else_if);
#endif

/*
  Because concatenating strings is very expensive and a common operation, we
  use a tree structure to represent the concatenated strings. That way, the
  concatenation can be done in O(1).
  At the end of the codegen process, the tree will be flattened into a single
  string.
*/

text wrap_int(int i) {
  if (text_alloc + 3 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_INTEGER;
  text_pool[text_alloc + 1] = i;
  return (text_alloc += 2) - 2;
}

text wrap_char(char c) {
  if (text_alloc + 2 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_CHAR;
  text_pool[text_alloc + 1] = c;
  return (text_alloc += 2) - 2;
}

text string_concat(text t1, text t2) {
  if (text_alloc + 4 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_TREE;
  text_pool[text_alloc + 1] = 2;
  text_pool[text_alloc + 2] = t1;
  text_pool[text_alloc + 3] = t2;
  return (text_alloc += 4) - 4;
}

text string_concat3(text t1, text t2, text t3) {
  if (text_alloc + 5 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_TREE;
  text_pool[text_alloc + 1] = 3;
  text_pool[text_alloc + 2] = t1;
  text_pool[text_alloc + 3] = t2;
  text_pool[text_alloc + 4] = t3;
  return (text_alloc += 5) - 5;
}

text string_concat4(text t1, text t2, text t3, text t4) {
  if (text_alloc + 6 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_TREE;
  text_pool[text_alloc + 1] = 4;
  text_pool[text_alloc + 2] = t1;
  text_pool[text_alloc + 3] = t2;
  text_pool[text_alloc + 4] = t3;
  text_pool[text_alloc + 5] = t4;
  return (text_alloc += 6) - 6;
}

text string_concat5(text t1, text t2, text t3, text t4, text t5) {
  if (text_alloc + 7 >= TEXT_POOL_SIZE) fatal_error("string tree pool overflow");
  text_pool[text_alloc] = TEXT_TREE;
  text_pool[text_alloc + 1] = 5;
  text_pool[text_alloc + 2] = t1;
  text_pool[text_alloc + 3] = t2;
  text_pool[text_alloc + 4] = t3;
  text_pool[text_alloc + 5] = t4;
  text_pool[text_alloc + 6] = t5;
  return (text_alloc += 7) - 7;
}

/*
  TODO: Optimize wrap_str
  For N char, we store it using 6*N (4 from string_concat, 2 from wrap_char) bytes, so not very efficient...
  Also, most of the strings passed to wrap_str are hardcoded, and could probably be unpacked once instead of every time.
*/
text wrap_str(char_ptr s) {
  int i = 0;
  int result = 0;

  while (s[i] != 0) {
    result = string_concat(result, wrap_char(s[i]));
    i += 1;
  }

  return result;
}

text concatenate_strings_with(text t1, text t2, text sep) {
  if (t1 == 0) return t2;
  if (t2 == 0) return t1;
  return string_concat3(t1, sep, t2);
}

void print_text(text t) {
  int i;

  if (t == 0) return;

  if (text_pool[t] == TEXT_TREE) {
    for (i = 0; i < text_pool[t + 1]; i += 1) {
      print_text(text_pool[t + i + 2]);
    }
  } /* else if (text_pool[t] == TEXT_STRING) {
    printf("%s", text_pool[t + 1]);
  } else if (text_pool[t] == TEXT_SUBSTRING) {
    printf("%.*s", text_pool[t + 2], text_pool[t + 1]);
  } */ else if (text_pool[t] == TEXT_INTEGER) {
    printf("%d", text_pool[t + 1]);
  } else if (text_pool[t] == TEXT_CHAR) {
    putchar(text_pool[t + 1]);
  } else {
    printf("\nt=%d %d\n", t, text_pool[t]);
    fatal_error("unexpected string tree node");
  }
}

/* Codegen context */

#define GLO_DECL_SIZE 100000
text glo_decls[GLO_DECL_SIZE];  /* Generated code */
int glo_decl_ix = 0;            /* Index of last generated line of code  */
int nest_level = 0;             /* Current level of indentation */
int in_tail_position = false;   /* Is the current statement in tail position? */
int loop_nesting_level = 0;     /* Number of loops surrounding the current statement */
int loop_end_actions_start = 0; /* Start position of declarations for the last action in a for loop */
int loop_end_actions_end = 0;   /* End position of declarations for the last action in a for loop */
ast local_env = 0;              /* List of local variables */
ast local_env_size = 0;         /* Size of the environment */
int gensym_ix = 0;              /* Counter for fresh_ident */
int fun_gensym_ix = 0;          /* Maximum value of gensym_ix for the current function */
int max_gensym_ix = 0;          /* Maximum value of gensym_ix for all functions */
int string_counter = 0;         /* Counter for string literals */

void append_glo_decl(text decl) {
  glo_decls[glo_decl_ix] = nest_level;
  glo_decls[glo_decl_ix + 1] = 1; /* If it's active or not. Used by undo_glo_decls and replay_glo_decls */
  glo_decls[glo_decl_ix + 2] = decl;
  glo_decl_ix += 3;
}

/*
  Remove the n last declarations by decrementing the active field.
  A non-positive active value means that the declaration is active,
  A 0 value means that the declaration was unset once.
  A negative value means that the declaration was unset multiple times.
  Because undone declarations are generally replayed, declarations with negative
  values are ignored when replayed since they have already been replayed before.
  This is useful to compile some code at a different time than it is used.
*/
void undo_glo_decls(int start) {
  while (start < glo_decl_ix) {
    glo_decls[start + 1] -= 1; /* To support nested undone declarations */
    start += 3;
  }
}

/*
  Replay the declarations betwee start and end. Replayed declarations must first
  be undone with undo_glo_decls.
  The reindent parameter controls if the declarations should be replayed at the
  current nest level or at the nest level when they were added.
*/
void replay_glo_decls(int start, int end, int reindent) {
  while (start < end) {
    if (glo_decls[start + 1] == 0) { /* Skip inactive declarations that are at the current level */
      append_glo_decl(glo_decls[start + 2]);
      if (!reindent) glo_decls[glo_decl_ix - 3] = glo_decls[start]; /* Replace nest_level */
    }
    start += 3;
  }
}

text replay_glo_decls_inline(int start, int end) {
  int i;
  text res = 0;
  while (start < end) {
    if (glo_decls[start + 1] == 0) { /* Skip inactive declarations */
      res = concatenate_strings_with(res, glo_decls[start + 2], wrap_str("; "));
    }
    start += 3;
  }
  if (res != 0) { res = string_concat(res, wrap_str("; ")); }

  return res;
}

void print_glo_decls() {
  int i;
  int level;
  for (i = 0; i < glo_decl_ix; i += 3) {
    if (glo_decls[i + 1] == 1) { /* Skip inactive declarations */
      level = glo_decls[i];
      while (level > 0) {
        putchar(' '); putchar(' ');
        level -= 1;
      }
      print_text(glo_decls[i + 2]);
      putchar('\n');
    }
  }
}

ast find_var_in_local_env(ast ident_tok) {
  ast env = local_env;
  ast var;
  while (env != 0) {
    var = get_child(env, 0);
    /* Because identifier tokens are unique, we can compare their address to check if they are the same. */
    if (get_child(var, 0) == ident_tok) return var;

    env = get_child(env, 1);
  }
  return -1;
}

/*
  Similar to env_var, but doesn't use the local environment and assumes that the
  identifier is internal or global. This is faster than env_var when we know that
  the variable is not local.
*/
text format_special_var(ast ident, ast prefixed_with_dollar) {
  int op = get_op(ident);
  if (op == IDENTIFIER_INTERNAL) {
    return string_concat(wrap_str("__g"), get_val(ident));
  } else if (op == IDENTIFIER_STRING) {
    return string_concat(wrap_str("__str_"), get_val(ident));
  } else if (op == IDENTIFIER_DOLLAR) {
    if (prefixed_with_dollar) {
      return wrap_int(get_val(ident));
    } else {
      return string_concat(wrap_char('$'), wrap_int(get_val(ident)));
    }
  } else if (op == IDENTIFIER_EMPTY) {
    return wrap_str("__");
  } else {
    printf("op=%d %c", op, op);
    fatal_error("format_special_var: unknown identifier type");
  }
}

text global_var(ast ident_tok) {
  return string_concat(wrap_char('_'), wrap_str(string_pool + get_val(ident_tok)));
}

text env_var_with_prefix(ast ident, ast prefixed_with_dollar) {
  ast var;
  text res;

  if (get_op(ident) == IDENTIFIER) {
    var = find_var_in_local_env(get_val(ident));
    if (var != -1) {
      if (get_child(var, 2) == KIND_PARAM AND get_child(var, 3)) {
        res = wrap_int(get_child(var, 2));
        if (!prefixed_with_dollar) res = string_concat(wrap_char('$'), res);
      } else {
        res = wrap_str(string_pool + get_val(get_val(ident)));
      }
    } else {
      res = global_var(get_val(ident));
    }
  } else {
    res = format_special_var(ident, prefixed_with_dollar);
  }

  return res;
}

text env_var(ast ident) {
  return env_var_with_prefix(ident, false);
}

text function_name(int ident_tok) {
  return string_concat(wrap_char('_'), wrap_str(string_pool + get_val(ident_tok)));
}

ast fresh_ident() {
  gensym_ix += 1;
  if (gensym_ix > fun_gensym_ix) {
    fun_gensym_ix = gensym_ix;
  }
  if (gensym_ix > max_gensym_ix) {
    max_gensym_ix = gensym_ix;
  }
  return new_ast0(IDENTIFIER_INTERNAL, wrap_int(gensym_ix));
}

/* TODO: Reuse identifier for strings used in multiple places */
ast fresh_string_ident() {
  string_counter += 1;
  return new_ast0(IDENTIFIER_STRING, wrap_int(string_counter - 1));
}

/* TODO: Remove this eventually or move to debug module */
void print_local_env() {
  ast env = local_env;
  ast ident;
  printf("##### print_local_env\n");
  while (env != 0) {
    ident = get_child(get_child(env, 0), 0);

    printf("Ident: %d = %s\n", ident, string_pool + get_val(ident));
    env = get_child(env, 1);
  }
}

/*
The local environment is a list of variables represented using ',' nodes.
A variable is a LOCAL_VAR node with 4 children:
  1. Ident
  2. Position of the variable in the shell environment ($1, $2, ...)
  3. Kind of variable (function param or local var)
  4. Constant: if the variable is never assigned to
*/
void add_var_to_local_env(ast ident_tok, int position) {
  ast env = local_env;
  ast var;

  /* Check if the variable is not in env. This should always do nothing */
  if (find_var_in_local_env(ident_tok) != -1) {
    fatal_error("add_var_to_local_env: variable already in local environment");
  }

  /* The var is not part of the environment, so we add it */
  var = new_ast4(LOCAL_VAR, ident_tok, position, KIND_LOCAL, false);
  local_env = new_ast2(',', var, local_env);
  local_env_size += 1;
}

void add_vars_to_local_env(ast lst, int position) {
  ast decl;
  while (lst != 0) {
    decl = get_child(lst, 0);
    add_var_to_local_env(get_child(decl, 0), position);
    lst = get_child(lst, 1);
    position += 1;
  }
}

/*
  Since global and internal variables are prefixed with _, we restrict the name
  of variables to not start with _. Also, because some shells treat some
  variables as special, we prevent their use. Additionally, EOF and NULL cannot
  be redefined.
*/
void assert_idents_are_safe(ast lst) {
  ast ident_tok;
  char_ptr name;
  while (lst != 0) {
    ident_tok = get_child(get_child(lst, 0), 0);
    name = string_pool + get_val(ident_tok);

    if (name[0] == '_' OR !strcmp(name, "EOF") OR !strcmp(name, "NULL") OR !strcmp(name, "argv")) {
      printf("%s ", name);
      fatal_error("variable name is invalid. It can't start with '_', be 'OEF', 'NULL' or 'argv'.");
    }

    lst = get_child(lst, 1);
  }
}

void save_local_vars() {
  ast env = local_env;
  ast local_var;
  ast ident;
  text res = 0;
  int counter = fun_gensym_ix;

  while (counter > 0) {
    ident = new_ast0(IDENTIFIER_INTERNAL, wrap_int(counter));
    res = concatenate_strings_with(res, format_special_var(ident, false), wrap_char(' '));
    counter -= 1;
  }

  while (env != 0) {
    local_var = get_child(env, 0);
    ident = new_ast0(IDENTIFIER, get_child(local_var, 0));
    /* Constant function parameters are assigned to $1, $2, ... and don't need to be saved */
    if (get_child(local_var, 2) == KIND_PARAM AND get_child(local_var, 3)) continue;

    res = concatenate_strings_with(res, env_var(ident), wrap_char(' '));

    env = get_child(env, 1);
  }

  if (res != 0) {
    append_glo_decl(string_concat(wrap_str("save_loc_var "), res));
  }
}

/*
  The only difference between save_local_vars and restore_local_vars is the
  order of the arguments and the call to rest_loc_var instead of save_loc_var.
*/
void restore_local_vars() {
  ast env = local_env;
  ast local_var;
  ast ident;
  text res = 0;
  int counter = fun_gensym_ix;

  while (counter > 0) {
    ident = new_ast0(IDENTIFIER_INTERNAL, wrap_int(counter));
    res = concatenate_strings_with(format_special_var(ident, false), res, wrap_char(' '));
    counter -= 1;
  }

  while (env != 0) {
    local_var = get_child(env, 0);
    ident = new_ast0(IDENTIFIER, get_child(local_var, 0));
    /* Constant function parameters are assigned to $1, $2, ... and don't need to be saved */
    if (get_child(local_var, 2) == KIND_PARAM AND get_child(local_var, 3)) continue;

    res = concatenate_strings_with(env_var(ident), res, wrap_char(' '));

    env = get_child(env, 1);
  }

  if (res != 0) {
    append_glo_decl(string_concat(wrap_str("rest_loc_var $1 "), res));
  }
}

text op_to_str(int op) {
  if      (op < 256)         return string_concat3(wrap_char(' '), wrap_char(op), wrap_char(' '));
  else if (op == AMP_AMP)    return wrap_str(" && ");
  else if (op == AMP_EQ)     return wrap_str(" &= ");
  else if (op == BAR_BAR)    return wrap_str(" || ");
  else if (op == BAR_EQ)     return wrap_str(" |= ");
  else if (op == CARET_EQ)   return wrap_str(" ^= ");
  else if (op == EQ_EQ)      return wrap_str(" == ");
  else if (op == GT_EQ)      return wrap_str(" >= ");
  else if (op == LSHIFT_EQ)  return wrap_str(" <<= ");
  else if (op == LT_EQ)      return wrap_str(" <= ");
  else if (op == LSHIFT)     return wrap_str(" << ");
  else if (op == MINUS_EQ)   return wrap_str(" -= ");
  else if (op == EXCL_EQ)    return wrap_str(" != ");
  else if (op == PERCENT_EQ) return wrap_str(" %%= ");
  else if (op == PLUS_EQ)    return wrap_str(" += ");
  else if (op == RSHIFT_EQ)  return wrap_str(" >>= ");
  else if (op == RSHIFT)     return wrap_str(" >> ");
  else if (op == SLASH_EQ)   return wrap_str(" /= ");
  else if (op == STAR_EQ)    return wrap_str(" *= ");
  else {
    printf("op=%d %c\n", op, op);
    fatal_error("op_to_str: unexpected operator");
  }
}

/*
  Similar to op_to_str, but returns the shell test operand instead of the C-style operands.
*/
text test_op_to_str(int op) {
  if      (op == EQ_EQ)      return wrap_str(" -eq ");
  else if (op == EXCL_EQ)    return wrap_str(" -ne ");
  else if (op == '<')        return wrap_str(" -lt ");
  else if (op == '>')        return wrap_str(" -gt ");
  else if (op == LT_EQ)      return wrap_str(" -le ");
  else if (op == GT_EQ)      return wrap_str(" -ge ");
  else {
    printf("op=%d %c\n", op, op);
    fatal_error("test_op_to_str: unexpected operator");
  }
}

text character_ident(int c) {
  if (in_range(c, 'a', 'z')) {
    return string_concat(wrap_str("__LOWER_"), wrap_char(c));
  } else if (in_range(c, 'A', 'Z')) {
    return string_concat(wrap_str("__UPPER_"), wrap_char(c));
  } else if (in_range(c, '0', '9')) {
    return string_concat(wrap_str("__DIGIT_"), wrap_int(c - 48));
  } else {
    if      (c == '\0') return wrap_str("__NULL_CH");
    else if (c == '\n') return wrap_str("__NEWLINE_CH");
    else if (c == ' ')  return wrap_str("__SPACE_CH");
    else if (c == '!')  return wrap_str("__EXCL_CH");
    else if (c == '"')  return wrap_str("__DQUOTE_CH");
    else if (c == '#')  return wrap_str("__SHARP_CH");
    else if (c == '$')  return wrap_str("__DOLLAR_CH");
    else if (c == '%')  return wrap_str("__PERCENT_CH");
    else if (c == '&')  return wrap_str("__AMP_CH");
    else if (c == '\'') return wrap_str("__QUOTE_CH");
    else if (c == '(')  return wrap_str("__LPAREN_CH");
    else if (c == ')')  return wrap_str("__RPAREN_CH");
    else if (c == '*')  return wrap_str("__STAR_CH");
    else if (c == '+')  return wrap_str("__PLUS_CH");
    else if (c == ',')  return wrap_str("__COMMA_CH");
    else if (c == '-')  return wrap_str("__MINUS_CH");
    else if (c == '.')  return wrap_str("__PERIOD_CH");
    else if (c == '/')  return wrap_str("__SLASH_CH");
    else if (c == ':')  return wrap_str("__COLON_CH");
    else if (c == ';')  return wrap_str("__SEMICOLON_CH");
    else if (c == '<')  return wrap_str("__LT_CH");
    else if (c == '=')  return wrap_str("__EQ_CH");
    else if (c == '>')  return wrap_str("__GT_CH");
    else if (c == '?')  return wrap_str("__QUESTION_CH");
    else if (c == '@')  return wrap_str("__AT_CH");
    else if (c == '^')  return wrap_str("__CARET_CH");
    else if (c == '[')  return wrap_str("__LBRACK_CH");
    else if (c == '\\') return wrap_str("__BACKSLASH_CH");
    else if (c == ']')  return wrap_str("__RBRACK_CH");
    else if (c == '_')  return wrap_str("__UNDERSCORE_CH");
    else if (c == '`')  return wrap_str("__BACKTICK_CH");
    else if (c == '{')  return wrap_str("__LBRACE_CH");
    else if (c == '|')  return wrap_str("__BAR_CH");
    else if (c == '}')  return wrap_str("__RBRACE_CH");
    else if (c == '~')  return wrap_str("__TILDE_CH");
    else if (c == '\a') return wrap_str("__ALARM_CH");
    else if (c == '\b') return wrap_str("__BACKSPACE_CH");
    else if (c == '\f') return wrap_str("__PAGE_CH");
    else if (c == '\r') return wrap_str("__RET_CH");
    else if (c == '\t') return wrap_str("__TAB_CH");
    else if (c == '\v') return wrap_str("__VTAB_CH");
    else fatal_error("Unknown character");
  }
}


ast replaced_fun_calls = 0;
ast conditional_fun_calls = 0;
ast literals_inits = 0;
int executes_conditionally = 0;
int contains_side_effects = 0;

/*
  We can't have function calls and other side effects in $(( ... )), so we need to handle them separately.
  For unconditional function calls, they are replaced with unique identifiers and returned as a list with their new identifiers.
  For pre/post-increments/decrements, we map them to a pre-side-effects and replace with the corresponding operation.
  Note that pre/post-increments/decrements of function calls are not supported.
*/
ast handle_side_effects_go(ast node, int executes_conditionally) {
  int op = get_op(node);
  int nb_children = get_nb_children(node);
  int start_gensym_ix;
  ast sub1;
  ast sub2;
  ast previous_conditional_fun_calls;
  ast left_conditional_fun_calls;
  ast right_conditional_fun_calls;

  if (nb_children == 0) {
    if (op == IDENTIFIER OR op == IDENTIFIER_INTERNAL OR op == IDENTIFIER_STRING OR op == IDENTIFIER_DOLLAR OR op == INTEGER OR op == CHARACTER) {
      return node;
    } else if (op == STRING) {
      /* We must initialize strings before the expression */
      sub1 = fresh_string_ident();
      literals_inits = new_ast2(',', new_ast2(',', sub1, get_val(node)), literals_inits);
      return sub1;
    } else {
      printf("handle_side_effects_go: op=%d %c", op, op);
      fatal_error("unexpected operator");
    }
  } else if (nb_children == 1) {
    if ((op == '&') OR (op == '*') OR (op == '+') OR (op == '-') OR (op == '~') OR (op == '!')) {
      /* TODO: Reuse ast node? */
      return new_ast1(op, handle_side_effects_go(get_child(node, 0), executes_conditionally));
    } else if ((op == PLUS_PLUS) OR (op == MINUS_MINUS)) {
      /* The parser fails on postfix ++/--, so this is only preincrement/predecrement */
      contains_side_effects = true;
      return new_ast1(op, handle_side_effects_go(get_child(node, 0), executes_conditionally));
    } else {
      printf("1: op=%d %c", op, op);
      fatal_error("unexpected operator");
    }
  } else if (nb_children == 2) {
    if ((op == '(')) { /* Function call */
      sub1 = fresh_ident(); /* Unique identifier for the function call */

      start_gensym_ix = gensym_ix;

      /* Traverse the arguments and replace them with the result of handle_side_effects_go */
      sub2 = get_child(node, 1);
      if (sub2 != 0) { /* Check if not an empty list */
        if (get_op(sub2) == ',') {
          while (get_op(sub2) == ',') {
            set_child(sub2, 0, handle_side_effects_go(get_child(sub2, 0), executes_conditionally));
            sub2 = get_child(sub2, 1);
          }
        } else { /* sub2 is the first argument, not wrapped in a cons cell */
          sub2 = handle_side_effects_go(sub2, executes_conditionally);
          set_child(node, 1, sub2);
        }
      }

      /*
        All the temporary variables used for the function parameters can be
        reused after the function call, so resetting the gensym counter.
      */
      gensym_ix = start_gensym_ix;

      if (executes_conditionally) { conditional_fun_calls = new_ast2(',', new_ast2(',', sub1, node), conditional_fun_calls); }
      else { replaced_fun_calls = new_ast2(',', new_ast2(',', sub1, node), replaced_fun_calls); }

      return sub1;
    } else if ( (op == '&') OR (op == '|') OR (op == '<') OR (op == '>') OR (op == '+') OR (op == '-') OR (op == '*') OR (op == '/')
      OR (op == '%') OR (op == '^') OR (op == ',') OR (op == EQ_EQ) OR (op == EXCL_EQ) OR (op == LT_EQ) OR (op == GT_EQ) OR (op == LSHIFT) OR (op == RSHIFT) OR (op == '=') OR (op == '[') ) {
      /* We can't place handle_side_effects_go directly in new_ast2 call because six-cc creates a global variable that gets overwritten in the other handle_side_effects_go calls */
      sub1 = handle_side_effects_go(get_child(node, 0), executes_conditionally);
      sub2 = handle_side_effects_go(get_child(node, 1), executes_conditionally); /* We could inline that one since the assignment to the global variable is done after the last handle_side_effects_go call */
      return new_ast2(op, sub1, sub2);
    } else if ((op == AMP_EQ) OR (op == BAR_EQ) OR (op == CARET_EQ) OR (op == LSHIFT_EQ) OR (op == MINUS_EQ) OR (op == PERCENT_EQ) OR (op == PLUS_EQ) OR (op == RSHIFT_EQ) OR (op == SLASH_EQ) OR (op == STAR_EQ)) {
      /* Just like previous case, except that we update contains_side_effects */
      contains_side_effects = true;
      sub1 = handle_side_effects_go(get_child(node, 0), executes_conditionally);
      sub2 = handle_side_effects_go(get_child(node, 1), executes_conditionally); /* We could inline that one since the assignment to the global variable is done after the last handle_side_effects_go call */
      return new_ast2(op, sub1, sub2);
    } else if ((op == AMP_AMP) OR (op == BAR_BAR)) {
      previous_conditional_fun_calls = conditional_fun_calls;
      conditional_fun_calls = 0;
      /*
        The left side is always executed, unless the whole expression is executed conditionally.
        We could compile it as always executed, but it makes the Shell code less regular so we compile it conditionally.
      */
      sub1 = handle_side_effects_go(get_child(node, 0), true);
      left_conditional_fun_calls = conditional_fun_calls;
      conditional_fun_calls = 0;
      sub2 = handle_side_effects_go(get_child(node, 1), true);
      right_conditional_fun_calls = conditional_fun_calls;
      conditional_fun_calls = previous_conditional_fun_calls;
      return new_ast4(op, sub1, sub2, left_conditional_fun_calls, right_conditional_fun_calls);
    } else {
      printf("2: op=%d %c", op, op);
      fatal_error("unexpected operator");
    }
  } else if (nb_children == 3) {
    /* TODO: Ternary expression */
    printf("3: op=%d %c\n", op, op);
    fatal_error("unexpected operator");
  } else if (nb_children == 4) {
    printf("4: op=%d %c\n", op, op);
    fatal_error("unexpected operator");
  } else {
    printf("5: op=%d %c with %d children\n", op, op, get_nb_children(node));
    fatal_error("unexpected operator");
  }
}

ast handle_side_effects(ast node) {
  replaced_fun_calls = 0;
  conditional_fun_calls = 0;
  literals_inits = 0;
  contains_side_effects = false;
  return handle_side_effects_go(node, false);
}

int RVALUE_CTX_BASE = 0;
int RVALUE_CTX_ARITH_EXPANSION = 1; /* Like base context, except that we're already in $(( ... )) */
int RVALUE_CTX_TEST = 2;

text with_prefixed_side_effects(ast test_side_effects, text code) {

  text test_side_effects_code = 0;

  while (test_side_effects != 0) {
    test_side_effects_code =
      string_concat3(test_side_effects_code,
                     comp_fun_call_code(get_child(get_child(test_side_effects, 0), 1), get_child(get_child(test_side_effects, 0), 0)),
                     wrap_str("; "));
    test_side_effects = get_child(test_side_effects, 1);
  }
  if (test_side_effects_code != 0) {
    return string_concat4(wrap_str("{ "), test_side_effects_code, code, wrap_str("; }"));
  } else {
    return code;
  }
}

/*
  Wrap code in $((...)) if it's not already and if it's already in $(( )), wrap
  it in parentheses if parens_otherwise is true. If it's not in an arithmetic
  expansion and we're compiling tests, we also add the test condition to make it
  a valid test.
*/
text wrap_if_needed(int parens_otherwise, int context, ast test_side_effects, text code) {
  if (context == RVALUE_CTX_ARITH_EXPANSION) {
    if (parens_otherwise) return string_concat3(wrap_char('('), code, wrap_char(')'));
    else return code;
  } else if (context == RVALUE_CTX_TEST) {
    return with_prefixed_side_effects(test_side_effects, string_concat3(wrap_str("[ $(( "), code, wrap_str(" )) -ne 0 ]")));
  } else {
    return string_concat3(wrap_str("$(("), code, wrap_str("))"));
  }
}

/*
  Used to supports the case `if/while (c) { ... }`, where c is a variable or a literal.
  This is otherwise handled by wrap-if-needed, but we don't want to wrap in $(( ... )) here.
*/
text wrap_in_condition_if_needed(int context, ast test_side_effects, text code) {
  if (context == RVALUE_CTX_TEST) {
    return with_prefixed_side_effects(test_side_effects, string_concat3(wrap_str("[ "), code, wrap_str(" -ne 0 ]")));
  } else {
    return code;
  }
}

text comp_rvalue_go(ast node, int context, ast test_side_effects) {
  int op = get_op(node);
  int nb_children = get_nb_children(node);
  int val;
  text sub1;
  text sub2;

  if (nb_children == 0) {
    if (op == INTEGER) {
      return wrap_in_condition_if_needed(context, test_side_effects, wrap_int(-get_val(node)));
    } else if (op == CHARACTER) {
      if (context == RVALUE_CTX_ARITH_EXPANSION) {
        return character_ident(get_val(node));
      } else {
        return wrap_in_condition_if_needed(context, test_side_effects, string_concat(wrap_char('$'), character_ident(get_val(node))));
      }
      return wrap_in_condition_if_needed(context, test_side_effects, wrap_int(get_val(node)));
    } else if (op == IDENTIFIER OR op == IDENTIFIER_INTERNAL OR op == IDENTIFIER_STRING OR op == IDENTIFIER_DOLLAR) {
      if (context == RVALUE_CTX_ARITH_EXPANSION) { return env_var_with_prefix(node, false); }
      else { return wrap_in_condition_if_needed(context, test_side_effects, string_concat(wrap_char('$'), env_var_with_prefix(node, true))); }
    } else if (op == STRING) {
      fatal_error("comp_rvalue_go: string should have been removed by handle_side_effects");
    } else {
      printf("op=%d %c", op, op);
      fatal_error("comp_rvalue_go: unknown rvalue with nb_children == 0");
    }
  } else if (nb_children == 1) {
    if (op == '*') {
      /*
        Setting context to RVALUE_CTX_BASE even if it's wrapped in $(( ... )) because we
        need another layer of wrapping if it's a complex expression, i.e. not a
        literal or a variable.
      */
      sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_BASE, 0);
      return wrap_if_needed(true, context, test_side_effects, string_concat(wrap_char('_'), sub1));
    } else if (op == '+') {
      /* +x is equivalent to x */
      return comp_rvalue_go(get_child(node, 0), context, test_side_effects);
    } else if (op == '-') {
      /*
        Check if the rest of ast is a literal, if so directly return the negated value.
        Note: I think this can be simplified by not wrapped in () in the else case.
      */
      if (get_op(get_child(node, 0)) == INTEGER) {
        return wrap_in_condition_if_needed(context, test_side_effects, wrap_int(-get_val(get_child(node, 0))));
      } else {
        sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
        return wrap_if_needed(false, context, test_side_effects, string_concat3(wrap_str("-("), sub1, wrap_char(')')));
      }
    } else if (op == '~') {
      sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
      return wrap_if_needed(false, context, test_side_effects, string_concat3(wrap_str("~("), sub1, wrap_char(')')));
    } else if (op == '!') {
      sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
      return wrap_if_needed(false, context, test_side_effects, string_concat(wrap_char('!'), sub1));
    } else if (op == MINUS_MINUS) {
      sub1 = comp_lvalue(get_child(node, 0));
      return wrap_if_needed(true, context, test_side_effects, string_concat(sub1, wrap_str(" -= 1")));
    } else if (op == PLUS_PLUS) {
      sub1 = comp_lvalue(get_child(node, 0));
      return wrap_if_needed(true, context, test_side_effects, string_concat(sub1, wrap_str(" += 1")));
    } else if (op == '&') {
      fatal_error("comp_rvalue_go: address of operator not supported");
    } else {
      printf("1: op=%d %c", op, op);
      fatal_error("comp_rvalue_go: unexpected operator");
    }
  } else if (nb_children == 2) {
    if (op == '+' OR op == '-' OR op == '*' OR op == '/' OR op == '%' OR op == '&' OR op == '|' OR op == '^' OR op == LSHIFT OR op == RSHIFT) {
      sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
      sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION, 0);
      return wrap_if_needed(true, context, test_side_effects, string_concat3(sub1, op_to_str(op), sub2));
    } else if (op == PLUS_EQ OR op == MINUS_EQ OR op == STAR_EQ OR op == SLASH_EQ OR op == PERCENT_EQ OR op == '=') {
      sub1 = comp_lvalue(get_child(node, 0));
      sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION, 0);
      return wrap_if_needed(true, context, test_side_effects, string_concat3(sub1, op_to_str(op), sub2));
    } else if (op == '[') { /* six.index */
      sub1 = comp_array_lvalue(get_child(node, 0));
      sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION, 0);
      return wrap_if_needed(false, context, test_side_effects, string_concat5(wrap_str("_$(("), sub1, wrap_char('+'), sub2, wrap_str("))")));
    } else if (op == EQ_EQ OR op == EXCL_EQ OR op == LT_EQ OR op == GT_EQ OR op == '<' OR op == '>') {
      if (context == RVALUE_CTX_TEST) {
        sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_BASE, 0);
        sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_BASE, 0);
        return with_prefixed_side_effects(test_side_effects, string_concat5(wrap_str("[ "), sub1, test_op_to_str(op), sub2, wrap_str(" ]")));
      } else {
        sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
        sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION, 0);
        return wrap_if_needed(true, context, test_side_effects, string_concat3(sub1, op_to_str(op), sub2));
      }
    } else if (op == AMP_AMP OR op == BAR_BAR) {
      fatal_error("comp_rvalue_go: && and || should have 4 children by that point");
    } else {
      fatal_error("comp_rvalue_go: unknown rvalue");
    }
  } else if (nb_children == 3) {
    if (op == '?') {
      fatal_error("comp_rvalue_go: ternary operator not supported");
    } else {
      printf("op=%d %c\n", op, op);
      fatal_error("comp_rvalue_go: unknown rvalue with 3 children");
    }
    return -1;
  } else if (nb_children == 4) {
    if (op == AMP_AMP OR op == BAR_BAR) {
      /*
        Note, this could also be compiled in a single [ ] block using -a and -o,
        which I think are POSIX compliant but are deprecated.
      */
      if (context == RVALUE_CTX_TEST) {
        /*
          When compiling in a test context, && and || can be compiled to Shell's
          && and || with [ ... ] blocks.

          A notable difference between these operators in Shell and C is that in
          Shell, they have equal precedence while in C, && has higher precedence.
          This means that we need to add parenthesis that would not be needed in C.

          As a heuristic, we add parenthesis whenever the left or right side of
          the operator is a different comparison operator.
        */
        sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_TEST, get_child(node, 2));
        sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_TEST, get_child(node, 3));
        if ((get_op(get_child(node, 0)) == AMP_AMP OR get_op(get_child(node, 0)) == BAR_BAR) AND get_op(get_child(node, 0)) != op) {
          sub1 = string_concat3(wrap_str("{ "), sub1, wrap_str("; }"));
        }
        if ((get_op(get_child(node, 1)) == AMP_AMP OR get_op(get_child(node, 1)) == BAR_BAR) AND get_op(get_child(node, 1)) != op) {
          sub2 = string_concat3(wrap_str("{ "), sub2, wrap_str("; }"));
        }
        return string_concat3(sub1, op_to_str(op), sub2);
      } else {
        if (test_side_effects != 0) { fatal_error("comp_rvalue_go: Arithmetic with function calls in && and || not supported"); }
        sub1 = comp_rvalue_go(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION, 0);
        sub2 = comp_rvalue_go(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION, 0);
        return wrap_if_needed(false, context, test_side_effects, string_concat3(sub1, op_to_str(op), sub2));
      }
    } else {

    }
  }
  fatal_error("comp_rvalue_go: should have returned before the end");
}

text escaped_char(char c) {
  if (c == '\a') return wrap_str("\\a");
  if (c == '\b') return wrap_str("\\b");
  if (c == '\f') return wrap_str("\\f");
  if (c == '\n') return wrap_str("\\n");
  if (c == '\r') return wrap_str("\\r");
  if (c == '\t') return wrap_str("\\t");
  if (c == '\v') return wrap_str("\\v");
  if (c == '\\') return wrap_str("\\\\");
  if (c == '"')  return wrap_str("\\\"");
  if (c == '\'') return wrap_str("\\\'");
  if (c == '?')  return wrap_str("\\\?");
  if (c == '$')  return wrap_str("\\$");
  return wrap_char(c);
}

text escape_string(char_ptr str) {
  text res = wrap_str("");
  text char_text;
  int i = 0;

  while (str[i] != '\0') {
    char_text = escaped_char(str[i]);
    res = string_concat(res, char_text);
    i += 1;
  }
  return res;
}

text comp_rvalue(ast node, int context) {
  ast simple_ast = handle_side_effects(node);
  ast replaced_fun_calls2 = replaced_fun_calls; /* Calling comp_fun_call can overwrite replaced_fun_calls, so it's saved */
  int fun_call_decl_start;
  text side_effects_text;

  while (literals_inits != 0) {
    append_glo_decl(string_concat5( wrap_str("defstr ")
                                  , format_special_var(get_child(get_child(literals_inits, 0), 0), false)
                                  , wrap_str(" \"")
                                  , escape_string(string_pool + get_child(get_child(literals_inits, 0), 1))
                                  , wrap_char('\"')));
    literals_inits = get_child(literals_inits, 1);
  }

  /* We don't want to call defstr on every iteration, so only capturing fun calls */
  fun_call_decl_start = glo_decl_ix;

  while (replaced_fun_calls2 != 0) {
    comp_fun_call(get_child(get_child(replaced_fun_calls2, 0), 1), get_child(get_child(replaced_fun_calls2, 0), 0));
    replaced_fun_calls2 = get_child(replaced_fun_calls2, 1);
  }

  /*
    When compiling a test, we place the function side effects inline with the condition.
    That way, any side effect performed in the condition of a while loop is repeated on each iteration.
    For if statements, it makes things shorter, but not always more readable.
  */
  if (context == RVALUE_CTX_TEST) {
    undo_glo_decls(fun_call_decl_start);
    side_effects_text = replay_glo_decls_inline(fun_call_decl_start, glo_decl_ix);
    return string_concat(side_effects_text, comp_rvalue_go(simple_ast, context, 0));
  } else {
    return comp_rvalue_go(simple_ast, context, 0);
  }
}

text comp_array_lvalue(ast node) {
  int op = get_op(node);
  text rvalue;
  if (op == IDENTIFIER OR op == IDENTIFIER_INTERNAL OR op == IDENTIFIER_STRING) {
    return env_var(node);
  } else if (op == '*') {
    rvalue = comp_rvalue(get_child(node, 0), RVALUE_CTX_BASE);
    return string_concat(wrap_char('_'), rvalue);
  } else {
    fatal_error("comp_array_lvalue: unknown lvalue");
  }
}

text comp_lvalue(ast node) {
  int op = get_op(node);
  text sub1;
  text sub2;

  if (op == IDENTIFIER OR op == IDENTIFIER_INTERNAL OR op == IDENTIFIER_STRING) {
    return env_var(node);
  } else if (op == '[') {
    sub1 = comp_array_lvalue(get_child(node, 0));
    sub2 = comp_rvalue(get_child(node, 1), RVALUE_CTX_ARITH_EXPANSION);
    return string_concat5(wrap_str("_$(("), sub1, wrap_char('+'), sub2, wrap_str("))"));
  } else if (op == '*') {
    sub1 = comp_rvalue(get_child(node, 0), RVALUE_CTX_BASE);
    return string_concat(wrap_char('_'), sub1);
  } else if (op == ARROW) {
    fatal_error("comp_lvalue: struct not yet supported");
  } else {
    fatal_error("comp_lvalue: unknown lvalue");
  }
}

text comp_fun_call_code(ast node, ast assign_to) {
  ast name = get_child(node, 0);
  ast params = get_child(node, 1);
  ast param;
  text code_params = 0;

  if (params != 0) { /* Check if not an empty list */
    if (get_op(params) == ',') {
      while (get_op(params) == ',') {
        param = comp_rvalue(get_child(params, 0), RVALUE_CTX_BASE);
        code_params = concatenate_strings_with(code_params, param, wrap_char(' '));
        params = get_child(params, 1);
      }
    } else {
      code_params = comp_rvalue(params, RVALUE_CTX_BASE);
    }
  } else {
    code_params = wrap_str("");
  }

  return string_concat5(
    function_name(get_val(name)),
    wrap_char(' '),
    env_var(assign_to),
    wrap_char(' '),
    code_params
  );
}

void comp_fun_call(ast node, ast assign_to) {
  append_glo_decl(comp_fun_call_code(node, assign_to));
}

void comp_assignment(ast lhs, ast rhs) {
  int lhs_op = get_op(lhs);
  if (lhs_op == IDENTIFIER OR lhs_op == '[' OR lhs_op == '*' OR lhs_op == ARROW) {
    if (get_op(rhs) == '(') {
      comp_fun_call(rhs, lhs);
    } else {
      /*
        Disabled because of arithmetic precision issues with some shells.
        It looks like assignments in $((...)) are done with more bits with ksh.
      */
      /* If lhs is an identifier, we generate x=$(( ... )) instead of : $(( x = ... )) */
      /* if (lhs_op == IDENTIFIER) {
        append_glo_decl(string_concat3(comp_lvalue(lhs), wrap_char('='), comp_rvalue(rhs, RVALUE_CTX_BASE)));
      } else {
        append_glo_decl(string_concat5(wrap_str(": $(( "), comp_lvalue(lhs), wrap_str(" = "), comp_rvalue(rhs, RVALUE_CTX_ARITH_EXPANSION), wrap_str(" ))")));
      }
      */
     append_glo_decl(string_concat5(wrap_str(": $(( "), comp_lvalue(lhs), wrap_str(" = "), comp_rvalue(rhs, RVALUE_CTX_ARITH_EXPANSION), wrap_str(" ))")));
    }
  } else {
    printf("lhs_op=%d %c\n", lhs_op, lhs_op);
    fatal_error("unknown lhs");
  }
}

void comp_body(ast node) {
  int i;
  int start_in_tail_position = in_tail_position;
  in_tail_position = false;

  if (node != 0) {
    while (get_op(node) == '{') {
      /* Last statement of body is in tail position if the body itself is in tail position */
      if (get_op(get_child(node, 1)) != '{') in_tail_position = start_in_tail_position;
      comp_statement(get_child(node, 0), false);
      node = get_child(node, 1);
    }
  }
}

void comp_statement(ast node, int else_if) {
  int op = get_op(node);
  text str;
  int start_loop_end_actions_start;
  int start_loop_end_actions_end;

  gensym_ix = 0;

  if (op == IF_KW) {
    /* TODO: Replace this with ternary expression? */
    if (else_if) {
      append_glo_decl(string_concat3(
            wrap_str("elif "),
            comp_rvalue(get_child(node, 0), RVALUE_CTX_TEST),
            wrap_str(" ; then")
          ));
    } else {
      append_glo_decl(string_concat3(
            wrap_str("if "),
            comp_rvalue(get_child(node, 0), RVALUE_CTX_TEST),
            wrap_str(" ; then")
          ));
    }

    nest_level += 1;
    if (get_child(node, 1) != 0) { comp_statement(get_child(node, 1), false); }
    else { append_glo_decl(wrap_char(':')); }
    nest_level -= 1;

    if (get_child(node, 2) != 0) {
      /* Compile sequence of if else if using elif */
      if (get_op(get_child(node, 2)) == IF_KW) {
        comp_statement(get_child(node, 2), true); /* comp_statement with else_if == true emits elif*/
      } else {
        append_glo_decl(wrap_str("else"));
        /* TODO: Emit : if body is empty */
        nest_level += 1;
        comp_statement(get_child(node, 2), false);
        nest_level -= 1;
      }
    }
    if (!else_if) append_glo_decl(wrap_str("fi"));
  } else if (op == WHILE_KW) {
    append_glo_decl(string_concat3(
      wrap_str("while "),
      comp_rvalue(get_child(node, 0), RVALUE_CTX_TEST),
      wrap_str(" ; do")
    ));

    loop_nesting_level += 1;
    nest_level += 1;
    if (get_child(node, 1) != 0) { comp_statement(get_child(node, 1), false); }
    else { append_glo_decl(wrap_char(':')); }
    nest_level -= 1;
    loop_nesting_level -= 1;

    append_glo_decl(wrap_str("done"));
  } else if (op == FOR_KW) {
    /* Save loop end actions from possible outer loop */
    start_loop_end_actions_start = loop_end_actions_start;
    start_loop_end_actions_end = loop_end_actions_end;

    if (get_child(node, 0)) comp_statement(get_child(node, 0), false);

    str = wrap_char(':'); /* Empty statement */
    if (get_child(node, 1)) {
      str = comp_rvalue(get_child(node, 1), RVALUE_CTX_TEST);
    }

    append_glo_decl(string_concat3(wrap_str("while "), str, wrap_str(" ; do")));

    /*
      This is a little bit of a hack, but it makes things so much simpler.
      Because we need to capture the code for the end of loop actions
      (increment, etc.), and those can be any statement expression, we somehow
      need to get the text generated by the call to comp_statement. Instead of
      modifying comp_statement to accomodate this, we just remove the code
      generated by comp_statement using undo_glo_decls and save the indices of
      the declarations of the loop end actions so they can replayed later.
    */
    loop_end_actions_start = glo_decl_ix;
    if (get_child(node, 2)) comp_statement(get_child(node, 2), false);
    undo_glo_decls(loop_end_actions_start);
    loop_end_actions_end = glo_decl_ix;

    loop_nesting_level += 1;
    nest_level += 1;
    if (get_child(node, 3) != 0) { comp_statement(get_child(node, 3), false); }
    else if (get_child(node, 2) == 0) { append_glo_decl(wrap_char(':')); }
    replay_glo_decls(loop_end_actions_start, loop_end_actions_end, true);

    nest_level -= 1;
    loop_nesting_level -= 1;
    loop_end_actions_start = start_loop_end_actions_start;
    loop_end_actions_end = start_loop_end_actions_end;

    append_glo_decl(wrap_str("done"));
  }  else if (op == BREAK_KW) {
    if (loop_nesting_level == 0) fatal_error("comp_statement: break not in loop");
    /* TODO: What's the semantic of break? Should we run the end of loop action before breaking? */
    append_glo_decl(wrap_str("break"));
  } else if (op == CONTINUE_KW) {
    if (loop_nesting_level == 0) fatal_error("comp_statement: continue not in loop");
    replay_glo_decls(loop_end_actions_start, loop_end_actions_end, true);
    /* We could remove the continue when in tail position, but it's not worth doing */
    append_glo_decl(wrap_str("continue"));
  } else if (op == RETURN_KW) {
    if (get_child(node, 0) != 0) {
      append_glo_decl(string_concat3(
        wrap_str(": $(( $1 = "),
        comp_rvalue(get_child(node, 0), RVALUE_CTX_ARITH_EXPANSION),
        wrap_str(" ))")
      ));
    }
    if (in_tail_position AND loop_nesting_level == 1) {
      append_glo_decl(wrap_str("break")); /* Break out of the loop, and the function prologue will do the rest */
    } else if (!in_tail_position OR loop_nesting_level != 0) {
      restore_local_vars();
      append_glo_decl(wrap_str("return"));
    } else {
      /* TODO: Make sure this can't create empty bodies */
    }
  } else if (op == '(') { /* six.call */
    comp_fun_call(node, new_ast0(IDENTIFIER_EMPTY, 0)); /* Reuse IDENTIFIER_EMPTY ast? */
  } else if (op == '{') { /* six.compound */
    comp_body(node);
  } else if (op == '=') { /* six.x=y */
    comp_assignment(get_child(node, 0), get_child(node, 1));
  } else if (op == VAR_DECL) {
    fatal_error("Variable declaration must be at the beginning of a function");
  } else {
    str = comp_rvalue(node, RVALUE_CTX_BASE);
    if (contains_side_effects) {
      append_glo_decl(string_concat(wrap_str(": "), str));
    }
  }
}

ast get_leading_var_declarations(ast node) {
  ast result = 0;
  ast local_var;
  ast tail;
  ast new_tail;

  if (get_op(node) == '{') {
    while (get_op(node) == '{') {
      local_var = get_child(node, 0);
      if (get_op(local_var) != VAR_DECL) break;

      /* Initialize list */
      if (result == 0) {
        result = new_ast2(',', local_var, 0);
        tail = result;
      } else {
        new_tail = new_ast2(',', local_var, 0);
        set_child(tail, 1, new_tail);
        tail = new_tail;
      }

      node = get_child(node, 1);
    }
  }

  return new_ast2(',', result, node);
}

void comp_glo_define_procedure(ast node) {
  ast name = get_child(node, 0);
  /* ast fun_type = get_child(node, 1); */
  ast params = get_child(node, 2);
  ast local_vars_and_body = get_leading_var_declarations(get_child(node, 3));
  ast local_vars = get_child(local_vars_and_body, 0);
  ast body = get_child(local_vars_and_body, 1);
  text comment = 0;
  int i;
  int body_start_decl_ix;
  int body_end_decl_ix;
  ast var;

  assert_idents_are_safe(params);
  assert_idents_are_safe(local_vars);

  add_vars_to_local_env(params, 2); /* Start position at 2 because 1 is taken by result_loc */
  add_vars_to_local_env(local_vars, local_env_size + 2);

  /* TODO: Analyze vars that are mutable */

  /* Show the mapping between the function parameters and $1, $2, etc. */
  i = 2; /* Start at 2 because $1 is assigned to result location */
  while (params != 0) {
    var = get_child(params, 0);
    comment = concatenate_strings_with(comment, string_concat3(env_var(new_ast0(IDENTIFIER, get_child(var, 0))), wrap_str(": $"), wrap_int(i)), wrap_str(", "));
    params = get_child(params, 1);
    i += 1;
  }
  if (comment != 0) comment = string_concat(wrap_str(" # "), comment);

  append_glo_decl(string_concat3(
    function_name(name),
    wrap_str("() {"),
    comment
  ));

  in_tail_position = true;
  fun_gensym_ix = 0;
  nest_level += 1;

  /*
    After setting the environment, we compile the body of the function and then
    call save-local-variables so it can save the local variables and synthetic
    variables that were used in the function.
  */
  body_start_decl_ix = glo_decl_ix;
  comp_body(body);
  body_end_decl_ix = glo_decl_ix;
  undo_glo_decls(body_start_decl_ix);

  save_local_vars();

  /* Initialize parameters */
  params = get_child(node, 2); /* Reload params because params is now = 0 */
  i = 2;
  while (params != 0) {
    var = get_child(params, 0);
    comp_assignment(new_ast0(IDENTIFIER, get_child(var, 0)), new_ast0(IDENTIFIER_DOLLAR, i));
    params = get_child(params, 1);
    i += 1;
  }

  /* Initialize local vars */
  while (local_vars != 0) {
    var = get_child(local_vars, 0);
    /* TODO: Replace with ternary expression? */
    if (get_child(var, 2) == 0) { comp_assignment(new_ast0(IDENTIFIER, get_child(var, 0)), new_ast0(INTEGER, 0)); }
    else { comp_assignment(new_ast0(IDENTIFIER, get_child(var, 0)), get_child(var, 2)); }
    local_vars = get_child(local_vars, 1);
  }

  replay_glo_decls(body_start_decl_ix, body_end_decl_ix, false);

  restore_local_vars();

  nest_level -= 1;

  append_glo_decl(wrap_str("}\n"));

  /* Reset local env */
  local_env_size = 0;
  local_env = 0;
}

text comp_constant(ast node) {
  int op = get_op(node);
  if (op == INTEGER) {
    return wrap_int(-get_val(node));
  } else if (op == CHARACTER) {
    return string_concat(wrap_char('$'), character_ident(get_val(node)));
  } else if (op == STRING) {
    return wrap_str(string_pool + get_val(node));
  } else if ((op == '-') AND get_nb_children(node) == 1) {
    return string_concat(wrap_char('-'), comp_constant(get_child(node, 0)));
  } else {
    fatal_error("comp_constant: unknown constant");
  }
}

void comp_glo_variable_declaration(ast node) {
  ast name = get_child(node, 0);
  ast type = get_child(node, 1);
  ast init = get_child(node, 2);

  text init_text;

  if (get_op(type) == '[') { /* Array declaration */
    append_glo_decl(
      string_concat4(
        wrap_str("defarr "),
        env_var(new_ast0(IDENTIFIER, name)),
        wrap_char(' '),
        wrap_int(get_val(get_child(type, 0)))
      )
    );
  } else {
    /* TODO: Replace with ternary expression? */
    if (init != 0) {
      init_text = comp_constant(init);
    } else {
      init_text = wrap_char('0');
    }
    append_glo_decl(
      string_concat4(
        wrap_str("defglo "),
        env_var(new_ast0(IDENTIFIER, name)),
        wrap_char(' '),
        init_text
      )
    );
  }
}

/*
This function compiles 1 top level declaration at the time.
The 3 types of supported top level declarations are:
  - global variable declarations
  - global variable assignations
  - function declarations
Structures, enums, and unions are not supported.
*/
void comp_glo_decl(ast node) {
  int op = get_op(node);

  if (op == '=') { /* Assignations */
   comp_assignment(get_child(node, 0), get_child(node, 1));
  } else if (op == VAR_DECL) {
    comp_glo_variable_declaration(node);
  } else if (op == FUN_DECL) {
    comp_glo_define_procedure(node);
  } else {
    printf("op=%d %c with %d children\n", op, op, get_nb_children(node));
    fatal_error("comp_glo_decl: unexpected declaration");
  }
}

int main() {

  ast node;

  init_ident_table();

  ch = '\n';
  get_tok();

  while (tok != EOF) {
    comp_glo_decl(parse_definition(0));
    print_glo_decls();
    /* Reset state */
    glo_decl_ix = 0;
    /* TODO: Clear heap */
  }

  printf("// string_pool_alloc=%d heap_alloc=%d\n", string_pool_alloc, heap_alloc);
  return 0;
}
