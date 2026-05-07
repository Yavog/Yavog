#include "Token.hpp"
#include "Parser/Source.hpp"
#include "Parser/SourceCollection.hpp"
#include <fstream>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <filesystem>
#include <iostream>

Token::Token(Token* parent,TokenType type):parent(parent),type(type)
{
    static_assert( std::numeric_limits<double>::is_iec559, "IEEE 754 floating point" );
    static_assert( std::numeric_limits<float >::is_iec559, "IEEE 754 floating point" );

    if(type == TokenType::DOUBLE)
        _double = 0;
    if(type == TokenType::INTEGER)
        _uint   = 0;

}
Token::~Token()
{
    for (auto &&c : children)
    {
        delete c;
    }
    
}
void Token::loadFrom(SourceCollection& collection, SourceId id){
    // #### set sourceID ####
    this->sourceId = id;
    
    assert(collection.has(id));
    LexInfo lexInfo{
        .sourceId = sourceId,
        .collection = collection,
        .index = 0,
        .code = &collection[sourceId].content
    };
    setStart(lexInfo);
    loadCode(lexInfo);
    setEnd  (lexInfo);
}
void Token::setStart(LexInfo& li){
    sourceId = li.sourceId;
    start    = li.index;
}
void Token::setEnd  (LexInfo& li){
    sourceId = li.sourceId;
    end      = li.index;
}
Token::LexInfo::operator char(){
    if(code == nullptr)
        return 0;
    if(code->size() <= index)
        return 0;
    return (*code)[index];
}
char Token::LexInfo::operator++(int){
    auto rv = operator char();
    index++;
    return rv;
}
void Token::LexInfo::error(std::string msg){
    if(collection.has(sourceId)){
        collection[sourceId].error(index, 1, msg);
    }
}
void Token::LexInfo::warning(std::string msg){
    if(collection.has(sourceId)){
        collection[sourceId].warning(index, 1, msg);
    }
}
void Token::LexInfo::hint   (std::string msg){
    if(collection.has(sourceId)){
        collection[sourceId].hint(index, 1, msg);
    }
}
void Token::LexInfo::ref    (std::string msg){
    if(collection.has(sourceId)){
        collection[sourceId].ref(index, 1, msg);
    }
}

Token::LexInfo& Token::LexInfo::operator=(const LexInfo& li){
    assert(&this->collection == &li.collection);
    this->code     = li.code;
    this->index    = li.index;
    this->sourceId = li.sourceId;
    return *this;
}

bool Token::isIDstart(char c){
    if(c>='a'&&c<='z')
        return true;
    if(c>='A'&&c<='Z')
        return true;
    if(c=='_')
        return true;
    return false;
}
bool Token::isIDcontinue(char c){
    if(c>='0'&&c<='9')
        return true;
    if(c>='a'&&c<='z')
        return true;
    if(c>='A'&&c<='Z')
        return true;
    if(c=='_')
        return true;
    return false;
}
bool Token::isNumeric(char c){
    return c>='0'&&c<='9';
}
bool Token::isOctal(char c){
    return c>='0'&&c<='7';
}
bool Token::isWhitespace(char c){
    if(c==' '||c=='\t'||c=='\n'||c=='\r')
        return true;
    return false;
}
bool Token::isSymbol(char c){
    const std::string symbols = "!#$%&'*+,-./:;<=>?@\\^_`|~";
    return (symbols.find(c)!=std::string::npos);
}
bool Token::isHexadecimal(char c){
    if('0'<=c&&c<='9')
        return true;
    if('a'<=c&&c<='f')
        return true;
    if('A'<=c&&c<='F')
        return true;
    return false;
}
std::int8_t Token::hex2Int(char c){
    if('0'<=c&&c<='9')
        return c-'0';
    if('a'<=c&&c<='f')
        return c-'a'+10;
    if('A'<=c&&c<='F')
        return c-'A'+10;
    return 0;
}
std::string Token::toUTF8(std::int64_t v){
    if(v<128)
        return {(char)v};
    std::string rv;
    if(v<2048){
        rv = std::string(2,'\0');
        rv[1] = 0xC0;
    }
    else if(v<65536){
        rv = std::string(3,'\0');
        rv[2] = 0xE0;
    }else{
        rv = std::string(4,'\0');
        rv[3] = 0xF0;
    }
    size_t i = 0;
    while(v!=0){
        rv[i++] |= v & 0x3F + 0x80;
        v>>=6;
    }
    std::reverse(rv.begin(), rv.end());
    return rv;
}
std::string Token::toHexRepresentation(double d){
    
    bool positiv = true;
    if(d<0){
        positiv = false;
        d=-d;
    }

    int64_t exponent = 0;
    while(d>=1){
        exponent++;
        d/=2;
    }
    while(d!=0 && d<0.5){
        exponent--;
        d*=2;
    }

    std::string digits = "";
    const double g = 1/16.f; 
    while(d!=0){
        int a = (d/g);
        d-=a*g;
        d*=16;
        if(0<=a&&a<=9)
            digits+= a+'0';
        else if(10<=a&&a<=15)
            digits+= a+'a'-10;
    }   
    return (std::string)(positiv?"":"-")+"0x."+digits+"P"+std::to_string(exponent);
} 

