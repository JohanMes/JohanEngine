#ifndef PARSER_INCLUDE
#define PARSER_INCLUDE

#include <cstdio>
#include <vector>
using std::vector;
#include "float3.h"
#include "Variable.h"

enum TokenKind {
    ttWhile,
    ttIf,
    ttElse,
    ttOpenParenth, // (
    ttCloseParenth, // )
    ttOpenBracket, // {
    ttCloseBracket, // }
    ttOpenSquare, // [
    ttCloseSquare, // ]
    ttVarType,
    ttUnknown,
    ttMulDiv,
    ttAddSub,
    ttAssign,
    ttMember,
};

class Var;

class Token {
	public:
		~Token() {
			if(text) {
				delete[] text;
			}
			if(data) {
				delete data;
			}
		}

		char* text;
		TokenKind type;
		unsigned int complement;
		unsigned int line;
		Var* data; // used to store temp values in Parser::Evaluate
};

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Parser {
	private:
		vector<Var*> vars;
		vector<Token*> tokens;
		unsigned int index;
		unsigned int line;
		int level;
		bool printtiming;
		bool savetokens;
		
		// remove variables if out of scope
		void IncScope();
		void DecScope();

		// Determine value of text
		int GetIntValue(const char* text);
		float GetFloatValue(const char* text);
		char* GetStringValue(const char* text);
		float3* GetFloat3Value(const char* text);
		Var* GetVariable(const char* text);

		// Determine value of text (including temps from Evaluate)
		int GetTokenIntValue(Token* token);
		float GetTokenFloatValue(Token* token);
		float3* GetTokenFloat3Value(Token* token);
		VarKind GetTokenKind(Token* token);

		// All-in-one for evaluating statements
		void GetExpression(std::vector<Token*>& result);
		Var* Evaluate(std::vector<Token*>& tokens);
		Var* Evaluate(Token* token); // much faster

		// Execute statements and modify index
		void HandleVar();
		Var* HandleFunction(Var* parent);
		void HandleAssignment(Var* lvalue);
		bool HandleBoolean();

		void Clear();
		void Tokenize(const char* file);
		void Optimize();
		void Parse();

		// Tokenizer helper
		void AddToken(int length,char* ptr);
	public:
		Parser(bool printtiming,bool savetokens);
		Parser(const char* file);
		~Parser();
		void AddArg(Var* value);
		void Execute(const char* file);
};

#endif
