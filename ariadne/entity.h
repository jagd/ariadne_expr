#ifndef ENTITY_H
#define ENTITY_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <fstream>
#include <string>
#include <list>
#include <memory>
class entity {
public:
    entity();
    virtual ~entity();
    virtual void print() const { }
    void setName(const std::string& name) { _name = name; }
    std::string getName() const { return _name; }
    void setStatus(bool status) { _status = status; }
    bool getStatus() const { return _status; }
    virtual void set(std::shared_ptr<entity> ent) { }
    virtual std::shared_ptr<entity> getAttributeValue(const std::string &attr) const { return 0; }
    void setDescription(const std::string& description) { _description = description; }
    std::string getDescription() const { return _description; }
protected:
    bool _status;
    std::string _name;
    std::string _description;
};

class entity_list {
public:
    entity_list();
    virtual ~entity_list();
    void push_back(std::shared_ptr<entity> ent);
    void push_front(std::shared_ptr<entity> ent);
    void insert(const std::string& name, std::shared_ptr<entity> ent);
    void clear();
    bool empty() const;
    int size() const;
    void setType(const std::string& type) { _type = type; }
    std::string getType() const { return _type; }
    void erase(const std::string& name);
    void rename(const std::string& oldname, const std::string& newname);
    void setDescription(const std::string& name, const std::string& description);
    void setStatus(const std::string& name, bool status);
    void print() const;
    void print(const std::string& name) const;

    std::shared_ptr<entity> back() const;
    std::shared_ptr<entity> front() const;
    std::shared_ptr<entity> pfind(const std::string& name);
    const std::shared_ptr<entity> pfind(const std::string& name) const;
    std::list<std::shared_ptr<entity> >& getList();
    const std::list<std::shared_ptr<entity> >& getList() const;
    std::list<std::shared_ptr<entity> >::iterator find(const std::string& name);
    std::list<std::shared_ptr<entity> >::const_iterator find(const std::string& name) const;
    bool check(const std::string& name) const;

protected:
    std::string _type;
    std::list<std::shared_ptr<entity> > _list;
};

#pragma GCC diagnostic pop
#endif // ENTITY_H
