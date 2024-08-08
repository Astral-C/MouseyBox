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
    {"false", TokenType::FALSE},
    {"or", TokenType::OR},
    {"and", TokenType::AND}
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
    {TokenType::SCONST, "TokenType::SCONST"},
    {TokenType::OR, "TokenType::OR"},
    {TokenType::AND, "TokenType::AND"}
};

std::map<NodeType, std::string> DebugNodeNames {
    {NodeType::Base, "NodeType::Base"},
    {NodeType::Program, "NodeType::Program"},
    {NodeType::If, "NodeType::If"},
    {NodeType::AssigNode, "NodeType::AssigNode"},
    {NodeType::Block, "NodeType::Block"},
    {NodeType::Exp, "NodeType::Exp"},
    {NodeType::Group, "NodeType::Group"},
    {NodeType::Term, "NodeType::Term"},
    {NodeType::Factor, "NodeType::Factor"},
    {NodeType::Comparison, "NodeType::Comparison"},
    {NodeType::And, "NodeType::And"},
    {NodeType::Or, "NodeType::Or"},
    {NodeType::Variable, "NodeType::Variable"},
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

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Declaration(){
    std::shared_ptr<mb::TreeNode<AstNode>> node = nullptr;
    Token peek = PeekToken();
    switch (peek.token)
    {
    case TokenType::IF:
        // Parse If
        node = IfStatement();
        break;
    
    case TokenType::IDENT:
        //Consume the ident then check whats next?
        node = AssignStatement();
        break;

    case TokenType::FOR:
        // Parse Loop
        break;

    default:
        break;
    }

    return node;

}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Declarations(){
    std::shared_ptr<mb::TreeNode<AstNode>> node = std::make_shared<mb::TreeNode<AstNode>>((AstNode)(AstNode){.mType = NodeType::Block});

    while(!AtEnd()){
        Token peek = PeekToken(); 
        mb::Log::Error(std::format("Peeked Token {} on line {}: {}", DebugTokenNames[peek.token], peek.line, peek.lexeme));
        if(peek.token == TokenType::END || peek.token == TokenType::ELSE){
            ConsumeToken();
            break;
        } else {
            std::shared_ptr<mb::TreeNode<AstNode>> decl = Declaration(); 
            
            if (decl == nullptr){
                return nullptr;
            }

            node->AddNode(decl);
        }
    }

    return node;

}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::IfStatement(){
   std::shared_ptr<mb::TreeNode<AstNode>> node = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::If });

    ConsumeToken(); // Consume 'if' token

    std::shared_ptr<mb::TreeNode<AstNode>> exp = Expression();
    if(exp == nullptr){
        return nullptr;
    }

    node->AddNode(exp);

    std::shared_ptr<mb::TreeNode<AstNode>> decls = Declarations();
    
    if(decls == nullptr){
        return nullptr;
    }

    node->AddNode(decls);

    if(PrevToken().token == TokenType::ELSE){
        std::shared_ptr<mb::TreeNode<AstNode>> decls = Declarations();
        if(decls == nullptr){
            return nullptr;
        }
        node->AddNode(decls);
    }

    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::AssignStatement(){
    std::shared_ptr<mb::TreeNode<AstNode>> node = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::AssigNode});

    ConsumeToken(); // Consume Ident
    if(ConsumeToken().token != TokenType::EQ){
        mb::Log::Error("Unexpected Token {} on line {}: {}", DebugTokenNames[PrevToken().token], PrevToken().line, PrevToken().lexeme);
        return nullptr;
    }

    std::shared_ptr<mb::TreeNode<AstNode>> exp = Expression();
    if(exp != nullptr){
        node->AddNode(exp);
    }

    
    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Group(){
    ConsumeToken(); // consume LParen
    mb::Log::Error("Entering Group!");
    
    std::shared_ptr<mb::TreeNode<AstNode>> exp = Expression();
    
    if(exp == nullptr){
        return nullptr;
    }
    
    if(ConsumeToken().token != TokenType::RPAREN){
        mb::Log::Error("Group missing closing parenthesis!");
        return nullptr;
    }

    return exp;
}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Literal(){
    Token t = ConsumeToken();
    return std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Literal, .mToken = t});
}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Term(){
    std::shared_ptr<mb::TreeNode<AstNode>> trm = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Term});
    
    std::shared_ptr<mb::TreeNode<AstNode>> lfct = Factor();

    if(PeekToken().token == TokenType::TERM){
        trm->AddNode(lfct);

        while(PeekToken().token == TokenType::TERM){
            ConsumeToken();
            std::shared_ptr<mb::TreeNode<AstNode>> rfct = Factor();
            trm->AddNode(rfct);
        }
    } else {
        return lfct;
    }

    return trm;

}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Factor(){
    std::shared_ptr<mb::TreeNode<AstNode>> fctr = nullptr;
    
    std::shared_ptr<mb::TreeNode<AstNode>> llit = Literal();

    if(PeekToken().token == TokenType::FACTOR){

        while(PeekToken().token == TokenType::FACTOR){
            fctr = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Factor});
            ConsumeToken();

            std::shared_ptr<mb::TreeNode<AstNode>> rlit = Literal();

            fctr->AddNode(llit);
            fctr->AddNode(rlit);
            llit = fctr;
        }
    } else {
        return llit;
    }

    return fctr;
}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::Comparison(){
    std::set<TokenType> cmpTypes = { TokenType::IS_EQ, TokenType::IS_NEQ, TokenType::LT, TokenType::LT_EQ, TokenType::GT, TokenType::GT_EQ };
    std::shared_ptr<mb::TreeNode<AstNode>> cmp = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Comparison});

    std::shared_ptr<mb::TreeNode<AstNode>> ltrm = Term();

    if(cmpTypes.contains(PeekToken().token)){
        cmp->AddNode(ltrm);
        while(cmpTypes.contains(PeekToken().token)){
            ConsumeToken();
            std::shared_ptr<mb::TreeNode<AstNode>> rtrm = Term();
            cmp->AddNode(rtrm);
        }
    } else {
        return ltrm;
    }

    return cmp;
}

