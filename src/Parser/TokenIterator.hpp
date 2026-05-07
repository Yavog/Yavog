#pragma once
#include <cstddef>
#include <cstdint>
#include "Parser/Token.hpp"

class TokenIterator{
    Token* parent = nullptr;
    size_t index;
    SourceCollection& collection;
public:
    TokenIterator(SourceCollection& collection,Token* parent,size_t index);

    //movement
    void forward(std::string& preComment);
    TokenIterator inside();

    //checks
    bool isIdentifier()const;
    bool isIdentifier(std::string id)const;
    bool isString()const;
    bool isChar()const;
    bool isSymbol()const;
    bool isSymbol(std::string sym)const;
    
    bool isSquare()const;
    bool isRound()const;
    bool isCurly()const;

    bool isDouble()const;
    bool isInteger()const;
    bool isNumber()const;

    std::string valueIdentifier()const;
    std::string valueString()const;
    std::string valueChar()const;
    std::string valueSymbol()const;
    uint64_t    asInteger()const;
    double      asDouble()const;

    bool exist()const;

    void error  (std::string msg);
	void warning(std::string msg);
	void hint   (std::string msg);
	void ref    (std::string msg);

    std::string getPrecomment();
};