#pragma once

#include "Parser/Token.hpp"
#include "Parser/TokenIterator.hpp"
#include <X11/XKBlib.h>
#include <cassert>
#include <map>
#include <string>
#include <vector>
class Json{
public:
    enum JsonType{
        OBJECT,
        ARRAY,
        STRING,
        NUMBER_INT,
        NUMBER_DOUBLE,
        TRUE,
        FALSE,
        _NULL,
    };
    std::string preComment;

    JsonType type;

    std::map<std::string,Json> members;
    std::vector<Json> elements;
    std::string string;

    union{
        int64_t     _int;
        double      _double;
    };

    Json():type(_NULL){
    }
    Json(JsonType type):type(type){

    }    

    static Json loadObject(const TokenIterator& bracket){
        Json json(JsonType::OBJECT);
        auto j = bracket;
        //members
        while(j.exist()){
            std::string key;
            std::string preComment;

            //key
            if(j.isString()){
                key = j.valueString();
            }else j.error("expected key to be a string.");
            j.forward(preComment);

            //colon
            if(!j.isSymbol(":"))
                j.error("expected ':'");
            j.forward(preComment);

            auto member= loadElement(j);
            member.preComment = preComment + member.preComment;

            if(j.exist()){
                if(!j.isSymbol(","))
                    j.error("expected ',' between members.");
                else j.forward(member.preComment);
            }
            json.members[key] = member;
        }
        return json;
    }
    static Json loadArray(const TokenIterator& bracket){
        Json json(JsonType::ARRAY);
        auto j = bracket;
        // elements
        while(j.exist()){
            auto element = loadElement(j);
            
            if(j.exist()){
                if(!j.isSymbol(","))
                    j.error("expected ',' between members.");
                else j.forward(element.preComment);
            }
            json.elements.push_back(element);
        }
        return json;
    }
    static Json loadElement(TokenIterator& i){
        /*
            object
            array
            string
            number
            true
            false
            null
        */
        //Object
        if(i.isCurly()){
            auto json = loadObject(i.inside());
            i.forward(json.preComment);
            return json;
        }
        //Array
        else if(i.isSquare()){
            auto json= loadArray(i.inside());
            i.forward(json.preComment);
            return json;
        }
        //String
        else if(i.isString()){
            Json json(JsonType::STRING);
            json.string = i.valueString();
            i.forward(json.preComment);
            return json;
        }
        //Number
        else if(i.isSymbol("-") || i.isNumber()){
            std::string preComment;
            bool sign = false;
            if(i.isSymbol("-")){
                sign = true;
                i.forward(preComment);
            }
            if(!i.isNumber())
                i.error("expected number");

            Json json(i.isInteger()?JsonType::NUMBER_INT:JsonType::NUMBER_DOUBLE);
            if(i.isInteger()){
                json._int = i.asInteger();
                json._int *= (sign?(-1):1);
            }
            else if(i.isDouble()){
                json._double = i.asDouble();
                json._double *= (sign?(-1):1);
            }
            json.preComment = preComment;
            i.forward(json.preComment);
            return json;
        }
        else if(i.isIdentifier("true")){
            auto json = Json(JsonType::TRUE);
            i.forward(json.preComment);
            return json;
        }else if(i.isIdentifier("false")){
            auto json = Json(JsonType::FALSE);
            i.forward(json.preComment);
            return json;
        }else if(i.isIdentifier("null")){
            auto json = Json(JsonType::_NULL);
            i.forward(json.preComment);
            return json;
        }
        i.error("invalid json token"); 
        auto json = Json(JsonType::_NULL); 
        i.forward(json.preComment);
        return json;
    }



    //TODO key,string escaping
    
    std::string write(std::string indentation, bool preCommentAlreadyWritten = false){
        auto writeComment = [](std::string indentation,std::string preComment)->std::string
        {
            size_t i = 0;
            std::vector<std::string> lines;
            while (i<preComment.size()) {
                // skip whitespace
                while (i<preComment.size() && Token::isWhitespace(preComment[i])) {
                    i++;
                }
                size_t newI = preComment.find('\n',i);
                std::string line = preComment.substr(i,newI-i);

                i = newI;

                lines.push_back(line);
            }
            //remove empty lines padding
            while(lines.size() && lines[lines.size()-1].size() == 0){
                lines.pop_back();
            }

            std::string rv = "// ";
            bool first = true;
            for (auto& line : lines) {
                if(first)
                    rv += line;
                else
                    rv += "\n"+indentation+"// "+line;
                first = false;
            }
            rv += "\n"+indentation;
            return rv;
        };
        auto escapeString = [](std::string str)->std::string
        {   
            std::string rv;
            for(unsigned char c:str){
                if(c == '"')
                    rv += "\"";
                else if(c == '\\')
                    rv += "\\\\";
                else if(c == '\b')
                    rv += "\\b";
                else if(c == '\f')
                    rv += "\\f";
                else if(c == '\n')
                    rv += "\\n";
                else if(c == '\r')
                    rv += "\\r";
                else if(c == '\t')
                    rv += "\\t";
                else if(c < 0x20){
                    rv += "\\u00";
                    auto upper = (c >> 4);
                    auto lower = (c & 0x0f);
                    rv += upper > 9? (char)(upper-10+'A'):upper+'0';
                    rv += lower > 9? (char)(lower-10+'A'):lower+'0';
                }
                else rv += c;
            }

            return "\""+rv+"\"";
        };

        std::string str;
        if(preComment.size() && !preCommentAlreadyWritten) 
            str += writeComment(indentation, preComment);
        std::string newIndentation = indentation + "\t";
        if(type == JsonType::OBJECT){
            str += "{";
            bool first = true;
            for(auto& pair:members){
                if(first){
                    str += "\n";
                }else{
                    str += ",\n";
                }
                str += newIndentation;
                if(pair.second.preComment.size())
                    str += writeComment(newIndentation, pair.second.preComment);
                str += escapeString(pair.first)+":"+pair.second.write(newIndentation,true);
                first = false;
            }
            str += "\n"+indentation+"}";
        }
        else if(type == JsonType::ARRAY){
            str += "[";
            bool first = true;
            for(auto& element:elements){
                if(first){
                    str += "\n";
                }else{
                    str += ",\n";
                }
                str += newIndentation + element.write(newIndentation);
                first = false;
            }
            str += "\n"+indentation+"]";
        }
        else if(type == JsonType::STRING)
            str += escapeString(string); 
        else if(type == JsonType::NUMBER_INT)
            str += std::to_string(_int);    
        else if(type == JsonType::NUMBER_DOUBLE)
            str += std::to_string(_double);    
        else if(type == JsonType::TRUE)
            str += "true";
        else if(type == JsonType::FALSE)
            str += "false";
        else if(type == JsonType::_NULL)
            str += "null";
        return str;
    }
private:
};