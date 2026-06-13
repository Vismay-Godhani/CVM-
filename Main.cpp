#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "VM.hpp"
using namespace std;

void RunSource(const string& src,map<string,Value>& env,bool debug) {
    Lexer Lexer(src);
    vector<Token*> tokens = Lexer.Tokenize();
    if(debug){
        cout<<"   TOKENS   \n";
        for(auto* t:tokens){
            cout<<TypeToString(t->TYPE)<<" : "<< t->VALUE<<"\n";
        }
        cout<<"\n\n\n";
    }
    if(tokens.empty() || tokens[0]->TYPE == TOKEN_EOF) return;
    Parser Parser(tokens);
    AST_NODE* ast = Parser.Parse();
    if(debug){
        cout << "   ABSTRACT SYNTAX TREE   \n";
        Parser.PrintAST(ast);
        cout << "\n\n\n";
    }
    Compiler Compiler;
    vector<Instructions> bytecode = Compiler.Compile(ast);
    if(debug){
        Compiler.PrintBytecode();
        cout<<"\n\n\n";
    }
    VM vm(bytecode, env);
    vm.Run();
}

void StartREPL(bool debug) {
    cout<<" CVM++ REPL\n";
    cout<<" Type 'exit' to quit.\n";    
    map<string, Value> global_env; 
    string line;
    while(true){
        cout<<">> ";
        if(!getline(cin,line)) 
            break;
        if(line=="exit"||line=="quit") 
            break;
        if(line.empty())
            continue;
        RunSource(line + "\n", global_env, debug);
    }
}

int main(int argc,char* argv[]){
    bool debug=false;
    string filename="";

    for(int i=1;i<argc;i++){
        if(string(argv[i])=="--debug"){
            debug=true;
        } else{
            filename=argv[i];
        }
    }

    if(filename.empty()){
        StartREPL(debug);
    } 
    else{
        ifstream SourceCode(filename);
        if(!SourceCode){
            cout<<"Source file not found: "<<filename<<endl;
            return 1;
        }
        stringstream ss;
        char c;
        while(SourceCode.get(c)){
            ss << c;
        }
        map<string, Value> global_env;
        RunSource(ss.str(), global_env,debug);
    }
    return 0;
}