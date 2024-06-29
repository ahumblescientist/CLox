#include "compiler.h"
#include "scanner.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct {
	Token current;
	Token prev;
	bool hadError;
	bool panicMode;
} Parser;

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

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;


static void binary(), grouping(), unary(), number();
static void expression();
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
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
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


static void errorAt(Token *token, char *msg) {
	if(parser.panicMode) return;
	parser.panicMode = true;
	printf("[line %d] Error", token->line);
	if(token->type == TOKEN_EOF) {
		printf(" at end");
	} else if(token->type == TOKEN_ERROR) {
		
	} else {
		printf("at '%.*s'", token->length, token->start);
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

static uint8_t makeConstant(Value value) {
	uint16_t index = addConstant(currentChunk(), value);
	if(index > 255) {
		error("Too many Constants in one chunk");
		return 0;
	}
	return (int8_t)index;
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
	}
  prefixRule();
	while(precedence <= getRule(parser.current.type)->precedence) {
		advance();
		ParseFn infixRule = getRule(parser.prev.type)->infix;
		infixRule();
	}
}

static void number() {
	double value = strtod(parser.prev.start, NULL);
	emitConstant(value);
}

static void grouping() {
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

static void unary() {
	TokenType operatorType = parser.prev.type;

	parsePrecedence(PREC_UNARY);

	switch(operatorType) {
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		default:
			break;

	}
}

static void binary() {
	TokenType operatorType = parser.prev.type;
	ParseRule *rule = getRule(operatorType);
	parsePrecedence((Precedence)(rule->precedence+1));
	switch(operatorType) {
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    default: return; // Unreachable.
	}
}


static void expression() {
  parsePrecedence(PREC_ASSIGNMENT);	
}

bool compile(Chunk *chunk, char *source) {
	initScanner(source);
  compillingChunk = chunk;
	parser.panicMode = false;
	parser.hadError = false;
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");
	endCompiler();
	return !parser.hadError;	
}
