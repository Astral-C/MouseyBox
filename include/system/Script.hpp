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
        Comparison,
        Term,
        Factor,
        Literal,
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

        bool Declarations(mb::TreeNode<AstNode>*);
        bool Declaration(mb::TreeNode<AstNode>*);
        bool Expression(mb::TreeNode<AstNode>*);

        bool Term(Token, Token, mb::TreeNode<AstNode>*);
        bool Factor(mb::TreeNode<AstNode>*); //Token, Token, ?
        bool Comparison(Token, Token, mb::TreeNode<AstNode>*);

        bool Statement(mb::TreeNode<AstNode>*);
        bool IfStatement(mb::TreeNode<AstNode>*);
        bool AssignStatement(mb::TreeNode<AstNode>*);
        bool Literal(mb::TreeNode<AstNode>*);

        Token PeekToken(int offset=0){
            return mTokens[mCurTokenIdx+offset];
        }

        Token ConsumeToken(){
            return mTokens[mCurTokenIdx++];
        }

        Token PrevToken(){
            return mTokens[mCurTokenIdx-1];
        }

        Token NextToken(){
            return mTokens[mCurTokenIdx+1];
        }

        bool AtEnd(){
            return mCurTokenIdx == mTokens.size();
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