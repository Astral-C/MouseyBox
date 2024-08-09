#ifndef __MB_SCRIPT_H__
#define __MB_SCRIPT_H__
#include <cstdint>
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <system/Containers/Tree.hpp>
#include <type_traits>
#include <functional>

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
        PRINT,
        COMMA,
        CALL,
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
        Print,
        Call,
        Err
    };

    extern std::map<std::string, TokenType> ReservedTokens;

    extern std::map<TokenType, std::string> DebugTokenNames;

    enum class SkitterType {
        None,
        Number,
        Bool,
        String
    };

    struct SkitterValue {
        SkitterType mType;
        
        std::string mStrValue;
        double mNumValue;
        bool mBoolValue;
    
        template<typename T>
        T& value(){
            //static_assert(std::is_base_of<std::string, , T>::value, "T must be string, double, or bool");
            return T(); // this isnt good but it cant happen, template error if not one of the specialized templates
        }

        SkitterValue& operator=(std::string s){
            mType = SkitterType::String;
            mStrValue = s;
            return *this;
        }

        SkitterValue& operator=(bool b){
            mType = SkitterType::Bool;
            mBoolValue = b;
            return *this;
        }

        SkitterValue& operator=(double n){
            mType = SkitterType::Number;
            mNumValue = n;
            return *this;
        }
    
    };

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

        std::shared_ptr<TreeNode<AstNode>> Declarations();
        std::shared_ptr<TreeNode<AstNode>> Declaration();
        std::shared_ptr<TreeNode<AstNode>> Expression();

        std::shared_ptr<TreeNode<AstNode>> Literal();
        std::shared_ptr<TreeNode<AstNode>> Term();
        std::shared_ptr<TreeNode<AstNode>> Factor();
        std::shared_ptr<TreeNode<AstNode>> Comparison();
        std::shared_ptr<TreeNode<AstNode>> And();
        std::shared_ptr<TreeNode<AstNode>> Or();

        std::shared_ptr<TreeNode<AstNode>> Group();
        std::shared_ptr<TreeNode<AstNode>> Statement();
        std::shared_ptr<TreeNode<AstNode>> IfStatement();
        std::shared_ptr<TreeNode<AstNode>> CallStatement();
        std::shared_ptr<TreeNode<AstNode>> PrintStatement();
        std::shared_ptr<TreeNode<AstNode>> AssignStatement();

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
        std::map<std::string, std::function<SkitterValue(std::vector<SkitterValue>)>> mCallables;
        mb::Tree<AstNode> mTree;

        SkitterValue ExecNode(std::shared_ptr<mb::TreeNode<AstNode>>);

    public:

        void DumpVars();
        void DumpTree();
        void Execute();

        SkitterValue& operator[](std::string name) {
            return mVars[name];
        }
        
        std::function<SkitterValue(std::vector<SkitterValue>)>& operator()(std::string name) {
            return mCallables[name];
        }

        Script();
        Script(std::string);
        ~Script();
    };

}

#endif