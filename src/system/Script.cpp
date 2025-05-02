#include <sstream>
#include <system/Log.hpp>
#include <system/Script.hpp>
#include <iostream>
#include <set>

namespace mb::Scripting {

namespace Globals {
    std::map<std::string, SkitterValue> mVars;
    std::map<std::string, std::function<SkitterValue(std::vector<SkitterValue>)>> mCallables;
}

SkitterValue& Variable(std::string name) {
    return Globals::mVars[name];
}

std::function<SkitterValue(std::vector<SkitterValue>)>& Function(std::string name) {
    return Globals::mCallables[name];
}

bool HasGlobalVar(std::string name){
    return Globals::mVars.contains(name);
}

bool HasGlobalFunc(std::string name){
    return Globals::mCallables.contains(name);
}

std::map<std::string, SkitterValue>& GetGlobalVars(){
    return Globals::mVars;
}

std::map<std::string, std::function<SkitterValue(std::vector<SkitterValue>)>>& GetGlobalFuncs(){
    return Globals::mCallables;
}

template<>
std::string& SkitterValue::value<std::string>(){
    return mStrValue;
}

template<>
double& SkitterValue::value<double>(){
    return mNumValue;
}

template<>
bool& SkitterValue::value<bool>(){
    return mBoolValue;
}

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
    {",", TokenType::COMMA},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"and", TokenType::AND},
    {"call", TokenType::CALL},
    {"global", TokenType::GLOBAL}

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
    {TokenType::TRUE, "TokenType::TRUE"},
    {TokenType::FALSE, "TokenType::FALSE"},
    {TokenType::ICONST, "TokenType::ICONST"},
    {TokenType::SCONST, "TokenType::SCONST"},
    {TokenType::OR, "TokenType::OR"},
    {TokenType::AND, "TokenType::AND"},
    {TokenType::CALL, "TokenType::CALL"},
    {TokenType::GLOBAL, "TokenType::GLOBAL"},
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
    {NodeType::Call, "NodeType::Call"},
    {NodeType::Global, "NodeType::Global"},
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

                if(cur == '#'){
                    while(stream[sp] != '\n') sp++;
                    curline++;
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
                if(isdigit(cur) || cur == '.'){
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

std::shared_ptr<TreeNode<AstNode>> Parser::Declaration(){
    std::shared_ptr<TreeNode<AstNode>> node = nullptr;
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

    case TokenType::GLOBAL:
        node = GlobalAssignStatement();
        break;

    case TokenType::CALL:
        node = CallStatement();
        break;

    case TokenType::PRINT:
        node = PrintStatement();
        break;

    case TokenType::FOR:
        // Parse Loop
        break;

    default:
        break;
    }

    return node;

}

std::shared_ptr<TreeNode<AstNode>> Parser::Declarations(){
    std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode)(AstNode){.mType = NodeType::Block});

    while(!AtEnd()){
        Token peek = PeekToken();
        if(peek.token == TokenType::END || peek.token == TokenType::ELSE){
            ConsumeToken();
            break;
        } else {
            std::shared_ptr<TreeNode<AstNode>> decl = Declaration();

            if (decl == nullptr){
                return nullptr;
            }

            node->AddNode(decl);
        }
    }

    return node;

}

