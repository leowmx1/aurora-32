#include "bootstrap.h"
/* This exact file is the self-host input. bootstrap.h supplies ONLY the LLVM
 * seed's OS bindings. The native compiler provides equivalent builtins.
 * int, char and pointers occupy one word. Private function ABI: stack args,
 * saved r15 frame base, result r0. Public syscall ABI remains r0..r3. */
/* Workspaces are written before use. Do not clear 220K words before the UI.
 * name_symbol initializes every live symbol; emit/read/next fill their prefixes. */
int source[65537] __attribute__((section(".noinit")));
int code[131072] __attribute__((section(".noinit")));
int names[20480] __attribute__((section(".noinit")));
int kinds[640] __attribute__((section(".noinit")));
int types[640] __attribute__((section(".noinit")));
int addresses[640] __attribute__((section(".noinit")));
int arities[640] __attribute__((section(".noinit")));
int defined[640] __attribute__((section(".noinit")));
int fixups[640] __attribute__((section(".noinit")));
int token_text[256] __attribute__((section(".noinit")));
int token;
int number;
int token_length;
int cursor;
int source_size;
int line_number;
int token_line;
int count;
int globals;
int locals;
int data_words;
int frame_words;
int expression_depth;
int statement_depth;
int loop_depth;
int loop_start[96] __attribute__((section(".noinit")));
int loop_break[96] __attribute__((section(".noinit")));
int value_kind;
int value_type;
int div_calls;
int key_calls;
int fail_jumps;
int *problem;
int problem_line;
int source_path[128] __attribute__((section(".noinit")));
int output_path[128] __attribute__((section(".noinit")));
int background_answer[128] __attribute__((section(".noinit")));
int input_swallow_lf;
int file_info[37] __attribute__((section(".noinit")));
int file_request[4] __attribute__((section(".noinit")));

