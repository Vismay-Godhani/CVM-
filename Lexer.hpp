#ifndef __Lexer_h
#define __Lexer_h
#include<string>
#include<iostream>
#include<sstream>
#include<vector>
#include<stack>
#include<algorithm>
using namespace std;

enum types{
    TOKEN_ID,

    TOKEN_INT,
    TOKEN_BOOL,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,

    TOKEN_COLON,
    TOKEN_IF,    
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_PRINT,
    TOKEN_INPUT,

    TOKEN_NEWLINE,
    TOKEN_INDENT,
    TOKEN_DEDENT,

    TOKEN_LESSTHAN,
    TOKEN_GREATERTHAN,
    TOKEN_LESSTHANEQUALS,
    TOKEN_GREATERTHANEQUALS,
    TOKEN_DOUBLEEQUALS,
    TOKEN_NOTEQUALS,
    TOKEN_EQUAL,

    TOKEN_OR,
    TOKEN_AND,
    TOKEN_NOT,

    TOKEN_LEFT_SBRACKET,
    TOKEN_RIGHT_SBRACKET,
    TOKEN_STRING,

    TOKEN_EOF
};

string TypeToString(types TYPE){
    switch(TYPE){
        case TOKEN_ID:
        return "TOKEN_ID";
        case TOKEN_INT:
        return "TOKEN_INT";
        case TOKEN_BOOL:
        return "TOKEN_BOOL";
        case TOKEN_PLUS:
        return "TOKEN_PLUS";
        case TOKEN_MINUS:
        return "TOKEN_MINUS";
        case TOKEN_MULTIPLY:
        return "TOKEN_MULTIPLY";
        case TOKEN_DIVIDE:
        return "TOKEN_DIVIDE";
        case TOKEN_MODULO:
        return "TOKEN_MODULO";
        case TOKEN_COLON:
        return "TOKEN_COLON";
        case TOKEN_IF:
        return "TOKEN_IF";
        case TOKEN_ELSE:
        return "TOKEN_ELSE";
        case TOKEN_WHILE:
        return "TOKEN_WHILE";
        case TOKEN_PRINT:
        return "TOKEN_PRINT";
        case TOKEN_NEWLINE:
        return "TOKEN_NEWLINE";
        case TOKEN_INDENT:
        return "TOKEN_INDENT";
        case TOKEN_DEDENT:
        return "TOKEN_DEDENT";
        case TOKEN_INPUT:
        return "TOKEN_INPUT";
        case TOKEN_LESSTHAN:
        return "TOKEN_LESSTHAN";
        case TOKEN_GREATERTHAN:
        return "TOKEN_GREATERTHAN";
        case TOKEN_LESSTHANEQUALS:
        return "TOKEN_LESSTHANEQUALS";
        case TOKEN_GREATERTHANEQUALS:
        return "TOKEN_GREATERTHANEQUALS";
        case TOKEN_DOUBLEEQUALS:
        return "TOKEN_DOUBLEEQUALS";
        case TOKEN_NOTEQUALS:
        return "TOKEN_NOTEQUALS";
        case TOKEN_EQUAL:
        return "TOKEN_EQUAL";
        case TOKEN_OR:
        return "TOKEN_OR";
        case TOKEN_AND:
        return "TOKEN_AND";
        case TOKEN_NOT:
        return "TOKEN_NOT";
        case TOKEN_LEFT_SBRACKET:
        return "TOKEN_LEFT_SBRACKET";
        case TOKEN_RIGHT_SBRACKET:
        return "TOKEN_RIGHT_SBRACKET";
        case TOKEN_STRING:
        return "TOKEN_STRING";
        case TOKEN_EOF:
        return "TOKEN_EOF";
        default:
        return "UNKNOWN_TOKEN";
    }
}

struct Token{
    enum types TYPE;
    string VALUE;
    int LineNum;
    int CharacterNum;
};