std::shared_ptr<TreeNode<AstNode>> Parser::IfStatement(){
   std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::If });

    ConsumeToken(); // Consume 'if' token

    std::shared_ptr<TreeNode<AstNode>> exp = Expression();
    if(exp == nullptr){
        return nullptr;
    }

    node->AddNode(exp);

    std::shared_ptr<TreeNode<AstNode>> decls = Declarations();

    if(decls == nullptr){
        return nullptr;
    }

    node->AddNode(decls);

    if(PrevToken().token == TokenType::ELSE){
        std::shared_ptr<TreeNode<AstNode>> decls = Declarations();
        if(decls == nullptr){
            return nullptr;
        }
        node->AddNode(decls);
    }

    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::AssignStatement(){
    Token t = ConsumeToken(); // Consume Ident

    std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::AssigNode, .mToken = t});

    if(ConsumeToken().token != TokenType::EQ){
        mb::Log::Error("Unexpected Token {} on line {}: {}", DebugTokenNames[PrevToken().token], PrevToken().line, PrevToken().lexeme);
        return nullptr;
    }

    if(PeekToken().token == TokenType::CALL){
        std::shared_ptr<TreeNode<AstNode>> exp = CallStatement();
        if(exp != nullptr){
            node->AddNode(exp);
        }
    } else {
        std::shared_ptr<TreeNode<AstNode>> exp = Expression();
        if(exp != nullptr){
            node->AddNode(exp);
        }
    }


    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::GlobalAssignStatement(){
    Token t = ConsumeToken(); // Consume Global

    std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Global, .mToken = t});

    t = ConsumeToken(); // Consume Ident
    std::shared_ptr<TreeNode<AstNode>> anode = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::AssigNode, .mToken = t});

    if(ConsumeToken().token != TokenType::EQ){
        mb::Log::Error("Unexpected Token {} on line {}: {}", DebugTokenNames[PrevToken().token], PrevToken().line, PrevToken().lexeme);
        return nullptr;
    }

    if(PeekToken().token == TokenType::CALL){
        std::shared_ptr<TreeNode<AstNode>> exp = CallStatement();
        if(exp != nullptr){
            anode->AddNode(exp);
        }
    } else {
        std::shared_ptr<TreeNode<AstNode>> exp = Expression();
        if(exp != nullptr){
            anode->AddNode(exp);
        }
    }

    node->AddNode(anode);

    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::CallStatement(){
    Token t = ConsumeToken(); // Consume Call

    std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Call, .mToken = t});

    node->AddNode(Literal());
    node->AddNode(Expression());

    while(PeekToken().token == TokenType::COMMA){
        ConsumeToken(); // Consume Comma
        node->AddNode(Expression());
    }


    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::PrintStatement(){
    Token t = ConsumeToken(); // Consume Print

    std::shared_ptr<TreeNode<AstNode>> node = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Print, .mToken = t});

    node->AddNode(Expression());

    while(PeekToken().token == TokenType::COMMA){
        ConsumeToken(); // Consume Comma
        node->AddNode(Expression());
    }


    return node;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Group(){
    ConsumeToken(); // consume LParen

    std::shared_ptr<TreeNode<AstNode>> exp = Expression();

    if(exp == nullptr){
        return nullptr;
    }

    if(ConsumeToken().token != TokenType::RPAREN){
        mb::Log::Error("Group missing closing parenthesis!");
        return nullptr;
    }

    return exp;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Literal(){

    Token t = PeekToken();
    if(t.token == TokenType::LPAREN){
        return Group();
    } else if(t.token == TokenType::IDENT) {
        ConsumeToken();
        return std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Variable, .mToken = t});
    } else {
        ConsumeToken();
        return std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Literal, .mToken = t});
    }
}

std::shared_ptr<TreeNode<AstNode>> Parser::Term(){
    std::shared_ptr<TreeNode<AstNode>> trm = nullptr;
    std::shared_ptr<TreeNode<AstNode>> lfct = Factor();

    if(PeekToken().token == TokenType::TERM){

        while(PeekToken().token == TokenType::TERM){
            trm = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Term, .mToken = ConsumeToken()});
            std::shared_ptr<TreeNode<AstNode>> rfct = Factor();

            trm->AddNode(lfct);
            trm->AddNode(rfct);
            lfct = trm;
        }
    } else {
        return lfct;
    }

    return trm;

}

