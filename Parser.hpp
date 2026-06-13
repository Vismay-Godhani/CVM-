#ifndef __Parser_h
#define __Parser_h
#include"Lexer.hpp"
#include<iostream>
#include<vector>

using namespace std;

enum Node_Type{
    NODE_ROOT,

    NODE_INT,
    NODE_BOOL,
    NODE_VARIABLE,
    NODE_STRING,

    NODE_ADD,
    NODE_SUBTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULUS,

    NODE_ASSIGN,
    NODE_PRINT,
    NODE_INPUT,

    NODE_IF,
    NODE_ELSE,
    NODE_WHILE,

    NODE_LESSTHAN,
    NODE_LESSTHANEQUALS,
    NODE_GREATERTHAN,
    NODE_GREATERTHANEQUALS,
    NODE_DOUBLEEQUALS,
    NODE_NOTEQUALS,

    NODE_AND,
    NODE_OR,
    NODE_NOT
};

string NodeTypeToString(Node_Type type) {
    switch(type) {
        case NODE_ROOT:
        return "ROOT";
        case NODE_INT:
        return "INT";
        case NODE_BOOL:
        return "BOOL";
        case NODE_VARIABLE:
        return "VARIABLE";
        case NODE_STRING:
        return "STRING";
        case NODE_ADD:
        return "ADD";
        case NODE_SUBTRACT:
        return "SUBTRACT";
        case NODE_MULTIPLY:
        return "MULTIPLY";
        case NODE_DIVIDE:
        return "DIVIDE";
        case NODE_MODULUS:
        return "MODULUS";
        case NODE_ASSIGN:
        return "ASSIGN";
        case NODE_PRINT:
        return "PRINT";
        case NODE_INPUT:
        return "INPUT";
        case NODE_IF:
        return "IF";
        case NODE_ELSE:
        return "ELSE";
        case NODE_WHILE:
        return "WHILE";
        case NODE_LESSTHAN:
        return "LESSTHAN";
        case NODE_LESSTHANEQUALS:
        return "LESSTHANEQUALS";
        case NODE_GREATERTHAN:
        return "GREATERTHAN";
        case NODE_GREATERTHANEQUALS:
        return "GREATERTHANEQUALS";
        case NODE_DOUBLEEQUALS:
        return "DOUBLEEQUALS";
        case NODE_NOTEQUALS:
        return "NOTEQUALS";
        case NODE_AND:
        return "AND";
        case NODE_OR:
        return "OR";
        case NODE_NOT:
        return "NOT";
        default:
        return "UNKNOWN_NODE";
    }
}

struct AST_NODE{
    enum Node_Type TYPE;
    string VALUE;
    AST_NODE*Left = nullptr;
    AST_NODE*Right = nullptr;
    vector<AST_NODE*> Children;
};

class Parser{
    public:
    Parser(vector<Token*> Tokenstream){
        tokens=Tokenstream;
        current=0;
    }
    
    AST_NODE* CreateNode(Node_Type TYPE,string VALUE=""){
        AST_NODE* NODE=new AST_NODE;
        NODE->TYPE=TYPE;
        NODE->VALUE=VALUE;
        return NODE;
    }

    void PrintAST(AST_NODE* node, int indent = 0) const {
        if (!node) return;
        for (int i = 0; i < indent; i++) cout << "  ";
        cout << NodeTypeToString(node->TYPE);
        if (!node->VALUE.empty()) {
            cout << " : " << node->VALUE;
        }
        cout << endl;
        if (node->Left) PrintAST(node->Left, indent + 1);
        if (node->Right) PrintAST(node->Right, indent + 1);
        for (auto* child : node->Children) {
            PrintAST(child, indent + 1);
        }
    }

    bool IsatEnd(){
        return Peek()->TYPE==TOKEN_EOF;
    }
    Token* Peek(){
        return tokens[current];
    }
    Token* Previous(){
        return tokens[current-1];
    }

    Token* Advance(){
        if(!IsatEnd()){
            current++;
        }
        return Previous();
    }

    bool Match(enum types TYPE){
        if(IsatEnd()) return false;
        if(Peek()->TYPE!=TYPE) return false;
        Advance();
        return true;
    }

    void Consume(enum types TYPE,string ERROR){
        if(Peek()->TYPE==TYPE){
            Advance();
            return;
        }
        cout<<"ERROR : "<<ERROR<<" at line "<<Peek()->LineNum<<endl;
        exit(1);
    }

