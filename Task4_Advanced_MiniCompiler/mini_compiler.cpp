/*
 * ============================================================
 *  DevAlpha Technologies — C++ Programming Internship
 *  Task 4 (Advanced): Advanced C++ Application
 *  Project     : Mini Compiler / Expression Calculator
 *  Concepts    : Tokenizer (Lexer), Parser, Expression Trees,
 *                Recursion, Stack, Templates, STL, OOP,
 *                Operator Precedence, Error Handling
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
using namespace std;

// ─────────────────────────────────────────────
//  ENUM: Token Types  (like a real compiler)
// ─────────────────────────────────────────────
enum TokenType {
    NUMBER, IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER,
    LPAREN, RPAREN,
    ASSIGN,
    END_OF_EXPR
};

// ─────────────────────────────────────────────
//  STRUCT: Token
// ─────────────────────────────────────────────
struct Token {
    TokenType type;
    double    numValue;
    string    strValue;

    Token(TokenType t, double v = 0, string s = "")
        : type(t), numValue(v), strValue(s) {}
};

// ─────────────────────────────────────────────
//  CLASS: Lexer (Tokenizer)
//  Breaks expression string into tokens
// ─────────────────────────────────────────────
class Lexer {
private:
    string input;
    int    pos;

    char current() { return (pos < (int)input.size()) ? input[pos] : '\0'; }
    void advance()  { pos++; }

    Token readNumber() {
        string num = "";
        while (isdigit(current()) || current() == '.') {
            num += current(); advance();
        }
        return Token(NUMBER, stod(num));
    }

    Token readIdentifier() {
        string id = "";
        while (isalpha(current()) || isdigit(current()) || current() == '_') {
            id += current(); advance();
        }
        return Token(IDENTIFIER, 0, id);
    }

public:
    Lexer(string expr) : input(expr), pos(0) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (pos < (int)input.size()) {
            char c = current();
            if (isspace(c))          { advance(); continue; }
            else if (isdigit(c) || (c == '.' && isdigit(input[pos+1])))
                                      tokens.push_back(readNumber());
            else if (isalpha(c))      tokens.push_back(readIdentifier());
            else if (c == '+')       { tokens.push_back(Token(PLUS));     advance(); }
            else if (c == '-')       { tokens.push_back(Token(MINUS));    advance(); }
            else if (c == '*')       {
                advance();
                if (current() == '*') { tokens.push_back(Token(POWER));   advance(); }
                else                    tokens.push_back(Token(MULTIPLY));
            }
            else if (c == '/')       { tokens.push_back(Token(DIVIDE));   advance(); }
            else if (c == '%')       { tokens.push_back(Token(MODULO));   advance(); }
            else if (c == '^')       { tokens.push_back(Token(POWER));    advance(); }
            else if (c == '(')       { tokens.push_back(Token(LPAREN));   advance(); }
            else if (c == ')')       { tokens.push_back(Token(RPAREN));   advance(); }
            else if (c == '=')       { tokens.push_back(Token(ASSIGN));   advance(); }
            else throw runtime_error(string("Unknown character: ") + c);
        }
        tokens.push_back(Token(END_OF_EXPR));
        return tokens;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Parser + Evaluator
//  Recursive Descent Parser with operator
//  precedence (like real compilers)
//
//  Grammar:
//    expr       -> assignment
//    assignment -> IDENTIFIER = expr | addexpr
//    addexpr    -> mulexpr ((+|-) mulexpr)*
//    mulexpr    -> powexpr ((*|/|%) powexpr)*
//    powexpr    -> unary (^ unary)*
//    unary      -> -unary | primary
//    primary    -> NUMBER | IDENTIFIER | func(expr) | (expr)
// ─────────────────────────────────────────────
class Parser {
private:
    vector<Token>       tokens;
    int                 pos;
    map<string, double>& variables;

    Token& current() { return tokens[pos]; }
    Token  consume()  { return tokens[pos++]; }

    bool check(TokenType t) { return current().type == t; }

    // Built-in math functions
    double callFunction(string name, double arg) {
        if (name == "sin")   return sin(arg * M_PI / 180.0); // degrees
        if (name == "cos")   return cos(arg * M_PI / 180.0);
        if (name == "tan")   return tan(arg * M_PI / 180.0);
        if (name == "sqrt")  return sqrt(arg);
        if (name == "abs")   return fabs(arg);
        if (name == "log")   return log10(arg);
        if (name == "ln")    return log(arg);
        if (name == "ceil")  return ceil(arg);
        if (name == "floor") return floor(arg);
        throw runtime_error("Unknown function: " + name);
    }

    // primary -> NUMBER | IDENTIFIER | func(expr) | (expr)
    double parsePrimary() {
        if (check(NUMBER)) {
            double val = current().numValue;
            consume();
            return val;
        }
        if (check(IDENTIFIER)) {
            string name = current().strValue;
            consume();
            // Built-in constants
            if (name == "pi" || name == "PI") return M_PI;
            if (name == "e"  || name == "E")  return M_E;
            // Function call
            if (check(LPAREN)) {
                consume(); // (
                double arg = parseExpr();
                if (!check(RPAREN))
                    throw runtime_error("Expected ')' after function argument.");
                consume(); // )
                return callFunction(name, arg);
            }
            // Variable
            if (variables.count(name))
                return variables[name];
            throw runtime_error("Undefined variable: " + name);
        }
        if (check(LPAREN)) {
            consume(); // (
            double val = parseExpr();
            if (!check(RPAREN))
                throw runtime_error("Mismatched parentheses.");
            consume(); // )
            return val;
        }
        throw runtime_error("Unexpected token in expression.");
    }

    // unary -> -unary | primary
    double parseUnary() {
        if (check(MINUS)) {
            consume();
            return -parseUnary();
        }
        if (check(PLUS)) {
            consume();
            return parseUnary();
        }
        return parsePrimary();
    }

    // powexpr -> unary (^ unary)*
    double parsePower() {
        double base = parseUnary();
        while (check(POWER)) {
            consume();
            double exp = parseUnary();
            base = pow(base, exp);
        }
        return base;
    }

    // mulexpr -> powexpr ((*|/|%) powexpr)*
    double parseMul() {
        double left = parsePower();
        while (check(MULTIPLY) || check(DIVIDE) || check(MODULO)) {
            TokenType op = current().type;
            consume();
            double right = parsePower();
            if (op == MULTIPLY) left *= right;
            else if (op == DIVIDE) {
                if (right == 0) throw runtime_error("Division by zero.");
                left /= right;
            }
            else left = fmod(left, right);
        }
        return left;
    }

    // addexpr -> mulexpr ((+|-) mulexpr)*
    double parseAdd() {
        double left = parseMul();
        while (check(PLUS) || check(MINUS)) {
            TokenType op = current().type;
            consume();
            double right = parseMul();
            if (op == PLUS)  left += right;
            else              left -= right;
        }
        return left;
    }

    // assignment -> IDENTIFIER = expr | addexpr
    double parseAssignment() {
        // Lookahead: IDENTIFIER followed by ASSIGN?
        if (check(IDENTIFIER) && pos + 1 < (int)tokens.size()
            && tokens[pos + 1].type == ASSIGN) {
            string varName = current().strValue;
            consume(); // IDENTIFIER
            consume(); // =
            double val = parseExpr();
            variables[varName] = val;
            cout << "  [OK] " << varName << " = " << val << endl;
            return val;
        }
        return parseAdd();
    }

    double parseExpr() { return parseAssignment(); }

public:
    Parser(vector<Token> toks, map<string, double>& vars)
        : tokens(toks), pos(0), variables(vars) {}

    double evaluate() {
        double result = parseExpr();
        if (!check(END_OF_EXPR))
            throw runtime_error("Unexpected token after expression.");
        return result;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Calculator (Main Controller)
// ─────────────────────────────────────────────
class Calculator {
private:
    map<string, double> variables;
    vector<string>      history;

    string trim(string s) {
        int l = 0, r = s.size() - 1;
        while (l <= r && isspace(s[l])) l++;
        while (r >= l && isspace(s[r])) r--;
        return s.substr(l, r - l + 1);
    }

public:
    Calculator() {
        // Preload constants as variables
        variables["pi"] = M_PI;
        variables["e"]  = M_E;
        variables["ans"] = 0;
    }

    double evaluate(string expr) {
        expr = trim(expr);
        if (expr.empty()) throw runtime_error("Empty expression.");

        Lexer lexer(expr);
        vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens, variables);
        double result = parser.evaluate();

        variables["ans"] = result;           // store last answer
        history.push_back(expr + " = " + to_string(result));
        return result;
    }

    void showHistory() {
        if (history.empty()) { cout << "\n  No history yet." << endl; return; }
        cout << "\n  ===== CALCULATION HISTORY =====" << endl;
        for (int i = 0; i < (int)history.size(); i++)
            cout << "  " << setw(3) << (i+1) << ". " << history[i] << endl;
        cout << "  ===============================" << endl;
    }

    void showVariables() {
        cout << "\n  ===== STORED VARIABLES =====" << endl;
        for (auto& v : variables)
            cout << "  " << left << setw(10) << v.first
                 << " = " << v.second << endl;
        cout << "  ===========================" << endl;
    }

    void clearHistory() { history.clear(); cout << "\n  [OK] History cleared." << endl; }
    void clearVars()    {
        variables.clear();
        variables["pi"] = M_PI;
        variables["e"]  = M_E;
        variables["ans"] = 0;
        cout << "\n  [OK] Variables cleared." << endl;
    }

    void showHelp() {
        cout << "\n  ===== HELP / SUPPORTED OPERATIONS =====" << endl;
        cout << "  Operators  : + - * / % ^ **"               << endl;
        cout << "  Functions  : sin(x) cos(x) tan(x)"         << endl;
        cout << "             : sqrt(x) abs(x) log(x) ln(x)"  << endl;
        cout << "             : ceil(x) floor(x)"              << endl;
        cout << "  Constants  : pi, e"                         << endl;
        cout << "  Variables  : x = 5  (assign and use)"       << endl;
        cout << "  Last Ans   : ans"                           << endl;
        cout << "  Examples   :"                               << endl;
        cout << "    2 + 3 * 4        => 14"                   << endl;
        cout << "    (2 + 3) * 4      => 20"                   << endl;
        cout << "    2 ^ 10           => 1024"                  << endl;
        cout << "    sqrt(144)        => 12"                    << endl;
        cout << "    sin(90)          => 1  (degrees)"         << endl;
        cout << "    x = 10           => stores x"             << endl;
        cout << "    x * 2 + 5        => uses x"               << endl;
        cout << "    ans + 1          => uses last result"      << endl;
        cout << "  =======================================" << endl;
    }
};

// ─────────────────────────────────────────────
//  FUNCTION: Display result nicely
// ─────────────────────────────────────────────
void printResult(double result) {
    cout << "  >> ";
    // Show as integer if whole number
    if (result == (long long)result && fabs(result) < 1e15)
        cout << fixed << setprecision(0) << result << endl;
    else
        cout << fixed << setprecision(6) << result << endl;
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {
    Calculator calc;

    cout << "\n  +==========================================+" << endl;
    cout << "  |   Mini Compiler / Expression Calculator  |" << endl;
    cout << "  |   DevAlpha Technologies Internship       |" << endl;
    cout << "  +==========================================+" << endl;
    cout << "  Type 'help' for usage, 'exit' to quit." << endl;
    cout << "  Commands: history | vars | clear | clvars" << endl;

    string input;
    while (true) {
        cout << "\n  calc> ";
        getline(cin, input);

        // Trim
        string cmd = input;
        transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        while (!cmd.empty() && isspace(cmd.front())) cmd.erase(cmd.begin());
        while (!cmd.empty() && isspace(cmd.back()))  cmd.pop_back();

        if (cmd == "exit" || cmd == "quit") {
            cout << "\n  Goodbye! Thank you for using Mini Compiler.\n" << endl;
            break;
        }
        else if (cmd == "help")    { calc.showHelp();      continue; }
        else if (cmd == "history") { calc.showHistory();   continue; }
        else if (cmd == "vars")    { calc.showVariables(); continue; }
        else if (cmd == "clear")   { calc.clearHistory();  continue; }
        else if (cmd == "clvars")  { calc.clearVars();     continue; }
        else if (cmd.empty())       continue;

        try {
            double result = calc.evaluate(input);
            printResult(result);
        } catch (exception& e) {
            cout << "  [ERR] " << e.what() << endl;
        }
    }
    return 0;
}
