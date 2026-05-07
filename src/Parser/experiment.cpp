
#include "Parser/Source.hpp"
#include "Parser/SourceCollection.hpp"
#include "Parser/Token.hpp"
#include "Parser/TokenIterator.hpp"
#include "data/Filebasic.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "Json.hpp"

int experimentParser(){
    SourceCollection collection;
    auto data = readFileOrThrow("world/settings.json");
    std::string str(data.begin(),data.end());
    auto sourceId = collection.add(std::make_shared<Source>("world/settings.json",str));
    Token token;
    token.loadFrom(collection,sourceId);
    token.print(collection, "");
    token.error(collection, "unknown keyword");
    TokenIterator it(collection,&token,0);
    // while (it.exist()) {
    //     it.error("hello there");
    //     it.forward();
    // }
    // it.error("hello there");

    auto json = Json::loadElement(it);
    auto test = Json(Json::JsonType::_NULL);
    test.preComment = "Something to test\n and use";
    json.members["test"] = test;
    auto str2 = json.write("");
    writeFileOrThrow("world/settings.json", std::vector<char>(str2.begin(),str2.end()));
    return 0;
}