void Token::loadCode(LexInfo& i,char end){
    std::string preComment;
    LexInfo commentLoc(i);
    while(i){
        skipWhitespace(i);
        if(!i)
            break;
        if(i == end)
            break;
        if(loadComment(i,preComment)){
            commentLoc = i;
            continue;
        }
        if(!loadIdentifier(i))
            if(!loadParentheses(i))
                if(!loadString(i))
                    if(!loadSymbol(i))
                        if(!loadNumber(i))
            //handle unknown tokens.
            {
                if(i==')'||i==']'||i=='}'){
                    if(end==0)
                        i.error((std::string)"Unexpected closing bracket '"+(char)i+"'");
                    else    
                        i.error((std::string) "Unexpected closing bracket '"+(char)i+"' instead of '"+(char)end+"'");
                    i++;
                    continue;
                }
                else{
                    auto tok = new Token(this,TokenType::DEBUG);
                    tok->setStart(i);
                    tok->setEnd(i);
                    children.push_back(tok);
                    tok->_string = "unknown: '";
                    tok->_string += i;
                    tok->_string += "'";
                    i.error((std::string)"unknown character '"+(char)i+"' "+std::to_string(i));
                    i++;
                }
            }
        if(children.size()){
            auto last = children[children.size()-1];
            //if(last->start.line<=commentLC.line){
            last->preComment += preComment;
            //}else this->postComment += preComment;
            preComment =  "";
        }
    }
    // if(children.size()){
    //     children[children.size()-1]->postComment += preComment;
    //     preComment =  "";
    // }else{
    //     this->innerComment += preComment;
    // }
    if(end!=i && end!=0){
        i.error((std::string)"expected '"+end+"' instead of EOL ");
    }
}