int error(int *s) {
    if (!problem) { problem = s; problem_line = token_line; }
    return 0;
}
int same(int *a, int *b) {
    int i = 0;
    while (a[i] && a[i] == b[i]) i = i + 1;
    return a[i] == b[i];
}
int letter(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
int digit(int c) { return c >= '0' && c <= '9'; }
int take() {
    int c = source[cursor];
    if (cursor < source_size) cursor = cursor + 1;
    if (c == '\n') line_number = line_number + 1;
    return c;
}
int escape() {
    int c = take();
    if (c == 'n') return 10;
    if (c == 'r') return 13;
    if (c == 't') return 9;
    if (c == 'b') return 8;
    if (c == '0') return 0;
    if (c == '\\' || c == '\'' || c == '"') return c;
    return error("Unsupported escape.");
}
int next() {
    token = 256; token_length = 0;
    if (problem) return 0;
    int c = 0; int i = 0;
    while (1) {
        c = source[cursor];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { take(); continue; }
        if (c == '/' && source[cursor + 1] == '/') {
            while (cursor < source_size && source[cursor] != '\n') take();
            continue;
        }
        if (c == '/' && source[cursor + 1] == '*') {
            take(); take();
            while (cursor < source_size && !(source[cursor] == '*' && source[cursor + 1] == '/')) take();
            if (cursor == source_size) return error("Unclosed comment.");
            take(); take(); continue;
        }
        if (c == '#') {
            /* A seed bridge, not a general preprocessor. */
            i = 0;
            while (cursor < source_size && source[cursor] != '\n' && source[cursor] != '\r' && i < 255) {
                token_text[i] = take(); i = i + 1;
            }
            token_text[i] = 0;
            if (!same(token_text, "#include \"bootstrap.h\"")) return error("Unsupported preprocessor directive.");
            continue;
        }
        break;
    }
    token_line = line_number;
    if (cursor == source_size) return 0;
    c = take();
    if (letter(c)) {
        token_text[0] = c; token_length = 1;
        while (letter(source[cursor]) || digit(source[cursor])) {
            if (token_length == 31) return error("Identifier exceeds 31 characters.");
            token_text[token_length] = take(); token_length = token_length + 1;
        }
        token_text[token_length] = 0; token = 257;
        if (same(token_text, "int") || same(token_text, "char")) token = 266;
        if (same(token_text, "if")) token = 267;
        if (same(token_text, "else")) token = 268;
        if (same(token_text, "while")) token = 269;
        if (same(token_text, "return")) token = 270;
        if (same(token_text, "break")) token = 271;
        if (same(token_text, "continue")) token = 272;
        return 0;
    }
    if (digit(c)) {
        number = c - '0'; int base = 10;
        if (c == '0' && (source[cursor] == 'x' || source[cursor] == 'X')) { take(); base = 16; }
        int digits = 0;
        while (1) {
            c = source[cursor]; int d = -1;
            if (digit(c)) d = c - '0';
            if (base == 16 && c >= 'a' && c <= 'f') d = c - 'a' + 10;
            if (base == 16 && c >= 'A' && c <= 'F') d = c - 'A' + 10;
            if (d < 0 || d >= base) break;
            if (base == 10 && (number < 0 || number > 429496729 || (number == 429496729 && d > 5))) return error("Integer exceeds 32 bits.");
            if (base == 16 && (number < 0 || number > 268435455)) return error("Integer exceeds 32 bits.");
            number = number * base + d; take(); digits = digits + 1;
        }
        if (base == 16 && !digits) return error("Expected hex digits.");
        token = 258; return 0;
    }
    if (c == '"' || c == '\'') {
        int quote = c;
        while (cursor < source_size && source[cursor] != quote) {
            c = take();
            if (c == 10 || c == 13 || !c) return error("Unclosed literal.");
            if (c == '\\') c = escape();
            if (problem) return 0;
            if (token_length == 255) return error("String exceeds 255 characters.");
            token_text[token_length] = c; token_length = token_length + 1;
        }
        if (cursor == source_size) return error("Unclosed literal.");
        take(); token_text[token_length] = 0; token = 259;
        if (quote == '\'') {
            if (token_length != 1) return error("Character literal needs one character.");
            number = token_text[0]; token = 258;
        }
        return 0;
    }
    token = c;
    if (c == '=' && source[cursor] == '=') { take(); token = 260; }
    else if (c == '!' && source[cursor] == '=') { take(); token = 261; }
    else if (c == '<' && source[cursor] == '=') { take(); token = 262; }
    else if (c == '>' && source[cursor] == '=') { take(); token = 263; }
    else if (c == '&' && source[cursor] == '&') { take(); token = 264; }
    else if (c == '|' && source[cursor] == '|') { take(); token = 265; }
    else if (!(c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '(' || c == ')' ||
               c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '=' ||
               c == '!' || c == '<' || c == '>' || c == '&' || c == '|' || c == '^' || c == '~')) error("Unsupported character or operator.");
    return 0;
}
int expect(int t) { if (token != t) return error("Unexpected token."); return next(); }
int emit(int word) {
    if (problem) return 0;
    if (count >= 131072) return error("Generated code exceeds 131072 words.");
    code[count] = word; count = count + 1; return count - 1;
}
int op(int opcode, int a, int b, int d) { return emit(opcode + a * 65536 + b * 4096 + d * 256); }
int ldi(int reg, int value) { emit(0x15 + reg * 256); return emit(value); }
int load(int reg, int address) { emit(0x13 + reg * 256); return emit(address); }
int store(int reg, int address) { emit(0x14 + reg * 65536); return emit(address); }
int jump(int condition) { if (condition) emit(0x31 + condition * 1048576); else emit(0x30); return emit(0); }
int patch(int at, int target) { if (!problem) code[at] = 0x80000000 + target; return 0; }
int chain(int at, int *head) { if (!problem) { code[at] = *head; *head = at; } return 0; }
int link(int head, int target) {
    while (head && !problem) { int tail = code[head]; patch(head, target); head = tail; }
    return 0;
}
int call_chain(int *head) { emit(0x34); return chain(emit(0), head); }
int syscall(int id) { ldi(0, id); emit(0x37); return emit(2); }
int zero_test() { ldi(1, 0); return op(0x2f, 0, 1, 0); }
int boolean(int condition) {
    int yes = jump(condition); ldi(0, 0); int end = jump(0);
    patch(yes, count); ldi(0, 1); return patch(end, count);
}
int leave() { op(0x42, 15, 0, 0); op(0x41, 0, 0, 15); return emit(0x36); }
int drop_args(int n) {
    if (n) { op(0x43, 0, 0, 1); ldi(2, n); op(0x21, 1, 2, 1); op(0x42, 1, 0, 0); }
    return 0;
}
int string_literal() {
    int ptr = ldi(0, 0); int after = jump(0); patch(ptr, count); int i = 0;
    while (i < token_length) { emit(token_text[i]); i = i + 1; }
    emit(0); patch(after, count); value_kind = 0; value_type = 1; return next();
}
int rvalue() { if (value_kind == 1) op(0x11, 0, 0, 0); value_kind = 0; return 0; }
int find_range(int lo, int hi) {
    int i = hi - 1;
    while (i >= lo) { if (same(names + i * 32, token_text)) return i; i = i - 1; }
    return 0;
}
int lookup() { int id = find_range(512, locals); if (!id) id = find_range(1, globals); return id; }
int name_symbol(int id) {
    int i = 0;
    while (i <= token_length) { names[id * 32 + i] = token_text[i]; i = i + 1; }
    kinds[id] = 0; types[id] = 0; addresses[id] = 0; arities[id] = 0; defined[id] = 0; fixups[id] = 0;
    return id;
}
int new_global() {
    if (globals == 512) { error("Too many global symbols."); return 1; }
    int id = globals; globals = globals + 1; return name_symbol(id);
}
int new_local() {
    if (locals == 640) { error("Too many local symbols (maximum 128)."); return 512; }
    if (find_range(512, locals)) { error("Duplicate local variable."); return 512; }
    int id = locals; locals = locals + 1; return name_symbol(id);
}
int pointer_type() {
    int depth = 0;
    while (token == '*') { depth = depth + 1; next(); }
    if (depth > 8) error("Pointer nesting exceeds 8."); return depth;
}
int symbol_address(int id) {
    if (kinds[id] == 1 || kinds[id] == 2) ldi(0, addresses[id]);
    else { ldi(0, addresses[id]); op(0x20, 15, 0, 0); }
    value_kind = 1; value_type = types[id];
    if (kinds[id] == 2 || kinds[id] == 4) { value_kind = 2; value_type = value_type + 1; }
    return 0;
}
int precedence(int t) {
    if (t == '=') return 1;
    if (t == 265) return 2;
    if (t == 264) return 3;
    if (t == '|') return 4;
    if (t == '^') return 5;
    if (t == '&') return 6;
    if (t == 260 || t == 261) return 7;
    if (t == '<' || t == '>' || t == 262 || t == 263) return 8;
    if (t == '+' || t == '-') return 9;
    if (t == '*' || t == '/' || t == '%') return 10;
    return 0;
}
int expression(int minimum);
int atom();
int function_call(int id) {
    next(); int n = 0; int argument_type = 0;
    if (token != ')') {
        while (!problem) {
            expression(1); rvalue(); argument_type = value_type; op(0x40, 0, 0, 0);
            n = n + 1;
            if (n > 8) { error("At most 8 arguments."); break; }
            if (token != ',') break;
            next();
        }
    }
    expect(')');
    if (arities[id] != n) return error("Wrong number of arguments.");
    if (kinds[id] == 6) {
        int builtin = addresses[id];
        if (builtin == -1) call_chain(&key_calls);
        else {
            int i = n;
            while (i > 0) { op(0x41, 0, 0, i); i = i - 1; }
            if (builtin == -2) {
                op(0x10, 1, 0, 0); op(0x10, 2, 0, 1); op(0x10, 3, 0, 2); op(0x10, 4, 0, 3);
                emit(0x37); emit(2);
            } else {
                ldi(2, 0); ldi(3, 0);
                if (builtin == -3) {
                    if (argument_type) syscall(6); else syscall(5);
                    ldi(1, 10); syscall(1);
                } else syscall(builtin);
            }
        }
    } else {
        emit(0x34);
        if (defined[id]) emit(0x80000000 + addresses[id]); else chain(emit(0), &fixups[id]);
        drop_args(n);
    }
    value_kind = 0; value_type = types[id]; return 0;
}
int atom() {
    value_kind = 0; value_type = 0;
    if (token == 258) { ldi(0, number); next(); }
    else if (token == 259) string_literal();
    else if (token == '(') {
        next();
        if (token == 266) {
            next(); int cast_type = pointer_type(); expect(')'); expression(11); rvalue(); value_type = cast_type;
        } else { expression(1); expect(')'); }
    } else if (token == '+' || token == '-' || token == '!' || token == '~' || token == '*' || token == '&') {
        int unary = token; next(); expression(11);
        if (unary == '&') {
            if (value_kind != 1) return error("Address-of needs a scalar lvalue.");
            value_kind = 0; value_type = value_type + 1;
        } else {
            rvalue();
            if (unary == '*') {
                if (!value_type) return error("Dereference needs a pointer.");
                value_type = value_type - 1; value_kind = 1;
            } else {
                if (unary == '-') { ldi(1, 0); op(0x21, 1, 0, 0); }
                if (unary == '~') { ldi(1, -1); op(0x2c, 0, 1, 0); }
                if (unary == '!') { zero_test(); boolean(2); }
                value_type = 0;
            }
        }
    } else if (token == 257) {
        int id = lookup();
        if (!id) return error("Unknown variable or function; declare it first.");
        next();
        if (kinds[id] == 5 || kinds[id] == 6) {
            if (token != '(') return error("Function pointers are not supported."); function_call(id);
        } else symbol_address(id);
    } else return error("Expected an integer expression.");
    while (token == '[' && !problem) {
        rvalue();
        if (!value_type) return error("Indexing needs a pointer or array.");
        int element_type = value_type - 1;
        op(0x40, 0, 0, 0); next(); expression(1); rvalue(); expect(']');
        op(0x41, 0, 0, 1); op(0x20, 1, 0, 0); value_kind = 1; value_type = element_type;
    }
    return 0;
}
int expression(int minimum) {
    expression_depth = expression_depth + 1;
    if (expression_depth > 96) { expression_depth = expression_depth - 1; return error("Expression nesting exceeds 96."); }
    atom();
    while (!problem && precedence(token) >= minimum && precedence(token)) {
        int operator = token; int level = precedence(token); int left_type = value_type; next();
        if (operator == '=') {
            if (value_kind != 1) { error("Assignment needs a scalar lvalue."); break; }
            op(0x40, 0, 0, 0); expression(level); rvalue(); op(0x41, 0, 0, 1); op(0x12, 0, 1, 0); value_type = left_type;
        } else {
            rvalue();
            if (operator == 264 || operator == 265) {
                zero_test(); int condition = 2; if (operator == 265) condition = 5;
                int shorted = jump(condition);
                expression(level + 1); rvalue(); zero_test(); boolean(5); int end = jump(0);
                patch(shorted, count); ldi(0, operator == 265); patch(end, count); value_type = 0;
            } else {
                op(0x40, 0, 0, 0); expression(level + 1); rvalue(); op(0x41, 0, 0, 1);
                int right_type = value_type; value_type = 0;
                if (operator == '+') {
                    op(0x20, 1, 0, 0); value_type = left_type; if (!left_type) value_type = right_type;
                } else if (operator == '-') {
                    op(0x21, 1, 0, 0); if (!right_type) value_type = left_type;
                } else if (operator == '*') op(0x24, 1, 0, 0);
                else if (operator == '/' || operator == '%') { ldi(4, operator == '%'); call_chain(&div_calls); }
                else if (operator == '&') op(0x28, 1, 0, 0);
                else if (operator == '|') op(0x2a, 1, 0, 0);
                else if (operator == '^') op(0x2c, 1, 0, 0);
                else {
                    op(0x2f, 1, 0, 0); int cond = 10;
                    if (operator == 260) cond = 2;
                    if (operator == 261) cond = 5;
                    if (operator == '<') cond = 9;
                    if (operator == 262) cond = 12;
                    if (operator == '>') cond = 11;
                    boolean(cond);
                }
            }
        }
        value_kind = 0;
    }
    expression_depth = expression_depth - 1; return 0;
}
int statement();
int array_size() {
    next();
    if (token != 258 || number < 1 || number > 131072) return error("Array size must be a literal in 1..131072.");
    int n = number; next(); expect(']'); return n;
}
int clear_words(int address_reg, int n) {
    ldi(1, n); ldi(2, 0); ldi(3, 1); int again = count;
    op(0x12, 2, address_reg, 0); op(0x20, address_reg, 3, address_reg);
    op(0x21, 1, 3, 1); patch(jump(5), again); return 0;
}
int local_declaration() {
    next(); int type = pointer_type();
    if (token != 257) return error("Expected variable name.");
    int id = new_local(); types[id] = type; kinds[id] = 3; addresses[id] = frame_words;
    next(); int n = 1;
    if (token == '[') { n = array_size(); kinds[id] = 4; }
    if (frame_words + n > 65536) return error("Function frame exceeds 65536 words.");
    frame_words = frame_words + n; symbol_address(id);
    if (kinds[id] == 4) clear_words(0, n);
    else {
        op(0x40, 0, 0, 0);
        if (token == '=') { next(); expression(1); rvalue(); } else ldi(0, 0);
        op(0x41, 0, 0, 1); op(0x12, 0, 1, 0);
    }
    return expect(';');
}
int statement() {
    if (problem) return 0;
    statement_depth = statement_depth + 1;
    if (statement_depth > 96) { statement_depth = statement_depth - 1; return error("Statement nesting exceeds 96."); }
    if (token == '{') {
        int saved_locals = locals;
        next(); while (!problem && token != '}' && token != 256) statement(); expect('}');
        locals = saved_locals;
    }
    else if (token == 266) local_declaration();
    else if (token == 267) {
        next(); expect('('); expression(1); rvalue(); expect(')'); zero_test(); int no = jump(2); statement();
        if (token == 268) { int end = jump(0); patch(no, count); next(); statement(); patch(end, count); }
        else patch(no, count);
    } else if (token == 269) {
        next(); expect('(');
        if (loop_depth == 96) error("Too many nested loops.");
        else {
            int slot = loop_depth; loop_depth = loop_depth + 1; loop_start[slot] = count; loop_break[slot] = 0;
            expression(1); rvalue(); expect(')'); zero_test(); int end = jump(2);
            statement(); patch(jump(0), loop_start[slot]); patch(end, count);
            link(loop_break[slot], count); loop_depth = loop_depth - 1;
        }
    } else if (token == 271 || token == 272) {
        int kind = token; next(); expect(';');
        if (!loop_depth) error("break/continue requires a loop.");
        else if (kind == 272) patch(jump(0), loop_start[loop_depth - 1]);
        else chain(jump(0), &loop_break[loop_depth - 1]);
    } else if (token == 270) { next(); expression(1); rvalue(); expect(';'); leave(); }
    else if (token == ';') next();
    else { expression(1); rvalue(); expect(';'); }
    statement_depth = statement_depth - 1; return 0;
}
int global_declaration() {
    expect(266); int type = pointer_type();
    if (token != 257) return error("Expected global name.");
    int id = lookup();
    if (!id) id = new_global(); else if (kinds[id] != 5) return error("Duplicate global name.");
    next();
    if (token != '(') {
        if (kinds[id]) return error("Duplicate global name.");
        kinds[id] = 1; types[id] = type; int n = 1;
        if (token == '[') { n = array_size(); kinds[id] = 2; }
        if (data_words + n > 1048576) return error("Global data exceeds 1048576 words.");
        addresses[id] = 0x81000400 + data_words; data_words = data_words + n;
        arities[id] = n;
        /* Deliberately narrow extension, identical to the LLVM seed contract.
         * Other attributes/sections and local/function attributes are errors. */
        if (token == 257 && same(token_text, "__attribute__")) {
            if (kinds[id] != 2) return error("noinit requires a global array.");
            next(); expect('('); expect('(');
            if (token != 257 || !same(token_text, "section")) return error("Expected section attribute.");
            next(); expect('(');
            if (token != 259 || !same(token_text, ".noinit")) return error("Only .noinit is supported.");
            next(); expect(')'); expect(')'); expect(')'); defined[id] = 1;
        }
        return expect(';');
    }
    next(); locals = 512; int argc = 0;
    if (token != ')') {
        while (!problem) {
            expect(266); int paramtype = pointer_type();
            if (token != 257) return error("Expected parameter name.");
            int param = new_local(); kinds[param] = 3; types[param] = paramtype; argc = argc + 1;
            if (argc > 8) return error("At most 8 parameters.");
            next(); if (token != ',') break; next();
        }
    }
    expect(')');
    if (kinds[id] && (arities[id] != argc || types[id] != type)) return error("Conflicting function declaration.");
    kinds[id] = 5; arities[id] = argc; types[id] = type;
    if (token == ';') { locals = 512; return next(); }
    if (defined[id]) return error("Duplicate function definition.");
    if (token != '{') return error("Expected function body.");
    /* Report definitions once, before parsing the body. The symbol table keeps
     * the name intact even after parameter tokens have replaced token_text. */
    putstr("  Function: "); putstr(names + id * 32); putchar(10);
    int i = 0;
    while (i < argc) { addresses[512 + i] = i - argc - 2; i = i + 1; }
    defined[id] = 1; addresses[id] = count; link(fixups[id], count); fixups[id] = 0; frame_words = 0;
    op(0x40, 15, 0, 0); op(0x43, 0, 0, 15);
    int frame_patch = ldi(1, 0); op(0x20, 15, 1, 1);
    ldi(2, 0x823fff00); op(0x2f, 1, 2, 0); chain(jump(3), &fail_jumps); op(0x42, 1, 0, 0);
    statement(); ldi(0, 0); leave();
    if (!problem) code[frame_patch] = frame_words;
    locals = 512; return 0;
}
int add_builtin(int *name, int argc, int callid) {
    int i = 0;
    while (name[i]) { token_text[i] = name[i]; i = i + 1; }
    token_text[i] = 0; token_length = i;
    int id = new_global(); kinds[id] = 6; addresses[id] = callid; arities[id] = argc; return 0;
}
int runtime_division() {
    if (!div_calls) return 0;
    link(div_calls, count); ldi(2, 0); ldi(3, 0); ldi(5, 0);
    op(0x2f, 0, 5, 0); chain(jump(2), &fail_jumps);
    op(0x2f, 1, 5, 0); int positive = jump(10);
    ldi(2, 1); ldi(3, 1); op(0x21, 5, 1, 1); patch(positive, count);
    op(0x2f, 0, 5, 0); positive = jump(10);
    ldi(6, 1); op(0x2c, 3, 6, 3); op(0x21, 5, 0, 0); patch(positive, count);
    op(0x2f, 4, 5, 0); int modulo = jump(5);
    op(0x25, 1, 0, 0); op(0x10, 3, 0, 2); int sign = jump(0);
    patch(modulo, count); op(0x26, 1, 0, 0); patch(sign, count);
    op(0x2f, 2, 5, 0); int done = jump(2);
    op(0x21, 5, 0, 0); patch(done, count); return emit(0x36);
}
int runtime_keys() {
    if (!key_calls) return 0;
    link(key_calls, count); int again = count;
    load(1, 0x81000000); load(2, 0x81000001); op(0x2f, 1, 2, 0); int available = jump(5);
    syscall(15); patch(jump(0), again);
    patch(available, count); ldi(3, 0x81000002); op(0x20, 3, 1, 3); op(0x11, 0, 3, 0);
    ldi(2, 1); op(0x20, 1, 2, 1); ldi(2, 255); op(0x28, 1, 2, 1); store(1, 0x81000000); emit(0x36);
    int callback = count;
    ldi(1, 255); op(0x28, 0, 1, 7); syscall(16); zero_test(); chain(jump(5), &fail_jumps);
    load(1, 0x81000001); ldi(2, 1); op(0x20, 1, 2, 2);
    ldi(3, 255); op(0x28, 2, 3, 2); load(3, 0x81000000); op(0x2f, 2, 3, 0); chain(jump(2), &fail_jumps);
    ldi(3, 0x81000002); op(0x20, 3, 1, 3); op(0x12, 7, 3, 0); store(2, 0x81000001);
    syscall(7); int fallback = count; patch(jump(0), fallback); return callback;
}
int compile() {
    cursor = 0; count = 0; globals = 1; locals = 512; data_words = 0; frame_words = 0;
    expression_depth = 0; statement_depth = 0; loop_depth = 0; div_calls = 0; key_calls = 0; fail_jumps = 0;
    problem = 0; line_number = 1; token_line = 1;
    add_builtin("sys", 4, -2); add_builtin("getchar", 0, -1); add_builtin("print", 1, -3);
    add_builtin("putchar", 1, 1); add_builtin("putstr", 1, 6); add_builtin("putint", 1, 5);
    add_builtin("sleep", 1, 29); add_builtin("exit", 1, 8);
    int boot = jump(0); next();
    while (!problem && token != 256) global_declaration();
    int main_id = 0; int i = 1;
    while (i < globals && !problem) {
        if (kinds[i] == 5 && !defined[i]) error("Function declared but not defined.");
        if (same(names + i * 32, "main")) main_id = i;
        i = i + 1;
    }
    if (!main_id || kinds[main_id] != 5 || arities[main_id] || types[main_id]) error("Define int main() with no parameters.");
    runtime_division(); int callback = runtime_keys();
    patch(boot, count); ldi(14, 0x82000000); op(0x42, 14, 0, 0); op(0x48, 14, 0, 0); ldi(15, 0);
    /* Keep ordinary globals zero-initialized; skip only explicit noinit arrays.
     * Coalesce adjacent normal globals so their startup code stays compact. */
    int first = 0; int span = 0; i = 1;
    while (i < globals && !problem) {
        if (kinds[i] == 1 || kinds[i] == 2) {
            if (defined[i]) {
                if (span) { ldi(0, first); clear_words(0, span); span = 0; }
            } else {
                if (!span) first = addresses[i];
                span = span + arities[i];
            }
        }
        i = i + 1;
    }
    if (span) { ldi(0, first); clear_words(0, span); }
    if (callback) {
        ldi(0, 0); store(0, 0x81000000); store(0, 0x81000001);
        ldi(1, 0x80000000 + callback); ldi(2, 0); ldi(3, 0x03000001); syscall(2);
        zero_test(); chain(jump(5), &fail_jumps);
    }
    emit(0x34); emit(0x80000000 + addresses[main_id]); op(0x10, 0, 0, 1); syscall(8);
    int halt_loop = count; patch(jump(0), halt_loop); link(fail_jumps, count);
    char *message = "Runtime error: division by zero, stack or input overflow.";
    token_length = 0;
    while (message[token_length]) { token_text[token_length] = message[token_length]; token_length = token_length + 1; }
    string_literal(); op(0x10, 0, 0, 1); syscall(6); ldi(1, 10); syscall(1); ldi(1, 2); syscall(8);
    halt_loop = count; patch(jump(0), halt_loop); return problem == 0;
}
int read_line(int *prompt, int *out) {
    putstr(prompt); int n = 0; int c = 0;
    while (1) {
        c = getchar();
        if (input_swallow_lf) { input_swallow_lf = 0; if (c == 10) continue; }
        if (c == 10 || c == 13) { input_swallow_lf = c == 13; break; }
        if (c == 8 || c == 127) { if (n) { n = n - 1; putchar(8); } }
        else if (c >= 32 && c <= 126) {
            if (n == 127) { putstr("\nInput too long.\n"); exit(1); }
            out[n] = c; n = n + 1; putchar(c);
        }
    }
    out[n] = 0; putchar(10); return 0;
}
int background_choice() {
    while (1) {
        read_line("Run in background? [Y/n] > ", background_answer);
        if (!background_answer[0] || same(background_answer, "y") || same(background_answer, "Y")) return 1;
        if (same(background_answer, "n") || same(background_answer, "N")) return 0;
        if (same(background_answer, "q") || same(background_answer, "Q")) return -1;
        putstr("Enter Y or N (q cancels).\n");
    }
    return 0;
}
int file_error(int status) { putstr("File error: "); putint(status); putstr(". Nothing installed.\n"); exit(1); return 0; }
/* The system console may be using storage while this compiler is background.
 * Yield on EBUSY; all other filesystem results retain their usual meaning. */
int fs_call(int request, int a, int b, int c) {
    int status = sys(request, a, b, c);
    while (status == -5) { sys(4, 0, 0, 0); status = sys(request, a, b, c); }
    return status;
}
int main() {
    putstr("Aurora-32 self-hosting Tiny C\n");
    putstr("Functions, recursion, arrays, pointers; native Aurora code.\n");
    putstr("Source <=65536 characters; code <=131072 words. q cancels.\n");
    read_line("Source > ", source_path);
    if (same(source_path, "q")) return 0;
    int id = fs_call(31, (int)source_path, 0, 0);
    if (id < 0) file_error(id);
    int status = fs_call(34, id, (int)file_info, 0);
    if (status < 0) file_error(status);
    if (file_info[1] != 2 || file_info[2] > 65536) { putstr("Expected text, at most 65536 characters.\n"); return 1; }
    source_size = file_info[2]; file_request[0] = id; file_request[1] = 0;
    while (file_request[1] < source_size) {
        file_request[3] = source_size - file_request[1];
        if (file_request[3] > 64) file_request[3] = 64;
        file_request[2] = (int)(source + file_request[1]); status = fs_call(32, (int)file_request, 0, 0);
        if (status != file_request[3]) file_error(status);
        file_request[1] = file_request[1] + status;
    }
    source[source_size] = 0; read_line("Output > ", output_path);
    if (same(output_path, "q")) return 0;
    int background_mode = background_choice();
    if (background_mode < 0) return 0;
    if (background_mode) {
        status = sys(36, 0, 0, 0);
        if (status == 0) putstr("Compiler running in background.\n");
        else putstr("Background unavailable; continuing in foreground.\n");
    } else putstr("Compiler running in foreground.\n");
    putstr("Compiling...\n");
    if (!compile()) {
        putstr("Compile error at line "); putint(problem_line); putstr(": "); putstr(problem);
        putstr("\nNothing saved.\n"); return 1;
    }
    putstr("Saving executable...\n"); status = fs_call(35, (int)output_path, (int)code, count);
    if (status == -10) { putstr("Output already exists. Choose a new name or remove it in the shell.\n"); return 1; }
    if (status < 0) file_error(status);
    putstr("Compiled "); putint(count); putstr(" code words. Run: "); putstr(output_path); putchar(10); return 0;
}
