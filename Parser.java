package com.pawcode;
import java.io.*;
import java.util.*;

// The Ast of the program is represented as nested LinkedList.
public class Parser {
    public Parser(String s){
        this(new StringReader(s));
    }
    public Parser(Reader r){
        super();
        this.r = r;
        c = read(); // load the first char into the buffer
    }
    
    private Reader r;
    private int c;      // most recent char token.
    
    // Token is just a marker class. Unfortunately java.lang.String is final so I can't simply subclass it.
    public static class Token {
        public Token(String s){
            super();
            this.s = s;
        }
        public String s;
        
        @Override
        public String toString(){
            return "Token["+s+"]";
        }
    };
    
    public Map<Object,Token> parseProgram(){
        Map<Object,Token> ret = new HashMap<Object,Token>();
        while (c != -1){
            // eat whitespace
            parseWhitespace();
            
            // parse eventType string
            Token eventType = parseToken();
            
            // eat whitespace
            parseWhitespace();
            
            // add parsed Ast to eventType
            ret.put(parseAst(),eventType);
        }
        return ret;
    }
    
    // parse can return:
    //      LinkedList (for s-expression) --> starts with '(' and ends with ')'
    //      Number                        --> starts with 0-9 or .
    //      String                        --> anything else, separated by whitespace
    public Object parseAst(){
        Object ret = null;
        
        // skip whitespace
        parseWhitespace();
        
        // end of stream
        if (c == -1){
            ret = null;
        }
        // LinkedList
        else if (c == '('){
            ret = parseLinkedList();
        }
        // Number
        else if ( c == '.' || Character.isDigit(c)) {
            ret = parseNumber();
        }
        // quoted string
        else if (c == '"' || c == '\''){
            ret = quotedString();
        }
        // Token
        else {
            ret = parseToken();
        }
        
        return ret;
    }
    
    private void parseWhitespace(){
        while (Character.isWhitespace(c) && c != -1){
            read();
        }
    }
    
    private LinkedList<Object> parseLinkedList(){
        read(); // consume '('
        
        LinkedList<Object> ret = new LinkedList<Object>();
        while (c != ')' && c != -1){
            ret.add(parseAst());
            
            // consume any whitespace left after every ast
            parseWhitespace();
        }
        
        read(); // consume ')'
        return ret;
    }
    
    private String quotedString(){
        String ret = "";
        int quoteType = c;
        read(); // consume '"' or "'"
        while (c != quoteType){
            if (c == '\\'){
                read(); // consume '\'
                int toAdd = c;
                switch(c){
                case '\\':
                case '"':
                case '\'':
                    toAdd = c;
                    break;
                case 'n':
                    toAdd = '\n';
                    break;
                case 't':
                    toAdd = '\t';
                    break;
                }
                ret += (char) toAdd;
                read(); // consume the type character
            }
            else {
                ret += (char) c;
                read(); // consume currently read character.
            }
        }
        read(); // consume '"' or "'"
        return ret;
    }
    private Token parseToken(){
        String ret = "";
        while (!Character.isWhitespace(c) && c != '(' && c != ')' && c != -1){
            ret += (char) c;
            read();
        }
        return new Token(ret);
    }
    
    private Number parseNumber(){
        Number ret = null;
        String numStr = "";
        boolean isInteger = true;
        
        while ( (c == '.' || Character.isDigit(c)) && c != -1) {
            if (c == '.') isInteger = false;
            numStr += (char) c;
            read();
        }
        
        if (isInteger){
            ret = Integer.valueOf(numStr);
        }
        else {
            ret = Double.valueOf(numStr);
        }
        return ret;
    }
    
    // helper methods.
    private int read(){
        int ret = 0;
        try {
            if (r.ready()){
                ret = r.read();
            }
            else {
                ret = -1;
            }
        } catch (IOException e){
            e.printStackTrace();
        }
        return c = ret;
    }
};