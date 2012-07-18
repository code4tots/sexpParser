class ParserException(Exception):
    def __init__(self,location,msg):
        super(ParserException,self).__init__(str(location)+msg)
        
class Location(object):
    def __init__(self,programString):
        self.lineno     = 1
        self.colno      = 0
        self.program    = programString
        
        self.counter    = 0
        self.colstack   = []
        self.linestack  = []
        self.line       = ''
        
    def clone(self):
        ret = Location(self.program)
        ret.lineno = self.lineno
        ret.colno = self.colno
        ret.counter = self.counter
        ret.line = self.line
        return ret
        
    def prev(self):
        ret = ''
        
        if self.counter < len(self.program):
            ret = self.program[self.counter]
        
        if ret == '\n':
            self.colno  = self.colstack.pop()
            self.line   = self.linestack.pop()
            self.lineno -= 1
            
        elif self.colno == 0:
            # If it is the beginning of the document, we should do nothing.
            pass
            
        else:
            self.colno -= 1
            self.line = self.line[:-1]
            
        if self.counter > 0:
            self.counter -= 1
            
        return ret
        
    def next(self):
        ret = ''
        
        if self.counter < len(self.program):
            ret = self.program[self.counter]
            if ret == '\n':
                self.colstack.append(self.colno)
                self.linestack.append(self.line)
                self.colno      = 0
                self.line       = ''
                self.lineno     += 1
            else:
                self.colno      += 1
                self.line       += ret
            
            self.counter += 1
        
        return ret
    
    def currentLine(self):
        ncount  = 1
        pos     = 0
        
        lend    = self.counter
        while lend < len(self.program) and self.program[lend] != '\n': lend += 1
        
        line    = self.program[self.counter-self.colno:lend]
        return line
        
        
    def __str__(self):
        return 'line '+str(self.lineno)+' col '+str(self.colno+1)+'\n'+ \
                self.currentLine()+'\n'+ \
                (self.colno*' ')+'*\n'
        

class Atom(str):
    def __repr__(self): return str(self)
    def __str__(self): return super(Atom,self).__str__()
    
class SingleQuoteString(str): pass
class DoubleQuoteString(str): pass
    
class Parser:
    def __init__(self,program):
        self.location = Location(program)
        self.c = 'x'    # It's important self.c != '', because
                        # self.c == '' signifies file has been
                        # finished.
        # Initialize self.c
        self.read()
    
    def read(self): self.c = self.location.next()
    
    def currentLocation(self): return self.location.clone()
    
    # Parse the sequence of S-expressions
    def parse(self):
        ret = []
        
        try:
            # self.c was initialized during __init__
            self.clearWhitespace()
            while self.c != '':
                ret.append(self.parseSingle())
                self.clearWhitespace()
                
        except ParserException as e:
            ret = None
            print(e)
            
        return ret
            
    def parseSingle(self):
        ret = None
        
        if self.c == '':
            pass
        elif self.c in ['"',"'"]:
            ret = self.parseString()
        elif self.c == '(':
            ret = self.parseList()
        elif self.c == ')':
            savedLocation = self.location.clone()
            savedLocation.prev()
            raise ParserException(savedLocation,'unmatched close paren!')
        else:
            ret = self.parseAtom()
            
        return ret
        
    def clearWhitespace(self):
        while self.c != '' and self.c.isspace(): self.read()
        
    def parseString(self):
        savedLocation = self.location.clone()
        savedLocation.prev()
        # quote better be one of ' or ". Otherwise, something
        # has gone terribly wrong...
        quote = self.c
    
        self.read() # consume quote
        
        ret = ''
        
        while self.c != quote:
            # if we hit a premature end,
            # we couldn't find the end quote!
            if self.c == '':
                raise ParserException(savedLocation,'could not find end quote!')
            # If there is an escape we want to make sure the character right after
            # it is included, even if it were a matching quote. However, the
            # escapes are not actually applied here.
            elif self.c == '\\':
                ret += self.c
                self.read()
                ret += self.c
                self.read()
            # If it's a normal charcter, just add it.
            else:
                ret += self.c
                self.read()
        
        if quote == '"':
            ret = DoubleQuoteString(ret)
        elif quote == "'":
            ret = SingleQuoteString(ret)
        else:
            raise ParserException(savedLocation,'a non-quote character was used as a quote!')
        
        self.read() # consume end quote
        
        return ret
                
    def parseList(self):
        savedLocation = self.location.clone()
        savedLocation.prev()
        # The list better start with '(', otherwise something is terribly wrong.
        self.read() # consume '('
        
        ret = []
        
        self.clearWhitespace()
        while self.c != ')':
            if self.c == '':
                raise ParserException(savedLocation,'matching end paren could not be found!')
            
            ret.append(self.parseSingle())
            self.clearWhitespace()
            
        # The list better end with ')', otherwise something is terribly wrong.
        self.read() # consume ')'
        
        return ret
        
    def parseAtom(self):
        # Atoms better not start with (, ), ', or ", otherwise, something
        # is terribly wrong.
        
        ret = ''
        while not ((self.c in ['','(',')','"',"'"]) or self.c.isspace()):
            ret += self.c
            self.read()
        
        return Atom(ret)
    

def main():
    program = open('test.txt','r').read()
    result = Parser(program).parse()
    if result: print(result)
    
if __name__=='__main__': main()