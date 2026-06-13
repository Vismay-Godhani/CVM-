#ifndef __Compiler_h
#define __Compiler_h
#include"Parser.hpp"
#include<vector>
#include<string>
using namespace std;

enum Opcode{
    OP_PUSH_INT,
    OP_PUSH_BOOL,
    OP_PUSH_STRING,
    OP_LOAD_VAR,
    OP_STORE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULUS,
    OP_LESSTHAN,
    OP_LESSTHANEQUALS,
    OP_GREATERTHAN,
    OP_GREATERTHANEQUALS,
    OP_DOUBLEEQUALS,
    OP_NOTEQUALS,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_PRINT,
    OP_INPUT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_POP,
    OP_HALT
};

struct Instructions{
    Opcode op;
    int operand_int=0;
    string operand_string="";
};

class Compiler{
    public:
    vector<Instructions> Bytecode;

    void Emit(Opcode op,int iop=0,string sop=""){
        Bytecode.push_back({op,iop,sop});
    }

    int CurrentAddress()const{
        return(int)Bytecode.size();
    }

    void Patch(int address,int target){
        Bytecode[address].operand_int=target;
    }

    void CompilerNode(AST_NODE* NODE){
        if(!NODE){
            return;
        }

        switch(NODE->TYPE){
            case NODE_ROOT:{
                for(auto* child:NODE->Children){
                    CompilerNode(child);
                }
                break;
            }

            case NODE_INT:{
                Emit(OP_PUSH_INT,stoi(NODE->VALUE));
                break;
            }

            case NODE_BOOL:{
                if(NODE->VALUE=="true"){
                    Emit(OP_PUSH_BOOL,1);
                }else{
                    Emit(OP_PUSH_BOOL,0);
                }
                break;
            }

            case NODE_STRING:{
                Emit(OP_PUSH_STRING,0,NODE->VALUE);
                break;
            }

            case NODE_VARIABLE:{
                Emit(OP_LOAD_VAR,0,NODE->VALUE);
                break;
            }

            case NODE_ADD:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_ADD);
                break;
            }