std::shared_ptr<TreeNode<AstNode>> Parser::Factor(){
    std::shared_ptr<TreeNode<AstNode>> fctr = nullptr;

    std::shared_ptr<TreeNode<AstNode>> llit = Literal();

    if(PeekToken().token == TokenType::FACTOR){

        while(PeekToken().token == TokenType::FACTOR){
            fctr = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Factor, .mToken = ConsumeToken()});

            std::shared_ptr<TreeNode<AstNode>> rlit = Literal();

            fctr->AddNode(llit);
            fctr->AddNode(rlit);
            llit = fctr;
        }
    } else {
        return llit;
    }

    return fctr;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Comparison(){
    std::set<TokenType> cmpTypes = { TokenType::IS_EQ, TokenType::IS_NEQ, TokenType::LT, TokenType::LT_EQ, TokenType::GT, TokenType::GT_EQ };

    std::shared_ptr<TreeNode<AstNode>> cmp = nullptr;

    std::shared_ptr<TreeNode<AstNode>> ltrm = Term();

    if(cmpTypes.contains(PeekToken().token)){

        while(cmpTypes.contains(PeekToken().token)){
            cmp = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Comparison, .mToken = ConsumeToken()});

            std::shared_ptr<TreeNode<AstNode>> rtrm = Term();
            cmp->AddNode(ltrm);
            cmp->AddNode(rtrm);
            ltrm = cmp;
        }
    } else {
        return ltrm;
    }

    return cmp;
}

std::shared_ptr<TreeNode<AstNode>> Parser::And(){
    std::shared_ptr<TreeNode<AstNode>> andn = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::And});

    std::shared_ptr<TreeNode<AstNode>> land = Comparison();

    if(PeekToken().token == TokenType::AND){
        andn->AddNode(land);

        while(PeekToken().token == TokenType::AND){
            ConsumeToken();
            std::shared_ptr<TreeNode<AstNode>> rand = Comparison();
            andn->AddNode(rand);
        }
    } else {
        return land;
    }

    return andn;
}

std::shared_ptr<TreeNode<AstNode>> Parser::Expression(){
    std::set<TokenType> OpTokens = {TokenType::IS_EQ, TokenType::IS_NEQ, TokenType::LT, TokenType::LT_EQ, TokenType::GT, TokenType::GT_EQ, TokenType::TERM, TokenType::FACTOR};

    std::shared_ptr<TreeNode<AstNode>> exp = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Exp});

    std::shared_ptr<TreeNode<AstNode>> lor = And();

    if(PeekToken().token == TokenType::OR){
        std::shared_ptr<TreeNode<AstNode>> orn = std::make_shared<TreeNode<AstNode>>((AstNode){.mType = NodeType::Or});
        orn->AddNode(lor);

        while(PeekToken().token == TokenType::OR){
            ConsumeToken();
            std::shared_ptr<TreeNode<AstNode>> ror = And();
            orn->AddNode(ror);
        }

        exp->AddNode(orn);
    } else {
        exp->AddNode(lor);
    }

    if(exp->GetChildren()->size() == 1){
        return lor;
    } else {
        return exp;
    }
}

Tree<AstNode> Parser::Parse(){
    Tree<AstNode> tree;

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

    if((temp = ConsumeToken()).token != TokenType::END){
        mb::Log::Error(std::format("{}: Program Missing End Token", temp.line));
    }

    return tree;
}


void PrintParseTree(std::shared_ptr<TreeNode<AstNode>> root, std::string tabs){
    if(root->data()->mToken.lexeme != ""){
        mb::Log::Debug(std::format("{}[Node Type is {} : {}]", tabs, DebugNodeNames[root->data()->mType], root->data()->mToken.lexeme));
    } else {
        mb::Log::Debug(std::format("{}[Node Type is {}]", tabs, DebugNodeNames[root->data()->mType]));
    }
    for(auto node : *root->GetChildren()){
        PrintParseTree(node, tabs + "  ");
    }
}

