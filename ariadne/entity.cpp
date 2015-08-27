#include <iostream>
#include "entity.h"

#include <stdexcept>
typedef std::logic_error catch_error;

using namespace std;

entity::entity() {

}

entity::~entity() {

}
entity_list::entity_list() {

}

entity_list::~entity_list() {

}

void entity_list::push_back(std::shared_ptr<entity> ent) {
    if(check(ent->getName())) throw catch_error("that name is already used");
    _list.push_back(ent);
}

void entity_list::push_front(std::shared_ptr<entity> ent) {
    if(check(ent->getName())) throw catch_error("that name is already used");
    _list.push_front(ent);
}

void entity_list::insert(const string& name, std::shared_ptr<entity> ent) {
   if(!check(name)) throw catch_error("no such entity");
   list<std::shared_ptr<entity> >::iterator iter=find(name);
   _list.insert(iter, ent);
}

void entity_list::clear() {
    for(list<std::shared_ptr<entity> >::iterator iter=_list.begin();iter!=_list.end(); iter++) {
        cout<<(*iter)->getName()<<endl;
    }
    _list.clear();
}

bool entity_list::empty() const {
    return _list.empty();
}

int entity_list::size() const {
    return (int) _list.size();
}

void entity_list::erase(const string& name) {
    if(!check(name)) throw catch_error("no such entity");
    list<std::shared_ptr<entity> >::iterator iter = find(name);
    _list.erase(iter);
}

void entity_list::rename(const string& oldname, const string& newname) {
    if(!check(oldname)) throw catch_error("no such entity");
    if(check(newname)) throw catch_error("that name is occupied by another entity");
    list<std::shared_ptr<entity> >::iterator iter = find(oldname);
    (*iter)->setName(newname);
}

void entity_list::setDescription(const string& name, const string& description) {
    if(!check(name)) throw catch_error("no such entity");
    list<std::shared_ptr<entity> >::iterator iter = find(name);
    (*iter)->setDescription(description);
}

void entity_list::setStatus(const string& name, bool status) {
    if(!check(name)) throw catch_error("no such entity");
    list<std::shared_ptr<entity> >::iterator iter = find(name);
    (*iter)->setStatus(status);
}

void entity_list::print() const {
    for(list<std::shared_ptr<entity> >::const_iterator iter=_list.begin();iter!=_list.end(); iter++) {
        cout<<"\t"<<(*iter)->getName()<<"\t"<<(*iter)->getDescription()<<endl;
    }
}

void entity_list::print(const std::string& name) const {
    if(!check(name)) throw catch_error("no such entity");
    const list<std::shared_ptr<entity> >::const_iterator iter = find(name);
    (*iter)->print();
}

std::shared_ptr<entity> entity_list::back() const {
    return _list.back();
}

std::shared_ptr<entity> entity_list::front() const {
    return _list.front();
}

std::shared_ptr<entity> entity_list::pfind(const std::string& name) {
    return *find(name);
}

const std::shared_ptr<entity> entity_list::pfind(const std::string& name) const {
    return *find(name);
}

list<std::shared_ptr<entity> >& entity_list::getList() {
    return _list;
}

const list<std::shared_ptr<entity> >& entity_list::getList() const {
    return _list;
}

list<std::shared_ptr<entity> >::iterator entity_list::find(const string& name) {
    list<std::shared_ptr<entity> >::iterator _iter;
    for(_iter=_list.begin();_iter!=_list.end(); _iter++)
            if((*_iter)->getName() == name) break;
    if(_iter==_list.end()) throw catch_error("no such entity");
    return _iter;
}

list<std::shared_ptr<entity> >::const_iterator entity_list::find(const string& name) const {
    list<std::shared_ptr<entity> >::const_iterator _iter;
    for(_iter=_list.begin();_iter!=_list.end(); _iter++)
        if((*_iter)->getName() == name) break;
    if(_iter==_list.end()) throw catch_error("no such entity");
    return _iter;
}

bool entity_list::check(const string& name) const {
    bool exist = false;
    /*
    for (const auto &i : _list) {
        if (i->getName() == name){
          +++
    }
    */
    for(list<std::shared_ptr<entity> >::const_iterator _iter=_list.begin();_iter!=_list.end(); _iter++)
        if((*_iter)->getName() == name) {
            exist = true;
            break;
        }
    return exist;
}
