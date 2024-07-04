#include "compiler.h"
#include "scanner.h"
#include "obj.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Token name;
  int depth;
} Local;

typedef struct {
  Local locals[256];
  int localCount;
  int scopeDepth;
} Compiler;

typedef struct {
	Token current;
	Token prev;
	bool hadError;
	bool panicMode;
} Parser;

Compiler *current = NULL;

Parser parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool );

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;


static void binary(bool), grouping(bool), unary(bool), number(bool), literal(bool), string(bool), variable(bool);
static void expression(), decleration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {variable,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {string,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {literal,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {literal,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {literal,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};


static ParseRule* getRule(TokenType type) {
  return &rules[type];
}


Chunk *compillingChunk;

static Chunk *currentChunk() {
	return compillingChunk;
}

static void initCompiler(Compiler *compiler) {
  compiler->scopeDepth = 0;
  compiler->localCount = 0;
  current = compiler;
}

static void errorAt(Token *token, char *msg) {
	if(parser.panicMode) return;
	parser.panicMode = true;
	printf("[line %d] Error", token->line);
	if(token->type == TOKEN_EOF) {
		printf(" at end");
	} else if(token->type == TOKEN_ERROR) {
		
	} else {
		printf(" at '%.*s'", token->length, token->start);
	}
	printf(": %s\n", msg);
	parser.hadError = true;
}

static void error(char *msg) {
	errorAt(&parser.prev, msg);
}

static void errorAtCurrent(char *msg) {
	errorAt(&parser.current, msg);
}

static void advance() {
	parser.prev = parser.current;
	while(1) {
		parser.current = scanToken();
		if(parser.current.type == TOKEN_ERROR) errorAtCurrent(parser.current.start);
    break;
	}
}

static void consume(TokenType type, char *msg) {
	if(parser.current.type != type) {
		errorAtCurrent(msg);
		return;
	}
	advance();
	return;
}

static bool check(TokenType type) {
  return parser.current.type == type;
}

static bool match(TokenType type) {
  if(check(type)) {
    advance();
    return true;
  }
  return false;
}

static uint8_t makeConstant(Value value) {
	uint16_t index = addConstant(currentChunk(), value);
	if(index > 255) {
		error("Too many Constants in one chunk");
		return 0;
	}
	return (int8_t)index;
}

static void sync() {
  parser.panicMode = false;
  while(parser.current.type != TOKEN_EOF) {
    if(parser.prev.type == TOKEN_SEMICOLON) return;
    switch(parser.current.type) {
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;
      default: break;
    }
    advance();
  }
}


static void emitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte, parser.prev.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
	emitByte(byte1);
	emitByte(byte2);
}

static void emitConstant(Value value) {
	emitBytes(OP_CONSTANT, makeConstant(value));
}

static void emitReturn() {
	emitByte(OP_RETURN);
}

static void endCompiler() {
	emitReturn();
}

static void parsePrecedence(Precedence precedence) {
	advance();
	ParseFn prefixRule = getRule(parser.prev.type)->prefix;
	if(prefixRule == NULL) {
		error("Expected expression");
    return;
	}
  bool canAssign = precedence <= PREC_ASSIGNMENT;
  prefixRule(canAssign);
	while(precedence <= getRule(parser.current.type)->precedence) {
		advance();
		ParseFn infixRule = getRule(parser.prev.type)->infix;
		infixRule(canAssign);
	}
  if(canAssign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target");
  }
}

static void number(bool canAssign) {
	double value = strtod(parser.prev.start, NULL);
	emitConstant(NUMBER_VAL(value));
}

static void literal(bool canAssign) {
  switch(parser.prev.type) {
    case TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    case TOKEN_NIL:
      emitByte(OP_NIL);
      break;
    default: break;
  }
}

static void grouping(bool canAssign) {
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

static void unary(bool canAssign) {
	TokenType operatorType = parser.prev.type;

	parsePrecedence(PREC_UNARY);

	switch(operatorType) {
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		default:
			break;

	}
}

static void string(bool canAssign) {
  emitConstant(OBJ_VAL(copyString(parser.prev.start + 1, parser.prev.length - 2)));
}

static void binary(bool canAssign) {
	TokenType operatorType = parser.prev.type;
	ParseRule *rule = getRule(operatorType);
	parsePrecedence((Precedence)(rule->precedence+1));
	switch(operatorType) {
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TOKEN_BANG_EQUAL:   emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
    case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS:          emitByte(OP_LESS); break;
    case TOKEN_GREATER:       emitByte(OP_GREATER); break;
    default: return; // Unreachable.
	}
}


static void expression() {
  parsePrecedence(PREC_ASSIGNMENT);	
}

static void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "expect ';' after expression");
  emitByte(OP_POP);
}

static void printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression");
  emitByte(OP_PRINT);
}

static void block() {
  while(!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    decleration();
  }
  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

static void beginScope() {
  current->scopeDepth++;
}

static void endScope() {
  current->scopeDepth--;
  while(current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
    emitByte(OP_POP);
    current->localCount--;
  }
}

static void statement() {
  if(match(TOKEN_PRINT)) {
    printStatement();
  } else if(match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

static uint8_t identifierConstant(Token *token) {
  return makeConstant(OBJ_VAL(copyString(token->start, token->length)));
}

static bool identifiersEqual(Token *a1, Token *a2) {
  if(a1->length != a2->length || memcmp(a1->start, a2->start, a1->length)) return false;
  return true;
}

static void addLocal(Token name) {
  if(current->localCount >= UINT8_MAX) {
    error("Too many local variables in one block.");
    return;
  }
  Local *local = current->locals + current->localCount++;
  local->name = name;
  local->depth = -1;
}

static void declareVariable() {
  if(current->scopeDepth == 0) return;
  Token *name = &parser.prev;
  for(int i=current->localCount-1;i>=0;i--) {
    Local *local = &current->locals[i];
    if(local->depth != -1 && local->depth < current->scopeDepth) {
      break;
    }
    if(identifiersEqual(name, &local->name)) {
      error("Already a variable with the same name in this scope.");
    } 
  }
  addLocal(*name);
}

static uint8_t parseVariable(char *msg) {
  consume(TOKEN_IDENTIFIER, msg); // consume identifier name and make it prev
  declareVariable();
  if(current->scopeDepth > 0) return 0;
  return identifierConstant(&parser.prev);
}

static void markInit() {
  current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(uint8_t id) {
  if(current->scopeDepth > 0) {
    markInit();
    return;
  }
  emitBytes(OP_DEFINE_GLOBAL, id);
}

static void varDecleration() {
  uint8_t global = parseVariable("expected Variable name");
  if(match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON,"Expect ';' after variable decleration");
  defineVariable(global);
}

static int resloveLocal(Compiler *compiler, Token *name) {
  for(int i=compiler->localCount-1;i>=0;i--) {
    Local *local = &(compiler->locals[i]);
    if(identifiersEqual(&local->name, name)) {
      if(local->depth == -1) {
        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }
  return -1;
}

static void namedVariable(Token name, bool canAssign) {
  uint8_t getOp, setOp;
  int arg = resloveLocal(current, &name);
  if(arg != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_GET_LOCAL;
  } else {
    arg = identifierConstant(&name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }
  if(canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitBytes(setOp, (uint8_t)arg);
  } else {
    emitBytes(getOp, (uint8_t)arg);
  }
}

static void variable(bool canAssign) {
  namedVariable(parser.prev, canAssign);
}

static void decleration() {
  if(match(TOKEN_VAR)) {
    varDecleration();
  } else {
    statement();
  }
  if(parser.panicMode) sync();
}

bool compile(Chunk *chunk, char *source) {
  Compiler compiler;
  initCompiler(&compiler);
	initScanner(source);
  compillingChunk = chunk;
	parser.panicMode = false;
	parser.hadError = false;
	advance();
  while(!match(TOKEN_EOF)) {
    decleration();
  }
	endCompiler();
	return !parser.hadError;	
}