SkitterValue Script::ExecNode(std::shared_ptr<TreeNode<AstNode>> root){
    switch (root->data()->mType)
    {

    case NodeType::Variable: {
        if(Globals::mVars.contains(root->data()->mToken.lexeme)){
            return Globals::mVars[root->data()->mToken.lexeme];
        } else {
            return mVars[root->data()->mToken.lexeme];
        }
    }

    case NodeType::Literal: {
        SkitterValue val;
        switch(root->data()->mToken.token){
            case TokenType::SCONST:
                val = std::string(root->data()->mToken.lexeme);
                return val;

            case TokenType::ICONST:
                val = std::stod(root->data()->mToken.lexeme);
                return val;

            case TokenType::TRUE:
            case TokenType::FALSE:
                val = root->data()->mToken.lexeme == "true";
                return val;
        }
        break;
    }

    case NodeType::Term: {
        SkitterValue val;

        SkitterValue lhs = ExecNode(root->GetChild(0));
        SkitterValue rhs = ExecNode(root->GetChild(1));

        if(root->data()->mToken.lexeme == "+"){
            if(lhs.mType == SkitterType::String){
                if(rhs.mType == SkitterType::Number){
                    val = lhs.value<std::string>() + std::to_string(rhs.value<double>());
                } else if(rhs.mType == SkitterType::Number){
                    val = lhs.value<std::string>() + (rhs.value<bool>() ? std::string("true") : std::string("false"));
                } else {
                    val = lhs.value<std::string>() + rhs.value<std::string>();
                }
                val.mType = SkitterType::String;
            } else {
                val = lhs.value<double>() + rhs.value<double>();
            }
        } else {
            val = ExecNode(root->GetChild(0)).value<double>() - ExecNode(root->GetChild(1)).value<double>();
        }
        return val;
    }

    case NodeType::Factor: {
        SkitterValue val;
        if(root->data()->mToken.lexeme == "*"){
            val = ExecNode(root->GetChild(0)).value<double>() * ExecNode(root->GetChild(1)).value<double>();
        } else {
            val = ExecNode(root->GetChild(0)).value<double>() / ExecNode(root->GetChild(1)).value<double>();
        }
        return val;
    }

    case NodeType::AssigNode: {
        SkitterValue val = ExecNode(root->GetChild(0));
        if(root->GetParent()->data()->mType != NodeType::Global){
            mVars[root->data()->mToken.lexeme] = val;
        } else {
            Globals::mVars[root->data()->mToken.lexeme] = val;
        }
        return val;
    }

    case NodeType::Global: {
        return ExecNode(root->GetChild(0));
    }

    case NodeType::Comparison: {
        SkitterValue val;

        SkitterValue lhs = ExecNode(root->GetChild(0));
        SkitterValue rhs = ExecNode(root->GetChild(1));

        switch (root->data()->mToken.token){
        case TokenType::GT:
            val = lhs.value<double>() > rhs.value<double>();
        break;

        case TokenType::GT_EQ:
            val = lhs.value<double>() >= rhs.value<double>();
        break;

        case TokenType::LT:
            val = lhs.value<double>() < rhs.value<double>();
        break;

        case TokenType::LT_EQ:
            val = lhs.value<double>() <= rhs.value<double>();
        break;

        case TokenType::IS_EQ:
            if(lhs.mType == rhs.mType && lhs.mType == SkitterType::Bool){
                val = lhs.value<bool>() == rhs.value<bool>();
            } else if(lhs.mType == rhs.mType && lhs.mType == SkitterType::Number){
                val = lhs.value<double>() == rhs.value<double>();
            } else if(lhs.mType == rhs.mType && lhs.mType == SkitterType::String){
                val = lhs.value<std::string>() == rhs.value<std::string>();
            } else {
                val = false;
            }
        break;

        case TokenType::IS_NEQ:
            if(lhs.mType == rhs.mType && lhs.mType == SkitterType::Bool){
                val = lhs.value<bool>() != rhs.value<bool>();
            } else if(lhs.mType == rhs.mType && lhs.mType == SkitterType::Number){
                val = lhs.value<double>() != rhs.value<double>();
            } else if(lhs.mType == rhs.mType && lhs.mType == SkitterType::String){
                val = lhs.value<std::string>() != rhs.value<std::string>();
            } else {
                val = true;
            }
        break;

        default:
            break;
        }


        return val;
    }

    case NodeType::And: {
        SkitterValue ret = ExecNode(root->GetChild(0));

        if(ret.mType == SkitterType::Number){
            ret.value<bool>() = ret.value<double>() > 0.0;
        } else if(ret.mType == SkitterType::String){
            ret.value<bool>() = ret.value<std::string>() == "true"; // lol
        }

        ret.mType = SkitterType::Bool;

        int childCount = root->GetChildren()->size();

        for(int i = 1; i < childCount; i++){
            SkitterValue v = ExecNode(root->GetChild(i));
            if(v.mType == SkitterType::Number){
                ret.value<bool>() = ret.value<bool>() && (ret.value<double>() > 0.0);
            } else if(v.mType == SkitterType::String){
                ret.value<bool>() = ret.value<bool>() && (ret.value<std::string>() == "true"); // lol
            } else {
                ret.value<bool>() = ret.value<bool>() && ret.value<bool>();
            }
        }

        return ret;
    }

    case NodeType::Exp: {
        return ExecNode(root->GetChild(0));
    }

    case NodeType::If: {
        SkitterValue v = ExecNode(root->GetChild(0));
        if(v.value<bool>()){
            ExecNode(root->GetChild(1));
        } else{
            if(root->GetChildren()->size() > 2){
                ExecNode(root->GetChild(2));
            }
        }
        return v;
    }

    case NodeType::Block: {
        for(auto node : *root->GetChildren()){
            ExecNode(node);
        }
        break;
    }

    case NodeType::Call: {
        if(!mCallables.contains(root->GetChild(0)->data()->mToken.lexeme) && !Globals::mCallables.contains(root->GetChild(0)->data()->mToken.lexeme)){
            return SkitterValue();
        }

        std::vector<SkitterValue> args = {};

        int childCount = root->GetChildren()->size();

        for(int i = 1; i < childCount; i++){
            SkitterValue v = ExecNode(root->GetChild(i));
            args.push_back(v);
        }

        SkitterValue v;

        if(Globals::mCallables.contains(root->GetChild(0)->data()->mToken.lexeme)){
            v = Globals::mCallables[root->GetChild(0)->data()->mToken.lexeme](args);
        } else {
            v = mCallables[root->GetChild(0)->data()->mToken.lexeme](args);
        }

        return v;
    }

    case NodeType::Print: {
        std::string str = "";

        for(auto node : *root->GetChildren()){
            SkitterValue v = ExecNode(node);

            if(v.mType == SkitterType::Bool){
                str += v.value<bool>() ? "true" : "false";
            } else if(v.mType == SkitterType::String){
                str += v.value<std::string>();
            } else {
                str += std::to_string(v.value<double>());
            }

        }

        mb::Log::Info(str);

        break;
    }

    default:
        break;
    }

    return (SkitterValue){ .mType = SkitterType::None };
}

