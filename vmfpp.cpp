#include "vmfpp.hpp"
#include <stack>
#include <sstream>

struct LexSymbol {
    enum Type {
        S_EOF = 0,
        S_CLASS,
        S_STRING,
        S_LBRACKET,
        S_RBRACKET,
    } type;

    std::string value;
};

static size_t lexBracket(std::string_view input, std::vector<LexSymbol> *arr) {
    LexSymbol s;
    s.type = input[0] == '{' ? LexSymbol::S_LBRACKET : LexSymbol::S_RBRACKET;
    s.value = "";
    arr->push_back(s);

    return 1;
}

static size_t lexString(std::string_view input, std::vector<LexSymbol> *arr) {
    size_t i = 1;

    while(input[i] != '\0' && input[i] != '\"')
        i++;

    LexSymbol sym;
    sym.type = LexSymbol::S_STRING;
    sym.value = input.substr(1, i-1);
    arr->push_back(sym);

    return input[i] == '"' ? i + 1: i;
}

static size_t lexClass(std::string_view input, std::vector<LexSymbol> *arr) {
    size_t i = 0;

    while(input[i] != '\0' && std::isalnum(input[i]))
        i++;

    LexSymbol sym;
    sym.type = LexSymbol::S_CLASS;
    sym.value = input.substr(0, i);;
    arr->push_back(sym);

    return input[i] == '\"' ? i + 1 : i;
}

static size_t skipComment(std::string_view input) {
    size_t i = 0;

    while(input[i] != '\0' && input[i] != '\n')
        i++;

    return i + 1;
}

static void lex(std::string_view input, std::vector<LexSymbol> *arr) {
    for(std::size_t i = 0; i < input.length();) {

        if(input[i] == '/' && input[i+1] == '/')
            i += skipComment(input.substr(i));

        else if(input[i] == '{' || input[i] == '}')
            i += lexBracket(input.substr(i), arr);

        else if(input[i] == '"')
            i += lexString(input.substr(i), arr);

        else if(isalpha(input[i]))
            i += lexClass(input.substr(i), arr);

        else
            i++;
    }

    LexSymbol eof;
    eof.type = LexSymbol::S_EOF;
    eof.value = "";
    arr->push_back(eof);
}

static long long parse_sym(VMF *root, std::vector<LexSymbol>& arr) {
    std::stack<VMF*> parse_stack;
    parse_stack.push(root);

    for(std::size_t i = 0; i < arr.size();) {
        VMF child;

        switch(arr[i].type) {
            case LexSymbol::S_STRING: 
                if(arr[i+1].type != LexSymbol::S_STRING)
                    return -7;

                parse_stack.top()->properties.insert({arr[i].value, arr[i+1].value});
                i += 2;
                break;

            case LexSymbol::S_CLASS: 
                if(arr[i+1].type != LexSymbol::S_LBRACKET)
                    return -6;

                child.name = arr[i].value;
                parse_stack.top()->children.push_back(child);
                parse_stack.push(&parse_stack.top()->children.back());

                // Temp
                i += 2;
                break;

            case LexSymbol::S_LBRACKET: 
                return -2;
                break;

            case LexSymbol::S_RBRACKET:
                parse_stack.pop();
                if(parse_stack.size() <= 0)
                    return -3;

                i++;
                break;

            case LexSymbol::S_EOF: 
                if(parse_stack.size() != 1)
                    return -8;

                return i;
                break;

            default:
                return -4;
        }
    }

    return -1;
}

std::string VMF::serialize() {
    std::stringstream rstr;

    for(const auto& [key, value] : this->properties) {
        rstr << "\"" << key << "\" \"" << value << "\"" << std::endl;
    }

    for(auto& val : this->children) {
        rstr << val.name << " {" << std::endl
             << val.serialize()
             << "}" << std::endl;
    }

    return rstr.str();
}

int VMF::parse(std::string_view input) {
    this->name = "";
    std::vector<LexSymbol> symbols;

    lex(input, &symbols);
    long long result = parse_sym(this, symbols);

    if(result < 0) {
        this->children.clear();
        this->properties.clear();
        return -1;
    }

    return 0;
}