class Lexer{
    public:
    Lexer(string SourceCode){
        source=SourceCode;
        cursor=0;
        size=SourceCode.length();
        current=SourceCode.at(cursor);
        LineNum=1;
        CharacterNum=1;
        Indentation.push(0);        
    }

    char advance(){
        if(cursor<size){
            char temp=current;
            cursor++;
            CharacterNum++;
            if(cursor<size){
                current=source[cursor];
            }
            else{
                current='\0';
            }
            return temp;
        }
        else{
            return '\0';
        }
    }

    char CheckForward(int offset){
        if(cursor+offset>=size){
            return '\0';
        }
        else{
            return source[cursor+offset];
        }
    }

    void SkipWhitespace(){
        while(current==' ' || current=='\t' || current=='\r'){
            advance();
        }
    }

    Token* MakeToken(types TYPE,string VALUE){
        Token* newtoken=new Token;
        newtoken->TYPE=TYPE;
        newtoken->VALUE=VALUE;
        newtoken->LineNum=LineNum;
        newtoken->CharacterNum=CharacterNum;
        return newtoken;
    }

    Token* Tokenize_ID(){
        stringstream buffer;
        if(isalpha(current) || current=='_'){
            buffer<<advance();
            while(isalnum(current) || current=='_'){
                buffer<<advance();
            }
        }
        string value=buffer.str();
        if(value=="true" || value=="false"){
            return MakeToken(TOKEN_BOOL,value);
        }
        else if(value=="if"){
            return MakeToken(TOKEN_IF,value);
        }
        else if(value=="else"){
            return MakeToken(TOKEN_ELSE,value);
        }
        else if(value=="while"){
            return MakeToken(TOKEN_WHILE,value);
        }
        else if(value=="print"){
            return MakeToken(TOKEN_PRINT,value);
        }
        else if(value=="input"){
            return MakeToken(TOKEN_INPUT,value);
        }

        return MakeToken(TOKEN_ID,value);
    }

    Token* Tokenize_INT(){
        stringstream buffer;
        while(isdigit(current)){
            buffer<<advance();
        }
        return MakeToken(TOKEN_INT,buffer.str());
    }

    Token* Tokenize_String(){
        advance();
        stringstream buffer;
        while(current!='"'&&current!='\0'){
            if(current=='\n'){
                cout<<"ERROR : unterminated string at line "<<LineNum<<endl;
                exit(1);
            }
            if(current=='\\'){
                advance();
                if(current=='"') {
                    buffer<<'"';
                    advance();
                }
                else if(current == '\\'){
                    buffer<<'\\';
                    advance(); 
                }
                else if(current == 'n'){
                    buffer<<'\n';
                    advance();
                }
                else{
                    buffer<<'\\';
                }
            }
            else{
                buffer << advance();
            }
        }
        if(current=='\0'){
            cout<<"ERROR : unterminated string at line "<<LineNum<<endl;
            exit(1);
        }
        advance();
        return MakeToken(TOKEN_STRING, buffer.str());
    }