void Token::print(SourceCollection& collection,std::string tab){
    assert(collection.has(sourceId));
    auto startPos = collection[sourceId].getline(start);
    auto endPos = collection[sourceId].getline(end);

    std::cout <<std::setw(4)<< sourceId<<"\t"
              <<std::setw(4)<< (startPos.line+1)<<":"
              <<std::setw(4)<< (startPos.column+1)<<" -"
              <<std::setw(4)<< (endPos.line+1)<<":"
              <<std::setw(4)<< (endPos.column+1)<<"\t";
    auto printComment = [&](){
        #if false
        if(preComment.size())
        std::cout <<"                    \t"<< tab << "#"<<preComment<< (preComment[preComment.size()-1]=='\n'?"":"\n");
        if(postComment.size())
        std::cout <<"                    \t"<< tab << "//"<<postComment<<(postComment[postComment.size()-1]=='\n'?"":"\n");
        #endif
    };
    if(type == TokenType::ROOT){
        std::cout << tab<<"ROOT"<<std::endl;
        printComment();
        for(auto& x: children)
            x->print(collection,tab+"\t");
    }
    if(type == TokenType::IDENTIFIER  ){
        std::cout << tab << "ID "<<_string << std::endl;
        printComment();
    }
    if(type == TokenType::SYMBOL  ){
        std::cout << tab << "SYM "<<_string << std::endl;
        printComment();
    }
    if(type == TokenType::BRACKET_CURLY||type == TokenType::BRACKET_ROUND || type == TokenType::BRACKET_SQUARE  ){
        std::cout << tab;
        if(type == TokenType::BRACKET_CURLY)
            std::cout << "CURLY"<<std::endl;
        if(type == TokenType::BRACKET_ROUND)
            std::cout << "ROUND"<<std::endl;
        if(type == TokenType::BRACKET_SQUARE)
            std::cout << "SQUARE"<<std::endl;
        printComment();
        for (auto &x : children){
            x->print(collection,tab+"\t");
        }   
    }
    if(type == TokenType::STRING){
        std::cout << tab << "STR "<<_string << std::endl;
        printComment();
    }
    if(type == TokenType::CHAR){
        std::cout << tab << "CHAR "<<_string << std::endl;
        printComment();
    }
    if(type == TokenType::DEBUG){
        std::cout << tab<<"[DEBUG]"<<_string<<std::endl;
        printComment();
    }
    if(type == TokenType::INTEGER){
        std::cout << tab<<"INTEGER "<< _uint<<" from "<<_string<<std::endl;
        printComment();
    }
    if(type == TokenType::DOUBLE){
        std::cout << tab<<"double "<<_double<<" from "<<_string<<" hex:"<<toHexRepresentation(_double)<<std::endl;
        printComment();
    }
}
void Token::error  (SourceCollection& collection,std::string msg){
    if(collection.has(sourceId))
        collection[sourceId].error(start,end-start+1,msg);
}
void Token::warning(SourceCollection& collection,std::string msg){
    if(collection.has(sourceId))
        collection[sourceId].warning(start,end-start+1,msg);
}
void Token::hint   (SourceCollection& collection,std::string msg){
    if(collection.has(sourceId))
        collection[sourceId].hint(start,end-start+1,msg);
}
void Token::ref    (SourceCollection& collection,std::string msg){
    if(collection.has(sourceId))
        collection[sourceId].ref(start,end-start+1,msg);
}
void Token::skipWhitespace(LexInfo& i){
    while (isWhitespace(i)){
        i++;
    } 
}
bool Token::loadIdentifier(LexInfo& i){
    if(!isIDstart(i))
        return false;
    auto tok = new Token(this,TokenType::IDENTIFIER);
    tok->setStart(i);
    while (isIDcontinue(i)){    
        tok->_string += i++;
    }
    tok->setEnd(i);
    children.push_back(tok);
    return true;
}
bool Token::loadSymbol    (LexInfo& i){
    if(!isSymbol(i))
        return false;
    const std::vector<std::string> symbols = {
        //three
        "<=>","->*",">>=","<<=", /*additionally*/ "+%=","-%=","*%=","...",
        //two
        "::","++","--","->",".*","&&","||","<<",">>",
        "<=",">=","==","!=","+=","-=","*=","/=","%=","^=","&=","|=",
        /*additionally*/ "+%","-%","*%","..",
        //one
        ".","+","-","*","/","%","~","!","&","<",">","^","|","?","@",":","=",",","#",";", /*additional*/ "$","^"
    }; 

    auto tok = new Token(this,TokenType::SYMBOL);
    tok->setStart(i);
    auto backup = i;
    std::string str;
    str += i++;
    str += i++;
    str += i++;
    i = backup;
    for (auto &&sym : symbols){
        bool valid = true;
        for (size_t j = 0; j < sym.size(); j++){
            if(str[j] != sym[j]){
                valid = false;
                break;
            }
        }
        if(!valid)
            continue;
        for (size_t j = 0; j < sym.size(); j++)
        {
            tok->_string += i++;
        }
        tok->setEnd(i);
        children.push_back(tok);
        return true;
    }
    i = backup;
    return false;
}
bool Token::loadParentheses(LexInfo& i){
    char start = i;
    if(start!='(' && start!='{' && start!='[')
        return false;

    Token* tok;
    if(start == '('){
        tok = new Token(this,TokenType::BRACKET_ROUND);
        tok->setStart(i);
        i++;
        tok->loadCode(i,')');
        i++;
    }
    else if(start == '{'){
        tok = new Token(this,TokenType::BRACKET_CURLY);
        tok->setStart(i);
        i++;
        tok->loadCode(i,'}');
        i++;
    }
    else if(start == '['){
        tok = new Token(this,TokenType::BRACKET_SQUARE);
        tok->setStart(i);
        i++;
        tok->loadCode(i,']');
        i++;
    }
    tok->setEnd(i);
    children.push_back(tok);
    return true;    
}
bool Token::loadString(LexInfo& i){
    if(i!='"'&&i!='\'')
        return false;
    char start = i;
    auto tok = new Token(this,i=='"'?TokenType::STRING:TokenType::CHAR);
    tok->setStart(i);
    i++;
    while(i && i!=start){
        if(i=='\\'){
            i++;
            if(!i){
                i.error((std::string)"Unexpected EOL expected escape sequence");
                break;
            }
            if(i=='/') {tok->_string += 0x2f;i++;}else 
            if(i=='\''){tok->_string += 0x27;i++;}else 
            if(i=='"') {tok->_string += 0x22;i++;}else 
            if(i=='?') {tok->_string += 0x3f;i++;}else 
            if(i=='\\'){tok->_string += 0x5c;i++;}else 
            if(i=='a') {tok->_string += 0x07;i++;}else 
            if(i=='b') {tok->_string += 0x08;i++;}else 
            if(i=='f') {tok->_string += 0x0c;i++;}else 
            if(i=='n') {tok->_string += 0x0a;i++;}else 
            if(i=='r') {tok->_string += 0x0d;i++;}else 
            if(i=='t') {tok->_string += 0x09;i++;}else
            if(i=='v') {tok->_string += 0x0b;i++;}else
            if(i=='o'){
                i++;
                //octal numbers  {unlimited}
                if(i!='{'){
                    i.error((std::string)"expected '{' after '\\o'");
                    continue;
                }
                i++;
                unsigned int value = 0;
                while(i!='}'){
                    if(!isOctal(i)){
                        i.error((std::string)"Expected octal digits ");
                        break;
                    }
                    value *= 8;
                    value += i-'0';
                    i++;
                }
                tok->_string += toUTF8(value);   
            }else 
            if(isOctal(i)){
                //octal numbers  (3 max)
                unsigned int value = 0;
                for (size_t j = 0; j < 3; j++){
                    if(!isOctal(i))
                        break;
                    value *= 8;
                    value += i-'0';
                    i++;
                }
                tok->_string += toUTF8(value);   
            }else 
            if(i=='x') {
                i++;
                bool needCurlyBrackets = false;
                if(i=='{'){
                    i++;
                    needCurlyBrackets = true;
                }
                //hex numbers 
                std::uint64_t value = 0;
                while (isHexadecimal(i))
                {
                    value *= 16;
                    value += hex2Int(i);
                    i++;
                }
                tok->_string += toUTF8(value);
                if(needCurlyBrackets){
                    if(i!='}'){
                        i.error((std::string)"Expected '}' to end escape sequence");
                    }else i++;
                }
            }else
            if(i=='u') {
                i++;
                //unicode 4 hex or {unlimited}
                bool unlimited = false;
                if(i=='{'){
                    i++;
                    unlimited = true;
                }
                std::uint64_t value = 0;
                for (size_t j = 0; j < 4 || unlimited; j++)
                {
                    if( !isHexadecimal(i)){
                        if(!unlimited)
                            i.error((std::string)"Expected 4 hexadecimal characters ");
                        break;
                    }
                    value *= 16;
                    value += hex2Int(i);
                    i++;
                }
                tok->_string += toUTF8(value);
                if(unlimited){
                    if(i!='}'){
                        i.error((std::string)"Expected '}' to end escape sequence");
                    }else i++;
                }
            }else
            if(i=='U') {
                i++;
                //unicode 8 hex
                std::uint64_t value = 0;
                for (size_t j = 0; j < 8; j++)
                {
                    if( !isHexadecimal(i)){
                        i.error((std::string)"Expected 8 hexadecimal characters ");
                        break;
                    }
                    value *= 16;
                    value += hex2Int(i);
                    i++;
                }
                tok->_string += toUTF8(value);
            }else 
            if(i=='N'){
                i.error((std::string)"UNIMPLEMENTED '\\N{n-char-sequence}'");
                i++;
                /*TODO*/
            }else
                i.error((std::string)"Unknown escape sequence: '"+(char)i+"' ("+std::to_string((int)(unsigned char)i)+")");            
        }else{
            tok->_string += i;
            i++;
        }
    }
    if(!i)
        i.error((std::string)"Unexpected EOL");
    i++;
    tok->setEnd(i);
    children.push_back(tok);
    return true;

}
bool Token::loadNumber(LexInfo& i){
   
    //from https://en.cppreference.com/w/cpp/language/integer_literal
    /* 
        decimal: [1-9][0-9]* 
        octal:   0[0-7]*
        hex:     0(x|X)hexadecimals+
        binary:  0(b|B)(0|1)+
    */
    std::uint64_t rv = 0;
    auto startIndex = i;
    std::string _string;
    bool isHex = false;
    if(i=='0'){
        _string += i++;
        if((i=='x'||i=='X')){
            _string+=  i++;
            //hex
            while (isHexadecimal(i))
            {
                rv*=16;
                rv+=hex2Int(i);
                _string+= i++;
            }
            isHex = true;
        }else if((i=='b'|i=='B')){
            _string+= i++;
            //bin
            while ( (i=='0'||i=='1'))
            {
                rv*=2;
                rv+=i-'0';
                _string+= i++;
            }
        }else{
            //octal
            while ((i>='0'&& i<='7'))
            {
                rv*=8;
                rv+=i-'0';
                _string+= i++;
            }
        }
    }else if(isNumeric(i)){
        //decimal
        while ( isNumeric(i))
        {
            rv*=10;
            rv+=i-'0';
            _string+= i++;
        }
        
    }else if(i!='.'){
        return false;
    }
    if(not(i=='.'||(!isHex&&(i=='e'||i=='E'))||(isHex&&(i=='p'||i=='P')))){
        auto num = new Token(this,TokenType::INTEGER);
        num->setStart(startIndex);
        num->setEnd(i);
        num->_uint = rv;
        num->_string = _string; 
        children.push_back(num);
        return true;
    }
    i = startIndex;
    //https://en.cppreference.com/w/cpp/language/floating_literal
    /*

    [0-9]+        decimalexponent
    [0-9]+.       decimalexponent?
    [0-9]*.[0-9]+ decimalexponent?

    (0x|0X) [0-F]+        hexeponent
    (0x|0X) [0-F]+.       hexeponent?
    (0x|0X) [0-F]*.[0-F]+ hexeponent?
    */
    //decimalexponent: (e|E)(+|-)?[0-9]+
    //hexexponent:     (p|P)(+|-)?[0-9]+
    return loadNumberFloat(i,isHex);    
}
bool Token::loadNumberFloat(LexInfo&i,bool hex){
    auto start = i;
    std::string _string; 
    if(!hex){
        // [0-9]+        decimalexponent   
        // [0-9]+.       decimalexponent?
        // [0-9]*.[0-9]+ decimalexponent?

        std::string digitsequence;
        std::string afterPoint;
        bool point = false;
        while (isNumeric(i)){
            digitsequence += i++;
        }
        _string += digitsequence;
        if(i=='.'){
            point = true;
            _string+= i++;
            while (isNumeric(i))
                afterPoint += i++;
            _string += afterPoint;
        }
        
        if(digitsequence.size()==0 && afterPoint.size()==0){
            // it was just a point
            i = start;
            return false;
        }

        //decimalexponent: (e|E)(+|-)?[0-9]+
        if(!point && i!='e'&&i!='E')
            i.error("expected decimal exponent. 'e' or 'E'");
        
        std::string exponent;
        if(i=='e'||i=='E'){
            _string += i++;
            if(i=='+'||i=='-'){
                exponent += i++;
            }
            if(!isNumeric(i))
                i.error("expected a digit [0-9] for exponent.");
            while (isNumeric(i))
            {
                exponent += i++;
            }
            _string+= exponent;
        }  
        //convert to double
        auto num = new Token(this,TokenType::DOUBLE);
        num->setStart(start);
        num->setEnd(i);
        num->_double = std::stold(_string);
        num->_string = _string;
        children.push_back(num);
        
        return true;
    }else{
        // (0x|0X) [0-F]+        hexeponent
        // (0x|0X) [0-F]+.       hexeponent
        // (0x|0X) [0-F]*.[0-F]+ hexeponent
        //hexexponent:     (p|P)(+|-)?[0-9]+
        if(i!='0')
            i.error("compilerbug.");
        _string+= i++;
        if(i!='x'&&i!='X')
            i.error("compilerbug.");
        _string+= i++;

        std::string digitsequence;
        std::string afterPoint;
        bool point = false;
        while (isHexadecimal(i))
            digitsequence += i++;
        _string+= digitsequence;
        if(i=='.'){
            point = true;
            _string+= i++;
            while (isHexadecimal(i))
                afterPoint += i++;
            _string+= afterPoint; 
        }
        
        if(digitsequence.size()==0 && afterPoint.size()==0){
            // it was just a point
            i.error("Expected fraction digits.");
        }

        //hexexponent:     (p|P)(+|-)?[0-9]+
        if(i!='p'&&i!='P')
            i.error("expected hexadecimal exponent. 'p' or 'P'");
        std::string exponent;
        if(i=='p'||i=='P'){
            _string+= i++;
            if(i=='+'||i=='-'){
                exponent += i++;
            }
            if(!isNumeric(i))
                i.error("expected a digit [0-9] for exponent.");
            while (isNumeric(i))
            {
                exponent += i++;
            }
            _string+= exponent;
        }
        //convert to double
        auto num = new Token(this,TokenType::DOUBLE);
        num->setStart(start);
        num->setEnd(i);
        num->_double = std::stold(_string);
        num->_string = _string;
        children.push_back(num);
        return true;
    }
}

bool Token::loadComment(LexInfo&i,std::string& preComment){
    auto start = i;
    if(i!='/')
        return false;
    i++;
    if(i!='/'&&i!='*'){
        i = start;
        return false;
    }
    // single line comment
    std::string comment;
    if(i=='/'){
        i++;
        while(i){
            comment += i++;
            if(i == '\n')
                break;
        }
    }
    // Multiline comment
    else if(i=='*'){
        i++;
        while(true){
            if(i==0){
                i.error("Unexpected EOL. Expected multiline comment to end.");
                break;
            }
            if(i=='*'){
                i++;
                if(i=='/'){
                    i++;
                    skipWhitespace(i);
                    break;
                }else
                    comment+='*';
            }else 
                comment+= i++;
        }
    }
    //post-comment?
    // if(children.size()&&children[children.size()-1]->end.line == start.position.line){
    //     children[children.size()-1]->postComment += comment;
    // }else{
    //     preComment += comment;
    // }
    preComment += comment;
    return true;
}