            case NODE_SUBTRACT:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_SUBTRACT);
                break;
            }

            case NODE_MULTIPLY:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_MULTIPLY);
                break;
            }

            case NODE_DIVIDE:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_DIVIDE);
                break;
            }

            case NODE_MODULUS:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_MODULUS);
                break;
            }

            case NODE_LESSTHAN:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_LESSTHAN);
                break;
            }
            
            case NODE_LESSTHANEQUALS:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_LESSTHANEQUALS);
                break;
            }

            case NODE_GREATERTHAN:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_GREATERTHAN);
                break;
            }

            case NODE_GREATERTHANEQUALS:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_GREATERTHANEQUALS);
                break;
            }

            case NODE_DOUBLEEQUALS:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_DOUBLEEQUALS);
                break;
            }

            case NODE_NOTEQUALS:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_NOTEQUALS);
                break;
            }

            case NODE_AND:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_AND);
                break;
            }

            case NODE_OR:{
                CompilerNode(NODE->Left);
                CompilerNode(NODE->Right);
                Emit(OP_OR);
                break;
            }

            case NODE_NOT:{
                CompilerNode(NODE->Right);
                Emit(OP_NOT);
                break;
            }

            case NODE_ASSIGN:{
                CompilerNode(NODE->Right);
                Emit(OP_STORE,0,NODE->VALUE);
                break;
            }

            case NODE_IF:{
                CompilerNode(NODE->Left);
                int jump_false=CurrentAddress();
                Emit(OP_JUMP_IF_FALSE,0);
                for(auto* child:NODE->Children){
                    CompilerNode(child);
                }
                if(NODE->Right){
                    int jump_end=CurrentAddress();
                    Emit(OP_JUMP,0);
                    Patch(jump_false,CurrentAddress());
                    for(auto* child:NODE->Right->Children){
                        CompilerNode(child);
                    }
                    Patch(jump_end,CurrentAddress());
                }else{
                    Patch(jump_false,CurrentAddress());
                }
                break;
            }

            case NODE_WHILE:{
                int loop_start=CurrentAddress();
                CompilerNode(NODE->Left);
                int jump_false=CurrentAddress();
                Emit(OP_JUMP_IF_FALSE,0);
                for(auto* child:NODE->Children){
                    CompilerNode(child);
                }
                Emit(OP_JUMP,loop_start);
                Patch(jump_false,CurrentAddress());
                break;
            }

            case NODE_PRINT:{
                CompilerNode(NODE->Right);
                Emit(OP_PRINT);
                break;
            }

            case NODE_INPUT:{
                Emit(OP_INPUT);
                break;
            }

            default:{
                cout<<"ERROR: Unknown node type "<<NODE->TYPE<<endl;
                exit(1);
            }
        }
    }

    vector<Instructions> Compile(AST_NODE* root){
        CompilerNode(root);
        Emit(OP_HALT);
        return Bytecode;
    }

    void PrintBytecode()const{
        cout<<endl<<"Bytecode"<<endl;
        for(int i=0;i<(int)Bytecode.size();i++){
            const auto& ins=Bytecode[i];
            cout<<i<<"\t";
            switch(ins.op){
                case OP_PUSH_INT:{
                    cout<<"PUSH INT "<<ins.operand_int;
                    break;
                }
                
                case OP_PUSH_BOOL:{
                    if(ins.operand_int==1){
                        cout<<"PUSH BOOL true";
                    }else{
                        cout<<"PUSH BOOL false";
                    }
                    break;
                }

                case OP_LOAD_VAR:{
                    cout<<"LOAD VARIABLE "<<ins.operand_string;
                    break;
                }

                case OP_STORE:{
                    cout<<"STORE "<<ins.operand_string;
                    break;
                }

                case OP_PUSH_STRING:{
                    cout<<"PUSH STRING \""<<ins.operand_string<<"\"";
                    break;
                }

                case OP_INPUT:{
                    cout<<"INPUT";
                    break;
                }

                case OP_ADD:{
                    cout<<"ADD";
                    break;
                }

                case OP_SUBTRACT:{
                    cout<<"SUBTRACT";
                    break;
                }

                case OP_MULTIPLY:{
                    cout<<"MULTIPLY";
                    break;
                }

                case OP_DIVIDE:{
                    cout<<"DIVIDE";
                    break;
                }

                case OP_MODULUS:{
                    cout<<"MODULUS";
                    break;
                }

                case OP_LESSTHAN:{
                    cout<<"LESS THAN";
                    break;
                }

                case OP_LESSTHANEQUALS:{
                    cout<<"LESS THAN EQUALS";
                    break;
                }

                case OP_GREATERTHAN:{
                    cout<<"GREATER THAN";
                    break;
                }

                case OP_GREATERTHANEQUALS:{
                    cout<<"GREATER THAN EQUALS";
                    break;
                }

                case OP_DOUBLEEQUALS:{
                    cout<<"DOUBLE EQUALS";
                    break;
                }

                case OP_NOTEQUALS:{
                    cout<<"NOT EQUALS";
                    break;
                }

                case OP_AND:{
                    cout<<"AND";
                    break;
                }

                case OP_OR:{
                    cout<<"OR";
                    break;
                }

                case OP_NOT:{
                    cout<<"NOT";
                    break;
                }

                case OP_PRINT:{
                    cout<<"PRINT";
                    break;
                }

                case OP_JUMP:{
                    cout<<"JUMP "<<ins.operand_int;
                    break;
                }

                case OP_JUMP_IF_FALSE:{
                    cout<<"JUMP IF FALSE "<<ins.operand_int;
                    break;
                }

                case OP_POP:{
                    cout<<"POP";
                    break;
                }

                case OP_HALT:{
                    cout<<"HALT";
                    break;
                }
            }
            cout<<endl;
        }
    }
};

#endif