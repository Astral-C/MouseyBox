#include <sstream>
#include <system/Log.hpp>
#include <system/Script.hpp>
#include <iostream>
#include <set>

namespace mb::Scripting {

enum class LexerState {
    BEGIN,
    IDENT,
    INT,
    STRING,
    ERR
};

std::map<std::string, TokenType> ReservedTokens {
    {"begin", TokenType::BEGIN},
    {"end", TokenType::END},
    {"+", TokenType::TERM},
    {"-", TokenType::TERM},
    {"*", TokenType::FACTOR},
    {"/", TokenType::FACTOR},
    {"=", TokenType::EQ},
    {"==", TokenType::IS_EQ},
    {"!=", TokenType::IS_NEQ},
    {">", TokenType::GT},
    {"<", TokenType::LT},
    {">=", TokenType::GT_EQ},
    {"<=", TokenType::LT_EQ},
    {"(", TokenType::LPAREN},
    {")", TokenType::RPAREN},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"int", TokenType::INT},
    {"bool", TokenType::BOOL},
    {"string", TokenType::STRING},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE}
};

std::map<TokenType, std::string> DebugTokenNames {
    {TokenType::BEGIN, "TokenType::BEGIN"},
    {TokenType::END, "TokenType::END"},
    {TokenType::TERM, "TokenType::TERM"},
    {TokenType::FACTOR, "TokenType::FACTOR"},
    {TokenType::EQ, "TokenType::EQ"},
    {TokenType::IS_EQ, "TokenType::IS_EQ"},
    {TokenType::IS_NEQ, "TokenType::IS_NEQ"},
    {TokenType::IDENT, "TokenType::IDENT"},
    {TokenType::LPAREN, "TokenType::LPAREN"},
    {TokenType::RPAREN, "TokenType::RPAREN"},
    {TokenType::IF, "TokenType::IF"},
    {TokenType::ELSE, "TokenType::ELSE"},
    {TokenType::FOR, "TokenType::FOR"},
    {TokenType::INT, "TokenType::INT"},
    {TokenType::BOOL, "TokenType::BOOL"},
    {TokenType::STRING, "TokenType::STRING"},
    {TokenType::TRUE, "TokenType::TRUE"},
    {TokenType::FALSE, "TokenType::FALSE"},
    {TokenType::ICONST, "TokenType::ICONST"},
    {TokenType::SCONST, "TokenType::SCONST"}
};

std::map<NodeType, std::string> DebugNodeNames {
    {NodeType::Base, "NodeType::Base"},
    {NodeType::Program, "NodeType::Program"},
    {NodeType::If, "NodeType::If"},
    {NodeType::AssigNode, "NodeType::AssigNode"},
    {NodeType::Block, "NodeType::Block"},
    {NodeType::Exp, "NodeType::Exp"},
    {NodeType::Group, "NodeType::Group"},
    {NodeType::Comparison, "NodeType::Comparison"},
    {NodeType::Term, "NodeType::Term"},
    {NodeType::Factor, "NodeType::Factor"},
    {NodeType::Literal, "NodeType::Literal"},
    {NodeType::Err, "NodeType::Err"}
};


