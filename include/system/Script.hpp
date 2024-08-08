#ifndef __MB_SCRIPT_H__
#define __MB_SCRIPT_H__
#include <cstdint>
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <system/Containers/Tree.hpp>

namespace mb::Scripting {
    enum class TokenType {
        BEGIN,
        END,
        TERM,
        FACTOR,
        IDENT,
        ICONST,
        SCONST,
        LPAREN,
        RPAREN,
        IF,
        ELSE,
        FOR,
        INT,
        STRING,
        BOOL,
        TRUE,
        FALSE,
        IS_EQ,
        IS_NEQ,
        GT,
        GT_EQ,
        LT,
        LT_EQ,
        EQ,
        OR,
        AND,
        NONE
    };

    enum class NodeType {
        Base,
        Program,
        If,
        AssigNode,
        Block,
        Exp,
        Group,
        Variable,
        Term,
        Factor,
        Comparison,
        Literal,
        Or,
        And,
        Err,
    };

    extern std::map<std::string, TokenType> ReservedTokens;

    extern std::map<TokenType, std::string> DebugTokenNames;

    typedef std::tuple<std::string, int, float, bool> SkitterValue;

    struct Token {
        int line { -1 };
        TokenType token { TokenType::NONE };
        std::string lexeme { "(none)" };
    };

    struct AstNode {
        NodeType mType { NodeType::Base };
        Token mToken { 0, TokenType::NONE, ""};
        SkitterValue mValue;
    };

    class Parser {
        int mCurTokenIdx { 0 };
        std::vector<Token> mTokens;
    
    public:
        void SetTokens(std::vector<Token> t) { mTokens = t; }

        mb::Tree<AstNode> Parse();

        std::shared_ptr<mb::TreeNode<AstNode>> Declarations();
        std::shared_ptr<mb::TreeNode<AstNode>> Declaration();
        std::shared_ptr<mb::TreeNode<AstNode>> Expression();


        std::shared_ptr<mb::TreeNode<AstNode>> Literal();
        std::shared_ptr<mb::TreeNode<AstNode>> Term();
        std::shared_ptr<mb::TreeNode<AstNode>> Factor();
        std::shared_ptr<mb::TreeNode<AstNode>> Comparison();
        std::shared_ptr<mb::TreeNode<AstNode>> And();
        std::shared_ptr<mb::TreeNode<AstNode>> Or();

        std::shared_ptr<mb::TreeNode<AstNode>> Group();
        std::shared_ptr<mb::TreeNode<AstNode>> Statement();
        std::shared_ptr<mb::TreeNode<AstNode>> IfStatement();
        std::shared_ptr<mb::TreeNode<AstNode>> AssignStatement();

        Token PeekToken(int offset=0){
            return mTokens[mCurTokenIdx+offset];
        }

        Token ConsumeToken(){
            return mTokens[mCurTokenIdx++];
        }

        void StepBack(){
            mCurTokenIdx--;
        }

        Token PrevToken(){
            return mTokens[mCurTokenIdx-1];
        }

        Token NextToken(){
            return mTokens[mCurTokenIdx+1];
        }

        bool AtEnd(){
            return mCurTokenIdx+1 == mTokens.size();
        }

        Parser();
        ~Parser();
    };

    class Script {
        std::map<std::string, SkitterValue> mVars;
        mb::Tree<AstNode> mTree;
    public:
        Script();
        Script(std::string);
        ~Script();
    };

}

#endif