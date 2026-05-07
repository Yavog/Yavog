#include "TokenIterator.hpp"
#include "Parser/Token.hpp"

TokenIterator::TokenIterator(SourceCollection& collection,Token* parent,size_t index):collection(collection),parent(parent),index(index){
}

//movement
void TokenIterator::forward(std::string& preComment){
    if(exist()){
        preComment += getPrecomment();
        index++;
    }
}
TokenIterator TokenIterator::inside(){
    if(exist()){
        return TokenIterator(collection,parent->children[index],0);
    }
    return *this;
}

//checks
bool TokenIterator::isIdentifier()const{
    if(exist())
        return parent->children[index]->type == TokenType::IDENTIFIER;
    return false;
}
bool TokenIterator::isIdentifier(std::string id)const{
    if(exist())
        return parent->children[index]->type == TokenType::IDENTIFIER && valueIdentifier() == id;
    return false;
}
bool TokenIterator::isString()const{
    if(exist())
        return parent->children[index]->type == TokenType::STRING;
    return false;
}
bool TokenIterator::isChar()const{
    if(exist())
        return parent->children[index]->type == TokenType::CHAR;
    return false;
}
bool TokenIterator::isSymbol()const{
    if(exist())
        return parent->children[index]->type == TokenType::SYMBOL;
    return false;
}
bool TokenIterator::isSymbol(std::string sym)const{
    if(exist())
        return parent->children[index]->type == TokenType::SYMBOL && valueSymbol() == sym;
    return false;
}

bool TokenIterator::isSquare()const{
    if(exist())
        return parent->children[index]->type == TokenType::BRACKET_SQUARE;
    return false;
}
bool TokenIterator::isRound()const{
    if(exist())
        return parent->children[index]->type == TokenType::BRACKET_ROUND;
    return false;
}
bool TokenIterator::isCurly()const{
    if(exist())
        return parent->children[index]->type == TokenType::BRACKET_CURLY;
    return false;
}

bool TokenIterator::isDouble()const{
    if(exist())
        return parent->children[index]->type == TokenType::DOUBLE;
    return false;
}
bool TokenIterator::isInteger()const{
    if(exist())
        return parent->children[index]->type == TokenType::INTEGER;
    return false;
}
bool TokenIterator::isNumber()const{
    return isInteger()||isDouble();
}

std::string TokenIterator::valueIdentifier()const{
    if(!exist())
        return "";
    return parent->children[index]->_string;
}
std::string TokenIterator::valueString()const{
    if(!exist())
        return "";
    return parent->children[index]->_string;
}
std::string TokenIterator::valueChar()const{
    if(!exist())
        return "";
    return parent->children[index]->_string;
}
std::string TokenIterator::valueSymbol()const{
    if(!exist())
        return "";
    return parent->children[index]->_string;
}
uint64_t    TokenIterator::asInteger()const{
    if(!exist())
        return 0;
    if(isInteger())
        return parent->children[index]->_uint;
    if(isDouble())
        return parent->children[index]->_double;
    return 0;
}
double      TokenIterator::asDouble()const{
    if(!exist())
        return 0;
    if(isInteger())
        return parent->children[index]->_uint;
    if(isDouble())
        return parent->children[index]->_double;
    return 0;
}

bool TokenIterator::exist()const{
    if(!parent)
        return false;
    return parent->children.size() > index;
}

void TokenIterator::error  (std::string msg){
    if(exist())
        parent->children[index]->error(collection,msg);
    else if(collection.has(parent->sourceId))
        collection[parent->sourceId].error(parent->end,1,msg);
}
void TokenIterator::warning(std::string msg){
    if(exist())
        parent->children[index]->warning(collection,msg);
    else if(collection.has(parent->sourceId))
        collection[parent->sourceId].warning(parent->end,1,msg);
}
void TokenIterator::hint   (std::string msg){
    if(exist())
        parent->children[index]->hint(collection,msg);
    else if(collection.has(parent->sourceId))
        collection[parent->sourceId].hint(parent->end,1,msg);
}
void TokenIterator::ref    (std::string msg){
    if(exist())
        parent->children[index]->ref(collection,msg);
    else if(collection.has(parent->sourceId))
        collection[parent->sourceId].ref(parent->end,1,msg);
}
std::string TokenIterator::getPrecomment(){
    if(exist())
        return parent->children[index]->preComment;
    return "";
}