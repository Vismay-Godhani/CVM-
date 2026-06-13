#ifndef __VM_h
#define __VM_h
#include"Compiler.hpp"
#include<stack>
#include<map>
#include<algorithm>
using namespace std;

struct Value{
    bool is_bool;
    bool is_string;
    int raw;
    string sval;

    static Value FromINT(int v){
        return{false,false,v,""};
    }

    static Value FromBOOL(bool v){
        if(v){
            return{true,false,1,""};
        }else{
            return{true,false,0,""};
        }
    }

    static Value FromSTRING(const string&v){
        return{false,true,0,v};
    }

    int asINT()const{
        return raw;
    }

    bool asBOOL()const{
        return raw!=0;
    }

    string str()const{
        if(is_string){
            return sval;
        }
        if(is_bool){
            return raw?"true":"false";
        }
        return to_string(raw);
    }
};

class VM{
    private:
    const vector<Instructions>&code;
    int ip;
    stack<Value>Stack;
    
    map<string,Value>&Variables;

    void push(Value v){
        Stack.push(v);
    }

    Value pop(){
        if(Stack.empty()){
            cout<<"ERROR: stack underflow"<<endl;
            exit(1);
        }
        Value v=Stack.top();
        Stack.pop();
        return v;
    }

    public:
    VM(const vector<Instructions>&code,map<string,Value>&env):code(code),ip(0),Variables(env){}

    void Run(){
        while(ip<(int)code.size()){
            const Instructions&ins=code[ip];

            switch(ins.op){
                case OP_PUSH_INT:{
                    push(Value::FromINT(ins.operand_int));
                    break;
                }

                case OP_PUSH_BOOL:{
                    push(Value::FromBOOL(ins.operand_int!=0));
                    break;
                }

                case OP_PUSH_STRING:{
                    push(Value::FromSTRING(ins.operand_string));
                    break;
                }

                case OP_LOAD_VAR:{
                    auto it=Variables.find(ins.operand_string);
                    if(it==Variables.end()){
                        cout<<"ERROR: undefined variable '"<<ins.operand_string<<"'"<<endl;
                        exit(1);
                    }
                    push(it->second);
                    break;
                }

                case OP_STORE:{
                    Variables[ins.operand_string]=pop();
                    break;
                }

                case OP_ADD:{
                    Value b=pop(),a=pop();
                    push(Value::FromINT(a.asINT()+b.asINT()));
                    break;
                }

                case OP_SUBTRACT:{
                    Value b=pop(),a=pop();
                    push(Value::FromINT(a.asINT()-b.asINT()));
                    break;
                }

                case OP_MULTIPLY:{
                    Value b=pop(),a=pop();
                    push(Value::FromINT(a.asINT()*b.asINT()));
                    break;
                }

                case OP_DIVIDE:{
                    Value b=pop(),a=pop();
                    if(b.asINT()==0){
                        cout<<"ERROR: division by zero"<<endl;
                        exit(1);
                    }
                    push(Value::FromINT(a.asINT()/b.asINT()));
                    break;
                }

                case OP_MODULUS:{
                    Value b=pop(),a=pop();
                    if(b.asINT()==0){
                        cout<<"ERROR: modulo by zero"<<endl;
                        exit(1);
                    }
                    push(Value::FromINT(a.asINT()%b.asINT()));
                    break;
                }

                case OP_LESSTHAN:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asINT()<b.asINT()));
                    break;
                }

                case OP_LESSTHANEQUALS:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asINT()<=b.asINT()));
                    break;
                }

                case OP_GREATERTHAN:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asINT()>b.asINT()));
                    break;
                }

                case OP_GREATERTHANEQUALS:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asINT()>=b.asINT()));
                    break;
                }
                
                case OP_DOUBLEEQUALS:{
                    Value b=pop(),a=pop();
                    if(a.is_string||b.is_string){
                        push(Value::FromBOOL(a.str()==b.str()));
                    }else{
                        push(Value::FromBOOL(a.asINT()==b.asINT()));
                    }
                    break;
                }

                case OP_NOTEQUALS:{
                    Value b=pop(),a=pop();
                    if(a.is_string||b.is_string){
                        push(Value::FromBOOL(a.str()!=b.str()));
                    }else{
                        push(Value::FromBOOL(a.asINT()!=b.asINT()));
                    }
                    break;
                }
                
                case OP_AND:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asBOOL()&&b.asBOOL()));
                    break;
                }

                case OP_OR:{
                    Value b=pop(),a=pop();
                    push(Value::FromBOOL(a.asBOOL()||b.asBOOL()));
                    break;
                }

                case OP_NOT:{
                    Value a=pop();
                    push(Value::FromBOOL(!a.asBOOL()));
                    break;
                }

                case OP_PRINT:{
                    cout<<pop().str()<<endl;
                    break;
                }
                
                case OP_INPUT:{
                    string s;
                    if(!getline(cin,s)){
                        cout<<"ERROR: input failed"<<endl;
                        exit(1);
                    }
                    
                    if(!s.empty()&&s.back()=='\r'){
                        s.pop_back();
                    }
                    
                    bool is_number=true;
                    int start=0;
                    if(s.length()>0&&s[0]=='-'){
                        start=1;
                    }
                    if(s.length()==(size_t)start){
                        is_number=false;
                    }
                    for(size_t i=start;i<s.length();i++){
                        if(!isdigit(s[i])){
                            is_number=false;
                            break;
                        }
                    }
                    
                    if(is_number){
                        push(Value::FromINT(stoi(s)));
                    }else if(s=="true"){
                        push(Value::FromBOOL(true));
                    }else if(s=="false"){
                        push(Value::FromBOOL(false));
                    }else{
                        push(Value::FromSTRING(s));
                    }
                    break;
                }
                
                case OP_JUMP:{
                    ip=ins.operand_int;
                    continue;
                }

                case OP_JUMP_IF_FALSE:{
                    if(!pop().asBOOL()){
                        ip=ins.operand_int;
                        continue;
                    }
                    break;
                }

                case OP_POP:{
                    pop();
                    break;
                }

                case OP_HALT:{
                    return;
                }

                default:{
                    cout<<"ERROR: unknown opcode at ip="<<ip<<endl;
                    exit(1);
                }
            }
            ip++;
        }
    }
};

#endif