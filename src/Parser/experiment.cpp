
#include "Parser/Source.hpp"
#include "Parser/SourceCollection.hpp"
#include "Parser/Token.hpp"
#include "data/Filebasic.hpp"
#include <memory>
int experimentParser(){
    SourceCollection collection;
    auto data = readFileOrThrow("world/main.gui");
    std::string str(data.begin(),data.end());
    auto sourceId = collection.add(std::make_shared<Source>("world/main.gui",str));
    Token token;
    token.loadFrom(collection,sourceId);
    token.print(collection, "");
    token.children[0]->error(collection, "unknown keyword");
    return 0;
}