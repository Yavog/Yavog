#pragma once
#include "SourceCollection.hpp"
#include <cassert>
#include <cstddef>

enum class TokenType{
    IDENTIFIER,
    INTEGER,
    STRING,
    CHAR,
    SYMBOL,
    BRACKET_SQUARE,
    BRACKET_ROUND,
    BRACKET_CURLY,
    DOUBLE,

    DEBUG,
    ROOT
};
class Token
{
    //debug data
    SourceId sourceId = -1;
    size_t   start;    
    size_t   end;    

    //type
    TokenType   type;

    //data
    std::string   _string;
    union{
        std::uint64_t _uint;
        double        _double;
    };

    //Tree
    std::vector<Token*> children;
    Token* parent = nullptr;

    //Loading ...
    struct LexInfo{
        SourceId          sourceId;
        SourceCollection& collection;

        size_t       index;
        std::string* code;
        

        LexInfo& operator=(const LexInfo&);

        operator char();
        char operator++(int);

        void error  (std::string msg);
	    void warning(std::string msg);
	    void hint   (std::string msg);
	    void ref    (std::string msg);

    };
    void setStart(LexInfo&);
    void setEnd  (LexInfo&);

    std::string preComment;

public:
    void error  (SourceCollection& collection,std::string msg);
	void warning(SourceCollection& collection,std::string msg);
	void hint   (SourceCollection& collection,std::string msg);
	void ref    (SourceCollection& collection,std::string msg);
    
    Token(Token* parent = nullptr,TokenType tokentype = TokenType::ROOT);
    Token(const Token& t)=delete;
    Token& operator=(Token&)=delete;
    ~Token();
    
    void loadFrom(SourceCollection& sh,SourceId id);
    
    void print(SourceCollection& sh,std::string tab);

    static bool isIDstart   (char c);
    static bool isIDcontinue(char c);
    static bool isNumeric    (char c);
    static bool isOctal      (char c);
    static bool isWhitespace (char c);
    static bool isSymbol     (char c);
    static bool isHexadecimal(char c);
    static std::int8_t hex2Int(char c);
    static std::string toUTF8 (std::int64_t v);
    static std::string toHexRepresentation(double d);
private:
    void loadCode(LexInfo& i,char end = 0);
    void skipWhitespace (LexInfo& i);
    bool loadComment    (LexInfo&i,std::string& preComment);
    bool loadIdentifier (LexInfo& i);
    bool loadSymbol     (LexInfo& i);
    bool loadParentheses(LexInfo& i);
    bool loadString     (LexInfo& i); // and char
    bool loadNumber     (LexInfo&i);
    bool loadNumberFloat(LexInfo&i,bool isHex);
};

