#include "sexpParser.hpp"

using namespace std;
using namespace sexpParser;

int main(int argc, char** argv){
    Parser p(&cin);
    Element e;
    p.parse(e);
    cout << e.toString() << endl;
    return 0;
}