
class ParserException(Exception): pass

# parse S-expressions
class Parser:
    def __init__(self,f):
        self.f = f
        self.c = 'x'    # It's important self.c != '', because
                        # self.c == '' signifies file has been
                        # finished.
        # Initialize self.c
        self.read()
        
    def read(self):
        if self.c != '':
            self.c = self.f.read(1)
    
    # Actual parsing methods
    def parseAll(self):
        ret = []
        last = self.parse()
        while last != None:
            ret.append(last)
            last = self.parse()
        return ret
    
    def parse(self):
        self.clearWhitespace()
        
        ret = None
        
        # If self.c == '', the file has finished reading ...
        # there really is nothing more we can do...
        if self.c == '':
            pass
        elif self.c in ['"',"'"]:
            ret = self.parseString()
        elif self.c == '(':
            ret = self.parseList()
        else:
            ret = self.parseAtom()
            
        return ret
        
    def clearWhitespace(self):
        while self.c != '' and self.c.isspace():
            self.read()
        
    def parseString(self):
        if not (self.c in ['"',"'"]):
            raise ParserException('Strings must start with \' or "!')
        
        quote = self.c  # save what kind of quote it was.
        self.read()     # consume the quote.
        
        ret = ''
        
        while not (self.c in ['',quote]):
            # If there is no escape character,
            # it's pretty simple.
            if self.c != '\\':
                ret += self.c
                self.read()
            # If there was an escape character,
            # we want to handle the escape.
            else:
                self.read() # consume '\\'
                if      self.c == '\\':
                    ret += '\\'
                elif    self.c == 'n':
                    ret += '\n'
                elif    self.c == 't':
                    ret += '\t'
                elif    self.c == '"':
                    ret += '"'
                elif    self.c == '\'':
                    ret += '\''
                self.read() # consume the indicator
                
        if self.c != quote:
            raise ParserException('Quote never closed')
            
        self.read()     # consume closing quote
        return ret
    
    def parseList(self):
        if not (self.c == '('):
            raise ParserException('Lists must start with a "("!')
        
        self.read()     # consume '('
        
        ret = []
        
        self.clearWhitespace()
        while not (self.c in ['',')']):
            ret.append(self.parse())
            self.clearWhitespace()
            
        if self.c != ')':
            raise ParserException('Lists need to be closed with a ")"!')
        self.read()     # consume ')'
        return ret
        
    def parseAtom(self):
        if self.c in ['(','"',"'"]:
            raise ParserException('Atoms cannot start with (, ", or \'')
        ret = ''
        while not ((self.c in ['','(', ')','"',"'"]) or self.c.isspace()):
            ret += self.c
            self.read()
        return ret
        
def main():
    import sys
    p = Parser(sys.stdin)
    print p.parseAll()

if __name__ == '__main__':
    main()