    void OptionalNewLine(){
        if(!IsatEnd() && Peek()->TYPE==TOKEN_NEWLINE){
            Advance();
        }
    }

    AST_NODE* ParseCondition(){
        return ParseLogicalOr();
    }

    AST_NODE* ParseLogicalOr(){
        AST_NODE* NODE=ParseLogicalAnd();
        while(!IsatEnd() && Peek()->TYPE==TOKEN_OR){
            Advance();
            AST_NODE* RIGHT=ParseLogicalAnd();
            AST_NODE* Parent=CreateNode(NODE_OR);
            Parent->Left=NODE;
            Parent->Right=RIGHT;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseLogicalAnd(){
        AST_NODE* NODE=ParseEquality();
        while(!IsatEnd() && Peek()->TYPE==TOKEN_AND){
            Advance();
            AST_NODE* RIGHT=ParseEquality();
            AST_NODE* Parent=CreateNode(NODE_AND);
            Parent->Left=NODE;
            Parent->Right=RIGHT;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseEquality(){
        AST_NODE* NODE=ParseComparison();
        while(!IsatEnd() && (Peek()->TYPE==TOKEN_DOUBLEEQUALS || Peek()->TYPE==TOKEN_NOTEQUALS)){
            Token* Operator=Advance();
            AST_NODE* RIGHT=ParseComparison();
            AST_NODE* Parent;
            if(Operator->TYPE==TOKEN_DOUBLEEQUALS){
                Parent=CreateNode(NODE_DOUBLEEQUALS);
            }
            else{
                Parent=CreateNode(NODE_NOTEQUALS);
            }
            Parent->Left=NODE;
            Parent->Right=RIGHT;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseComparison(){
        AST_NODE* NODE=ParseExpression();
        while(!IsatEnd() && (Peek()->TYPE==TOKEN_LESSTHAN || Peek()->TYPE==TOKEN_LESSTHANEQUALS ||  Peek()->TYPE==TOKEN_GREATERTHAN || Peek()->TYPE==TOKEN_GREATERTHANEQUALS)){
            Token* Operator=Advance();
            AST_NODE* RIGHT=ParseExpression();
            AST_NODE* Parent;
            if(Operator->TYPE==TOKEN_LESSTHAN){
                Parent=CreateNode(NODE_LESSTHAN);
            }
            else if(Operator->TYPE==TOKEN_LESSTHANEQUALS){
                Parent=CreateNode(NODE_LESSTHANEQUALS);
            }
            else if(Operator->TYPE==TOKEN_GREATERTHAN){
                Parent=CreateNode(NODE_GREATERTHAN);
        }
            else{
                Parent=CreateNode(NODE_GREATERTHANEQUALS);
            }
            
            Parent->Left=NODE;
            Parent->Right=RIGHT;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseExpression(){
        AST_NODE* NODE=ParseTerm();
        while(Peek()->TYPE==TOKEN_PLUS || Peek()->TYPE==TOKEN_MINUS){
            Token* Operator=Advance();
            AST_NODE* Right=ParseTerm();
            AST_NODE* Parent;

            if(Operator->TYPE==TOKEN_PLUS){
                Parent=CreateNode(NODE_ADD);
            }
            else{
                Parent=CreateNode(NODE_SUBTRACT);
            }
            
            Parent->Left=NODE;
            Parent->Right=Right;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseTerm(){
        AST_NODE* NODE=ParseFactor();
        while(Peek()->TYPE==TOKEN_MULTIPLY || Peek()->TYPE==TOKEN_DIVIDE || Peek()->TYPE==TOKEN_MODULO){
            Token* Operator=Advance();
            AST_NODE* Right=ParseFactor();
            AST_NODE* Parent;
            if(Operator->TYPE==TOKEN_MULTIPLY){
                Parent=CreateNode(NODE_MULTIPLY);
            }
            else if(Operator->TYPE==TOKEN_DIVIDE){
                Parent=CreateNode(NODE_DIVIDE);
            }
            else{
                Parent=CreateNode(NODE_MODULUS);
            }

            Parent->Left=NODE;
            Parent->Right=Right;
            NODE=Parent;
        }
        return NODE;
    }

    AST_NODE* ParseFactor(){
        if(Match(TOKEN_NOT)){
            AST_NODE* NODE=CreateNode(NODE_NOT);
            NODE->Right=ParseFactor();
            return NODE;
        }
        if(Match(TOKEN_INT)){
            return CreateNode(NODE_INT,Previous()->VALUE);
        }
        if(Match(TOKEN_BOOL)){
            return CreateNode(NODE_BOOL,Previous()->VALUE);
        }
        if(Match(TOKEN_ID)){
            return CreateNode(NODE_VARIABLE,Previous()->VALUE);
        }
        if(Match(TOKEN_LEFT_SBRACKET)){
            AST_NODE* Expression=ParseExpression();
            Consume(TOKEN_RIGHT_SBRACKET,"Expected ')'");
            return Expression;
        }
        if(Match(TOKEN_STRING)){
            return CreateNode(NODE_STRING, Previous()->VALUE);
        }
        if(Match(TOKEN_INPUT)){
            return CreateNode(NODE_INPUT);
        }

        cout<<"[!] Expected expression at line "<<Peek()->LineNum<<endl;
        exit(1);
    }
    
    AST_NODE* ParseStatement(){
        if(Match(TOKEN_PRINT)){
            return ParsePrint();
        }
        else if(Match(TOKEN_IF)){
            return ParseIf();
        }
        else if(Match(TOKEN_WHILE)){
            return ParseWhile();
        }
        else if(Peek()->TYPE==TOKEN_ID){
            return ParseAssignment();
        }
        else{
            cout<<"ERROR Invalid Statement at line "<<Peek()->LineNum<<endl;
            exit(1);
        }
    }


    AST_NODE* ParseIf(){
        AST_NODE* NODE=CreateNode(NODE_IF);
        NODE->Left=ParseCondition();
        Consume(TOKEN_COLON,"Expected ':' after if condition");
        Consume(TOKEN_NEWLINE,"Expected newline");
        Consume(TOKEN_INDENT,"Expected indent");

        while(!IsatEnd() && Peek()->TYPE!=TOKEN_DEDENT){
            if(Peek()->TYPE==TOKEN_NEWLINE){
                Advance();
                continue;
            }
            NODE->Children.push_back(ParseStatement());
        }
        Consume(TOKEN_DEDENT,"Expected dedent");

        if(Match(TOKEN_ELSE)){
            AST_NODE* ELSENODE=CreateNode(NODE_ELSE);
            Consume(TOKEN_COLON,"Expected ':' after else condition");
            Consume(TOKEN_NEWLINE,"Expected newline");
            Consume(TOKEN_INDENT,"Expected indent");

            while(!IsatEnd() && Peek()->TYPE!=TOKEN_DEDENT){
                if(Peek()->TYPE==TOKEN_NEWLINE){
                    Advance();
                    continue;
                }
                ELSENODE->Children.push_back(ParseStatement());
            }
            Consume(TOKEN_DEDENT,"Expected dedent");
            NODE->Right=ELSENODE;
        }

        return NODE;
    }

    AST_NODE* ParseWhile(){
        AST_NODE* NODE=CreateNode(NODE_WHILE);
        NODE->Left=ParseCondition();
        Consume(TOKEN_COLON,"Expected ':' after while");
        Consume(TOKEN_NEWLINE,"Expected newline");
        Consume(TOKEN_INDENT,"Expected indent");

        while(!IsatEnd() && Peek()->TYPE!=TOKEN_DEDENT){
            if(Peek()->TYPE==TOKEN_NEWLINE){
                Advance();
                continue;
            }
            NODE->Children.push_back(ParseStatement());
        }
        Consume(TOKEN_DEDENT,"Expected dedent");

        return NODE;
    }

    AST_NODE* ParseAssignment(){
        Token* Identifier=Advance();
        Consume(TOKEN_EQUAL,"Expected '=' after identifier");
        AST_NODE* Expression=ParseExpression();
        AST_NODE* NODE=CreateNode(NODE_ASSIGN,Identifier->VALUE);
        NODE->Right=Expression;
        OptionalNewLine();
        return NODE;
    }

    AST_NODE* ParsePrint(){
        AST_NODE* NODE=CreateNode(NODE_PRINT);
        NODE->Right=ParseExpression();
        OptionalNewLine();
        return NODE;
    }

    AST_NODE* Parse(){
        AST_NODE* Root = CreateNode(NODE_ROOT);
        while(!IsatEnd()){
            if(Peek()->TYPE==TOKEN_NEWLINE){
                Advance();
                continue;
            }
            Root->Children.push_back(ParseStatement());
        }
        return Root;
    }

    private:
    vector<Token*> tokens;
    int current;
};

#endif