void Script::Execute(){
    SkitterValue value = ExecNode(mTree.GetRoot()->GetChild(0));
}

void Script::DumpVars(){
    mb::Log::Debug("===Global===");
    for(auto [key, val] : Globals::mVars){
        switch (val.mType)
        {
        case SkitterType::Number:{
            mb::Log::Info("{} = {}", key, val.value<double>());
            break;
        }
        case SkitterType::Bool:{
            mb::Log::Info("{} = {}", key, (val.value<bool>() ? "true" : "false"));
            break;
        }
        case SkitterType::String:{
            mb::Log::Info("{} = {}", key, val.value<std::string>());
            break;
        }
        default:
            break;
        }
    }
    mb::Log::Debug("===Local===");
    for(auto [key, val] : mVars){
        switch (val.mType)
        {
        case SkitterType::Number:{
            mb::Log::Info("{} = {}", key, val.value<double>());
            break;
        }
        case SkitterType::Bool:{
            mb::Log::Info("{} = {}", key, (val.value<bool>() ? "true" : "false"));
            break;
        }
        case SkitterType::String:{
            mb::Log::Info("{} = {}", key, val.value<std::string>());
            break;
        }
        default:
            break;
        }
    }
    mb::Log::Debug("=========");
}

void Script::DumpTree(){
    PrintParseTree(mTree.GetRoot(), "");
}

Script::Script(){

}

Script::Script(std::string script){

    Parser p;
    p.SetTokens(LexString(script));

    mTree = p.Parse();
}

Script::~Script(){

}

}