std::vector<Token> LexString(std::string stream){
    bool lexing = true;
    LexerState state = LexerState::BEGIN;

    int curline = 0;
    std::vector<Token> tokens;

    int sp = 0;

    Token currentToken;
    currentToken.lexeme = "";

    while(lexing){
        
        if(sp == stream.size()){
            lexing = false;
        }

        char cur = stream[sp++];

        switch(state) {
            case LexerState::BEGIN:
                if(isalpha(cur)){
                    state = LexerState::IDENT;
                    currentToken.lexeme += cur;
                }

                if(isdigit(cur)){
                    state = LexerState::INT;
                    currentToken.lexeme += cur;
                }

                if(cur == '"'){
                    state = LexerState::STRING;
                }

                if(stream[sp-1] == '=' && stream[sp] == '='){
                    sp++;
                    tokens.push_back({curline, ReservedTokens["=="], "=="});
                    currentToken.lexeme = "";
                } else if(stream[sp-1] == '!' && stream[sp] == '='){
                    sp++;
                    tokens.push_back({curline, ReservedTokens["!="], "!="});
                    currentToken.lexeme = "";
                } else if(stream[sp-1] == '>' && stream[sp] == '='){
                    sp++;
                    tokens.push_back({curline, ReservedTokens[">="], ">="});
                    currentToken.lexeme = "";
                } else if(stream[sp-1] == '<' && stream[sp] == '='){
                    sp++;
                    tokens.push_back({curline, ReservedTokens["<="], "<="});
                    currentToken.lexeme = "";
                } else if(ReservedTokens.contains(std::string(1, cur))){
                    tokens.push_back({curline, ReservedTokens[std::string(1, cur)], std::string(1, cur)});
                    currentToken.lexeme = "";
                }

                if(cur == '\n'){
                    curline++;
                }

                break;

            case LexerState::IDENT:
                if(isalnum(cur) || cur == '_'){
                    currentToken.lexeme += cur;
                } else { // whitespace
                    currentToken.line = curline;
                    if(cur == '\n') curline++;
                    if(ReservedTokens.contains(currentToken.lexeme)){
                        if(ReservedTokens.contains(currentToken.lexeme + stream[sp+1])){
                            mb::Log::Debug(currentToken.lexeme);
                            currentToken.token = ReservedTokens[currentToken.lexeme];
                        } else {
                            currentToken.token = ReservedTokens[currentToken.lexeme];
                        }
                    } else {
                        currentToken.token = TokenType::IDENT;
                    }

                    tokens.push_back(currentToken);
                    currentToken.lexeme = "";
                    sp--;
                    
                    state = LexerState::BEGIN;
                }
                break;

            case LexerState::INT:                                                                                                                                              
                if(isdigit(cur)){
                    currentToken.lexeme += cur;
                } else {
                    currentToken.token = TokenType::ICONST;
                    currentToken.line = curline;
                    tokens.push_back(currentToken);
                    currentToken.lexeme = "";
                    sp--;
                    state = LexerState::BEGIN;
                }
                break;

            case LexerState::STRING:
                if(cur == '"'){
                    currentToken.line = curline;
                    currentToken.token = TokenType::SCONST;
                    state = LexerState::BEGIN;

                    tokens.push_back(currentToken);
                    currentToken.lexeme = "";
                } else if(cur == '\n') {
                    state = LexerState::ERR;
                } else {
                    currentToken.lexeme += cur;
                }
                break;

            case LexerState::ERR:
                mb::Log::Debug(std::format("[Script] Lex error line {}: {}", curline, currentToken.lexeme));
                lexing = false;
                break;

            default:
                lexing = false;
                break;
        }

    }

    return tokens;
}

Parser::Parser(){}
Parser::~Parser(){}

bool Parser::Declaration(mb::TreeNode<AstNode>* root){
    Token peek = PeekToken();
    switch (peek.token)
    {
    case TokenType::IF:
        // Parse If
        if(!IfStatement(root)){
            return false;
        }

        break;
    
    case TokenType::IDENT:
        //Consume the ident then check whats next?
        if(!AssignStatement(root)){
            return false;
        }
        break;

    case TokenType::FOR:
        // Parse Loop
        break;

    default:
        return false;
        mb::Log::Error(std::format("Unexpected Token {} on line {}: {}", DebugTokenNames[peek.token], peek.line, peek.lexeme));
        break;
    }

    return true;

}

bool Parser::Declarations(mb::TreeNode<AstNode>* root){
    mb::TreeNode<AstNode>* node = root->AddChild({.mType = NodeType::Block});

    while(!AtEnd()){
        Token peek = PeekToken(); 
        mb::Log::Error(std::format("Peeked Token {} on line {}: {}", DebugTokenNames[peek.token], peek.line, peek.lexeme));
        if(peek.token == TokenType::END || peek.token == TokenType::ELSE){
            ConsumeToken();
            return true;
        } else if (!Declaration(node)){
            return false;
        }
    }
    return true;
}

