import java.util.Vector;
import java.io.*;

public class SexpParser {
    // Atom class distinguishes an atom
    // from a literal string. Since String
    // is final, I had to write a wrapper
    // class instead of subclass
    public final class Atom {
        public Atom(String s){
            name = s;
        }
        private final String name;
        
        @Override
        public String toString(){
            return name;
        }
    }
    
    
    private Reader r;
    private int c;
    
    public SexpParser(Reader r){
        this.r = r;
        
        // while c == -1, read() won't work --
        // so the parse method initializes it when
        // its ready
        c = -1;
    }
    
    // The returned Vector will contain either Strings or 
    // Other Vector that contains Vectors and/or Strings
    // and so on for all those nested Vectors
    
    // the public parse method needs to initialize things.
    public Vector parse(){
        
        // make sure c != -1, because that signals end of stream
        c = 0; 
        
        // initialize c
        read();
        
        return pparse();
    }
    
    
    
    
    
    
    // helper methods
    
    // parse without initializing
    // (stands for private parse)
    private Vector pparse(){
        Vector<Object> ret = new Vector<Object>();
        clearWhitespace();
        while (c != -1){
            ret.add(parseSingle());
            clearWhitespace();
        }
        return ret;
    }
    
    // parse a single entity, whatever it
    // may be.
    private Object parseSingle() {
        Object ret = null;
        
        // c == -1 --> done
        if (c == -1){
            
        }
        else if (c == '('){
            ret = parseList();
        }
        else if (c == '"' || c == '\''){
            ret = parseString();
        }
        else {
            ret = parseAtom();
        }
        return ret;
    }
    
    private void clearWhitespace(){
        while (c != -1 && Character.isWhitespace((char) c)) read();
    }
    
    private Vector parseList(){
        if (c != '('){
            throw new Error("Lists must start with '('");
        }
        read(); // consume '('
        
        Vector<Object> ret = new Vector<Object>();
        
        clearWhitespace();
        while (c != -1 && c != ')'){
            ret.add(parseSingle());
            clearWhitespace();
        }
        
        if (c != ')'){
            throw new Error("Could not found matching ')'");
        }
        read(); // consume ')'
        
        return ret;
    }
    
    private String parseString(){
        if (! (c == '"' || c == '\'') ){
            throw new Error("Strings must start with a ' or \"");
        }
        String ret = "";
        int quote = c;
        
        while (c != -1 && c != quote){
            if (c != '\\'){
                ret += (char) c;
                read();
            }
            else {
                read(); // consume '\\'
                switch (c){
                case '\\': ret += '\\'; break;
                case 'n' : ret += '\n'; break;
                case 't' : ret += '\t'; break;
                case '"' : ret += '\"'; break;
                case '\'': ret += '\''; break;
                }
                read(); // consume the marker character
            }
        }
        
        if (c != quote){
            throw new Error("String quote never closed!");
        }
        
        // consume closign quote
        read();
        
        return ret;
    }
    
    private Atom parseAtom(){
        if (c == '"' || c == '\'' || c == '(' || c == ')'){
            throw new Error("Atoms cannot start with \", \', (, or )");
        }
        
        String ret = "";
        while (c != -1 && c != '"' && c != '\'' && c != '(' && c != ')'){
            ret += (char) c;
            read();
        }
        
        return new Atom(ret);
    }
    
    
    // From the answers on this page:
    // http://stackoverflow.com/questions/11323657/what-might-cause-an-ioexception-in-java-when-i-make-a-call-to-reader-read
    // It looks as though the IOException thrown by read
    // is highly dependent on the kind of Reader that 
    // the user of this parser has given me. So on some
    // level, it might be good design to just report that
    // my read() throws an IOException too. However, I am kind
    // of a lazy person, and I don't like having to explicitly
    // catch exceptions all the time. So I'm just going to
    // catch it, and throw an error instead
    private void read(){
        try {
            if (c != -1)   c = r.read();
        } catch (IOException e){
            throw new IOError(e);
        }
    }
    
    
    public static void main(String[] args){
        SexpParser sp = new SexpParser(new InputStreamReader(System.in));
        System.out.println(sp.parse());
    }
}