    vector<Token*> Tokenize(){
        vector<Token*> tokens;
        while(current!='\0'){
            SkipWhitespace();
            if(current=='\0'){
                break;
            }

            if(current=='#') {
                while(current!='\n'&&current!='\0') {
                    advance();
                }
                continue;
            }

            if(isdigit(current)){
                tokens.push_back(Tokenize_INT());
                continue;
            }

            if(isalpha(current) || current=='_'){
                tokens.push_back(Tokenize_ID());
                continue;
            }

            if(current=='"'){
                tokens.push_back(Tokenize_String());
                continue;
            }

            switch(current){
                case '\n':{
                    tokens.push_back(MakeToken(TOKEN_NEWLINE,"\\n"));
                    while(true){
                        advance();
                        LineNum++;
                        CharacterNum=0;
                        int Spaces=0;
                        while(current==' '){
                            Spaces++;
                            advance();
                        }
                        if(current=='\n'||current=='\r'){
                            continue;
                        }
                        if(current=='\0'){
                            break;
                        }
                        if(Spaces%4!=0){
                            cout<<"ERROR Invalid indentation in Line "<<LineNum<<endl;
                            exit(1);
                        }
                        if(Spaces>Indentation.top()){
                            Indentation.push(Spaces);
                            tokens.push_back(MakeToken(TOKEN_INDENT,"INDENT"));
                        }else if(Spaces<Indentation.top()){
                            while(Spaces<Indentation.top()){
                                Indentation.pop();
                                tokens.push_back(MakeToken(TOKEN_DEDENT,"DEDENT"));
                            }
                        }
                        if(Spaces!=Indentation.top()){
                            cout<<"ERROR Invalid dedent at line "<<LineNum<<endl;
                            exit(1);
                        }
                        break;
                    }
                    break;
                }

                case '+':
                    tokens.push_back(MakeToken(TOKEN_PLUS,"+"));
                    advance();
                    break;
                case '-':
                    tokens.push_back(MakeToken(TOKEN_MINUS,"-"));
                    advance();
                    break;
                case '*':
                    tokens.push_back(MakeToken(TOKEN_MULTIPLY,"*"));
                    advance();
                    break;
                case '/':
                    tokens.push_back(MakeToken(TOKEN_DIVIDE,"/"));
                    advance();
                    break;
                case '%':
                    tokens.push_back(MakeToken(TOKEN_MODULO,"%"));
                    advance();
                    break;

                case '=':{
                    if(CheckForward(1)=='='){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_DOUBLEEQUALS,"=="));
                    }
                    else{
                        advance();
                        tokens.push_back(MakeToken(TOKEN_EQUAL,"="));
                    }
                    break;
                }

                case '<':{
                    if(CheckForward(1)=='='){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_LESSTHANEQUALS,"<="));
                    }
                    else{
                        tokens.push_back(MakeToken(TOKEN_LESSTHAN,"<"));
                        advance();
                    }
                    break;
                }

                case '>':{
                    if(CheckForward(1)=='='){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_GREATERTHANEQUALS,">="));
                    }
                    else{
                        tokens.push_back(MakeToken(TOKEN_GREATERTHAN,">"));
                        advance();
                    }
                    break;
                }

                case '!':{
                    if(CheckForward(1)=='='){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_NOTEQUALS,"!="));
                    }
                    else{
                        tokens.push_back(MakeToken(TOKEN_NOT,"!"));
                        advance();
                    }
                    break;
                }

                case '&':{
                    if(CheckForward(1)=='&'){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_AND,"&&"));
                    }
                    else{
                        cout<<"ERROR Invalid token '&'"<<endl;
                        exit(1);
                    }
                    break;
                }

                case '|':{
                    if(CheckForward(1)=='|'){
                        advance(); advance();
                        tokens.push_back(MakeToken(TOKEN_OR,"||"));
                    }
                    else{
                        cout<<"ERROR Invalid token '|'"<<endl;
                        exit(1);
                    }
                    break;
                }

                case '(':
                    tokens.push_back(MakeToken(TOKEN_LEFT_SBRACKET,"("));
                    advance();
                    break;
                case ')':
                    tokens.push_back(MakeToken(TOKEN_RIGHT_SBRACKET,")"));
                    advance();
                    break;
                case ':':
                    tokens.push_back(MakeToken(TOKEN_COLON,":"));
                    advance();
                    break;

                default:{
                    cout<<"LEXER ERROR : Invalid token "<<current<<" at line "<<LineNum<<endl;
                    exit(1);
                }
            }
        }

        while(Indentation.top()!=0){
            Indentation.pop();
            tokens.push_back(MakeToken(TOKEN_DEDENT,"DEDENT"));
        }

        tokens.push_back(MakeToken(TOKEN_EOF,"EOF"));
        return tokens;
    }

    private:
    string source;
    int cursor;
    int size;
    char current;
    int LineNum;
    int CharacterNum;
    stack<int> Indentation;
};
#endif