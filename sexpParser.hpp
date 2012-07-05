#include <iostream>
#include <vector>
#include <string>
#include <ctype.h>

// In C++, unlike in some tamer langauges like Java or Python,
// large datastructures like vector are, by default, passed
// by value. This means that if the parser methods returned
// a vector instead of modifying a vector through references,
// the entire s-expression tree may be copied several times.
// Which is bad.

namespace sexpParser {

struct  Element;


// parsed element types
enum Type { atom, string, list, none };


// union that is only going to be used inside struct Element
// C++ doesn't let me use elements with destructors/constructors
// in unions, so I had to use pointers instead.
union Element_Union {
    std::vector<Element*>*      l;
    std::string*                s;
};

// a parsed element type.
struct Element {
    Type            t; // the type of element contained in u.
    Element_Union   u; // actual value of this Element.
    
    // for initialization and destruction.
    Element(){
        // t is set to none to indicate the the 
        // Element_Union pointer is invalid.
        t = none;
    }
    
    ~Element(){
        clean();
    }
    
    // clean up any dynamic memory this Element is holding.
    void clean(){
        if (t == none){
            // if it's none type, there is nothing to do.
        }
        else if (t == list){
            for (unsigned int i = 0; i < u.l->size(); i++){
                delete (*(u.l))[i];
            }
            delete u.l;
        }
        else {
            delete u.s;
        }
        t = none;
    }
    
    // for easy initialization into whatever type.
    void initList(){
        clean();
        t   = list;
        u.l = new std::vector<Element*>();
    }
    
    void initString(std::string s){
        clean();
        t   = string;
        u.s = new std::string(s);
    }
    
    void initAtom(std::string s){
        clean();
        t   = atom;
        u.s = new std::string(s);
    }
    
    // for debugging
    std::string toString(){
        std::string ret = "";
        if      (t == none){
            ret = "none";
        }
        else if (t == list){
            ret = "[";
            std::string sep = "";
            for (unsigned int i = 0; i < u.l->size(); i++){
                ret += sep + (*(u.l))[i]->toString();
                sep = ",";
            }
            ret += "]";
        }
        else if (t == string){
            ret = "\"" + *(u.s) + "\"";
        }
        else if (t == atom){
            ret = *(u.s);
        }
        return ret;
    }
};

class Parser{
public:
    Parser(std::istream* is){
        this->is = is;
        
        // when c == -1, only the public parse method should work.
        c = -1;
    }
    
    void parse(Element& ret){
        // initialize c.
        c = 0; read();
        // now actual parsing.
        pparse(ret);
    }
    
private:
    void clearWhitespace(){
        while (c != -1 && isspace(c)) { read(); }
    }

    // pparse --> private parse
    void pparse(Element& ret){
        ret.initList();
        
        clearWhitespace();
        Element* next = NULL;
        while (c != -1){
            next = new Element();
            parseSingle(*next);
            ret.u.l->push_back(next);
            
            clearWhitespace();
        }
    }
    
    void parseSingle(Element& ret){
        if      (c == '(') {
            parseList(ret);
        }
        else if (c == '"' || c == '\''){
            parseString(ret);
        }
        else {
            parseAtom(ret);
        }
    }
    
    void parseList(Element& ret){
        // consume '('
        read();
        
        ret.initList();
        
        clearWhitespace();
        Element* next = NULL;
        while (c != -1 && c != ')'){
            next = new Element();
            parseSingle(*next);
            ret.u.l->push_back(next);
            
            clearWhitespace();
        }
        
        // consume ')'
        read();
    }
    
    void parseString(Element& ret){
        int quote = c;
        
        read(); // consume quote
        
        std::string s = "";
        while (c != -1 && c!=quote){
            if (c != '\\'){
                s += (char) c;
                read();
            }
            else {
                read(); // consume '\\'
                
                switch (c){
                case '\\': s += '\\'; break;
                case 'n' : s += '\n'; break;
                case 't' : s += '\t'; break;
                case '"' : s += '\"'; break;
                case '\'': s += '\''; break;
                }
                
                read(); // consume indicated character
            }
        }
        read(); // consume end quote
        
        ret.initString(s);
    }
    
    void parseAtom(Element& ret){
        std::string s;
        while (c != -1 && !isspace(c) && c != '\'' && c != '"' && c != '(' && c != ')'){
            s += (char) c;
            read();
        }
        ret.initAtom(s);
    }
    

    void read(){
        if (c != -1 && is->good())  { c = is->get(); }
        else                        { c = -1; }
    }

    int c;
    std::istream* is;
};

}