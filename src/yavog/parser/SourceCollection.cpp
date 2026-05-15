#include "parser/SourceCollection.hpp"
#include "parser/Source.hpp"
#include <cassert>
#include <memory>

SourceId SourceCollection::add(std::shared_ptr<Source> source){
    auto rv = sources.size();
    sources.push_back(source);
    return rv;
}
Source& SourceCollection::operator[](SourceId id){
    assert(sources.size()>id);
    return *sources[id];
}
bool SourceCollection::has(SourceId id){
    return sources.size()>id;
}