std::shared_ptr<mb::TreeNode<AstNode>> Parser::And(){  
    std::shared_ptr<mb::TreeNode<AstNode>> andn = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::And});

    std::shared_ptr<mb::TreeNode<AstNode>> land = Comparison();

    if(PeekToken().token == TokenType::AND){
        andn->AddNode(land);

        while(PeekToken().token == TokenType::AND){
            ConsumeToken();
            std::shared_ptr<mb::TreeNode<AstNode>> rand = Comparison();
            andn->AddNode(rand);
        }
    } else {
        return land;
    }

    return andn;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Expression(){
    std::set<TokenType> OpTokens = {TokenType::IS_EQ, TokenType::IS_NEQ, TokenType::LT, TokenType::LT_EQ, TokenType::GT, TokenType::GT_EQ, TokenType::TERM, TokenType::FACTOR};

    std::shared_ptr<mb::TreeNode<AstNode>> exp = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Exp});

    std::shared_ptr<mb::TreeNode<AstNode>> lor = And();

    if(PeekToken().token == TokenType::OR){
        std::shared_ptr<mb::TreeNode<AstNode>> orn = std::make_shared<mb::TreeNode<AstNode>>((AstNode){.mType = NodeType::Or});
        orn->AddNode(lor);

        while(PeekToken().token == TokenType::OR){
            ConsumeToken();
            std::shared_ptr<mb::TreeNode<AstNode>> ror = And();
            orn->AddNode(ror);
        }
        
        exp->AddNode(orn);
    } else {
        exp->AddNode(lor);
    }

    return exp;
}

mb::Tree<AstNode> Parser::Parse(){
    mb::Tree<AstNode> tree;

    tree.SetRoot({ .mType = NodeType::Program });

    Token temp;
    if((temp = ConsumeToken()).token != TokenType::BEGIN){
        mb::Log::Error("{}: Program Missing Begin Token, got \"{}\"", temp.line, temp.lexeme);
    }

    std::shared_ptr<TreeNode<AstNode>> decls = Declarations();
    tree.GetRoot()->AddNode(decls);

    if(decls == nullptr){
        mb::Log::Error(std::format("{}: Parse Error Token", temp.line));
    }

    if((temp = PrevToken()).token != TokenType::END){
        mb::Log::Error(std::format("{}: Program Missing End Token", temp.line));
    }

    return tree;
}


void PrintParseTree(std::shared_ptr<mb::TreeNode<AstNode>> root, std::string tabs){
    if(root->data()->mToken.lexeme != ""){
        mb::Log::Debug(std::format("{}[Node Type is {} : {}]", tabs, DebugNodeNames[root->data()->mType], root->data()->mToken.lexeme));
    } else {
        mb::Log::Debug(std::format("{}[Node Type is {}]", tabs, DebugNodeNames[root->data()->mType]));
    }
    for(auto node : *root->GetChildren()){
        PrintParseTree(node, tabs + "  ");
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