bool Parser::IfStatement(mb::TreeNode<AstNode>* root){
    mb::TreeNode<AstNode>* node = root->AddChild({.mType = NodeType::If });

    ConsumeToken(); // Consume 'if' token

    if(!Expression(node)){
        return false;
    }

    if(!Declarations(node)){
        return false;
    }

    if(PrevToken().token == TokenType::ELSE){
        if(!Declarations(node)){
            return false;
        }
    }

    return true;
}

bool Parser::AssignStatement(mb::TreeNode<AstNode>* root){
    mb::TreeNode<AstNode>* node = root->AddChild({.mType = NodeType::AssigNode});

    ConsumeToken(); // Consume Ident
    if(ConsumeToken().token != TokenType::EQ){
        mb::Log::Error("Unexpected Token {} on line {}: {}", DebugTokenNames[PrevToken().token], PrevToken().line, PrevToken().lexeme);
        return false;
    }

    mb::Log::Error("Last Token in Assign {}: {}", DebugTokenNames[PrevToken().token], PrevToken().line, PrevToken().lexeme);

    if(!Expression(node)){
        return false;
    }

    return true;
}

bool Parser::Factor(mb::TreeNode<AstNode>* root){
    AstNode nodeData = {.mType = NodeType::Factor};

    ConsumeToken();

    mb::TreeNode<AstNode>* node = root->AddChild(nodeData);

    return true;
}

bool Parser::Term(Token right, Token op, mb::TreeNode<AstNode>* root){

    mb::Log::Error("Hanlding Term...");
    AstNode nodeData = {.mType = NodeType::Term};

    Token lhs = ConsumeToken();

    mb::TreeNode<AstNode>* node = root->AddChild(nodeData);
    return true;
}

bool Parser::Comparison(Token right, Token op, mb::TreeNode<AstNode>* root){
    AstNode nodeData = {.mType = NodeType::Comparison};

    ConsumeToken();
    
    mb::TreeNode<AstNode>* node = root->AddChild(nodeData);
    return true;
}

bool Parser::Expression(mb::TreeNode<AstNode>* root){
    std::set<TokenType> OpTokens = {TokenType::IS_EQ, TokenType::IS_NEQ, TokenType::LT, TokenType::LT_EQ, TokenType::GT, TokenType::GT_EQ, TokenType::TERM, TokenType::FACTOR};

    mb::TreeNode<AstNode>* node = root->AddChild({.mType = NodeType::Exp});

    

    return true;
}

bool Parser::Statement(mb::TreeNode<AstNode>* root){
    return true;
}

bool Parser::Literal(mb::TreeNode<AstNode>* root){
    return true;
}


mb::Tree<AstNode> Parser::Parse(){
    mb::Tree<AstNode> tree;

    tree.SetRoot({ .mType = NodeType::Program });

    Token temp;
    if((temp = ConsumeToken()).token != TokenType::BEGIN){
        mb::Log::Error("{}: Program Missing Begin Token, got \"{}\"", temp.line, temp.lexeme);
    }

    if(!Declarations(tree.GetRoot())){
        mb::Log::Error(std::format("{}: Parse Error Token", temp.line));
    }

    if((temp = PrevToken()).token != TokenType::END){
        mb::Log::Error(std::format("{}: Program Missing End Token", temp.line));
    }

    return tree;

}


void PrintParseTree(mb::TreeNode<AstNode>* root, std::string tabs){
    mb::Log::Debug(std::format("{}[Node Type is {}]", tabs, DebugNodeNames[root->data()->mType]));
    for(auto node : *root->GetChildren()){
        PrintParseTree(&node, tabs + "  ");
    }
}

Script::Script(){

}

Script::Script(std::string script){

    Parser p;
    p.SetTokens(LexString(script));

    mTree = p.Parse();

    PrintParseTree(mTree.GetRoot(), "");

}

Script::~Script(){

}

}