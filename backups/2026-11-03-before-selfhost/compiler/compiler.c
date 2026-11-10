#include <aurora.h>
#include <aurora_system.h>
AURORA_USER_PROGRAM;

/* Mini-C: bounded recursive descent, direct native word-code emission.
 * No host-side compilation, privileged instructions or runtime interpreter. */
#define SOURCE_MAX 4096u
#define CODE_MAX 8192u
#define VAR_MAX 64u
#define BASE 0x80000000u
#define DATA 0x81000000u
#define KEY_READ (DATA+128u)
#define KEY_WRITE (DATA+129u)
#define KEY_DATA (DATA+130u)
typedef aurora_word_t W;
enum { END=256, IDENT, NUMBER, STRING, EQ, NE, LE, GE, AND, OR,
       INT, IF, ELSE, WHILE, BREAK, CONTINUE, RETURN };
static W source[SOURCE_MAX+1u] __attribute__((section(".noinit")));
static W code[CODE_MAX] __attribute__((section(".noinit")));
static W names[VAR_MAX][32] __attribute__((section(".noinit")));
static W token_text[256], token_length, token, number;
static W source_size, cursor, line_number, token_line, count, variables;
static W expression_depth, statement_depth, loop_depth;
static W loop_start[24], loop_break[24];
static W div_calls, key_calls, fail_jumps;
static const char *problem;
static W problem_line;
static W input[128], source_path[128], output_path[128];
static volatile W input_used, input_ready, input_overflow, accepting, skip_lf;
static aurora_fs_info info;
static aurora_fs_read_request request;

static void text(const char *s) {
  while(*s) {
    W n=0u; while(n<64u && s[n]) ++n;
    if(system.io.Write((const W *)s,n)!=(int)n) system.Exit(1);
    s+=n;
  }
}
__attribute__((noinline)) static void error(const char *s) {
  if(!problem) { problem=s; problem_line=token_line; }
}
static int same(const W *a,const char *b) {
  while(*a && *a==(W)*b) { ++a; ++b; }
  return *a==(W)*b;
}
static int letter(W c) { return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'; }
static int digit(W c) { return c>='0'&&c<='9'; }
static W take(void) {
  W c=source[cursor]; if(cursor<source_size) ++cursor;
  if(c=='\n') ++line_number; return c;
}
static W escape(void) {
  W c=take();
  if(c=='n') return '\n'; if(c=='r') return '\r'; if(c=='t') return '\t';
  if(c=='\\'||c=='\''||c=='"') return c;
  error("Unsupported escape; use n, r, t, backslash or quote."); return 0u;
}
static void next(void) {
  token=END; token_length=0u;
  if(problem) return;
  for(;;) {
    W c=source[cursor];
    if(c==' '||c=='\t'||c=='\r'||c=='\n') { take(); continue; }
    if(c=='/' && source[cursor+1u]=='/') {
      while(cursor<source_size && source[cursor]!='\n') take(); continue;
    }
    if(c=='/' && source[cursor+1u]=='*') {
      take(); take();
      while(cursor<source_size && !(source[cursor]=='*' && source[cursor+1u]=='/')) take();
      if(cursor==source_size) { token_line=line_number; error("Unclosed comment."); return; }
      take(); take(); continue;
    }
    break;
  }
  token_line=line_number;
  if(cursor==source_size) return;
  W c=take();
  if(letter(c)) {
    token_text[token_length++]=c;
    while(letter(source[cursor])||digit(source[cursor])) {
      if(token_length==31u) { error("Identifier exceeds 31 characters."); return; }
      token_text[token_length++]=take();
    }
    token_text[token_length]=0u; token=IDENT;
    if(same(token_text,"int")) token=INT;
    else if(same(token_text,"if")) token=IF;
    else if(same(token_text,"else")) token=ELSE;
    else if(same(token_text,"while")) token=WHILE;
    else if(same(token_text,"break")) token=BREAK;
    else if(same(token_text,"continue")) token=CONTINUE;
    else if(same(token_text,"return")) token=RETURN;
    return;
  }
  if(digit(c)) {
    number=c-'0';
    while(digit(source[cursor])) {
      W d=take()-'0';
      if(number>429496729u || (number==429496729u && d>5u)) {
        error("Integer exceeds 32 bits."); return;
      }
      number=number*10u+d;
    }
    token=NUMBER; return;
  }
  if(c=='"'||c=='\'') {
    W quote=c;
    while(cursor<source_size && source[cursor]!=quote) {
      c=take();
      if(c=='\n'||c=='\r'||c==0u) { error("Unclosed literal."); return; }
      if(c=='\\') c=escape();
      if(problem) return;
      if(token_length==255u) { error("String exceeds 255 characters."); return; }
      token_text[token_length++]=c;
    }
    if(cursor==source_size) { error("Unclosed literal."); return; }
    take(); token_text[token_length]=0u;
    if(quote=='\'') {
      if(token_length!=1u) { error("Character literal needs one character."); return; }
      number=token_text[0]; token=NUMBER;
    } else token=STRING;
    return;
  }
  token=c;
  if(c=='=' && source[cursor]=='=') { take(); token=EQ; }
  else if(c=='!' && source[cursor]=='=') { take(); token=NE; }
  else if(c=='<' && source[cursor]=='=') { take(); token=LE; }
  else if(c=='>' && source[cursor]=='=') { take(); token=GE; }
  else if(c=='&' && source[cursor]=='&') { take(); token=AND; }
  else if(c=='|' && source[cursor]=='|') { take(); token=OR; }
  else if(!(c=='+'||c=='-'||c=='*'||c=='/'||c=='%'||c=='('||c==')'||c=='{'||
            c=='}'||c==';'||c=='='||c=='!'||c=='<'||c=='>')) error("Unsupported character or operator.");
}
static void expect(W t) {
  if(token!=t) { error(t==';'?"Expected ';'.":t==')'?"Expected ')'.":
      t=='('?"Expected '('.":t=='}'?"Expected '}'.":"Unexpected token."); return; }
  next();
}
__attribute__((noinline)) static W emit(W word) {
  if(problem) return 0u;
  if(count==CODE_MAX) { error("Generated code exceeds 8192 words."); return 0u; }
  code[count]=word; return count++;
}
/* Encodings match llvm/lib/Target/Aurora-32/AuroraInstr{Formats,Info}.td. */
static void op(W opcode,W a,W b,W d) { emit(opcode+a*65536u+b*4096u+d*256u); }
static void ldi(W reg,W value) { emit(0x15u+reg*256u); emit(value); }
static void load(W reg,W address) { emit(0x13u+reg*256u); emit(address); }
static void store(W reg,W address) { emit(0x14u+reg*65536u); emit(address); }
static W jump(W condition) { emit(condition?0x31u+condition*1048576u:0x30u); return emit(0u); }
static void patch(W at,W target) { if(!problem) code[at]=BASE+target; }
static void chain(W at,W *head) { if(!problem) { code[at]=*head; *head=at; } }
static void link(W head,W target) {
  while(head && !problem) { W tail=code[head]; patch(head,target); head=tail; }
}
static void call(W *head) { emit(0x34u); chain(emit(0u),head); }
static void syscall(W id) { ldi(0u,id); emit(0x37u); emit(2u); }
static void exit_code(void) {
  ldi(2u,0u); ldi(3u,0u); syscall(8u); W at=count; patch(jump(0u),at);
}
static void zero_test(void) { ldi(1u,0u); op(0x2fu,0u,1u,0u); }
static void boolean(W condition) {
  W yes=jump(condition); ldi(0u,0u); W end=jump(0u);
  patch(yes,count); ldi(0u,1u); patch(end,count);
}
static void literal(void) {
  /* Embed word characters behind a jump; no extra RAM copy at runtime. */
  emit(0x115u); W ptr=emit(0u);
  ldi(2u,0u); ldi(3u,0u); syscall(6u);
  W after=jump(0u); patch(ptr,count);
  for(W i=0u;i<token_length;++i) emit(token_text[i]); emit(0u);
  patch(after,count);
}
static int find_variable(void) {
  for(W i=0u;i<variables;++i) {
    W j=0u; while(names[i][j] && names[i][j]==token_text[j]) ++j;
    if(names[i][j]==token_text[j]) return (int)i;
  }
  return -1;
}
static int builtin(const char *s) { return token==IDENT && same(token_text,s); }
static int reserved(void) {
  return builtin("print")||builtin("putchar")||builtin("getchar")||builtin("sleep")||builtin("exit")||builtin("main");
}
static W precedence(W t) {
  if(t==OR) return 1u; if(t==AND) return 2u;
  if(t==EQ||t==NE) return 3u;
  if(t=='<'||t=='>'||t==LE||t==GE) return 4u;
  if(t=='+'||t=='-') return 5u;
  if(t=='*'||t=='/'||t=='%') return 6u;
  return 0u;
}
static void expression(W minimum);
static void atom(void) {
  if(token==NUMBER) { ldi(0u,number); next(); }
  else if(token=='(') { next(); expression(1u); expect(')'); }
  else if(token=='+'||token=='-'||token=='!') {
    W unary=token; next(); expression(7u);
    if(unary=='-') { ldi(1u,0u); op(0x21u,1u,0u,0u); }
    else if(unary=='!') { zero_test(); boolean(2u); }
  } else if(builtin("getchar")) { next(); expect('('); expect(')'); call(&key_calls); }
  else if(token==IDENT) {
    int id=find_variable(); if(id<0) { error("Unknown variable."); return; }
    load(0u,DATA+(W)id); next();
  } else error("Expected an integer expression.");
}
static void expression(W minimum) {
  if(++expression_depth>24u) { error("Expression nesting exceeds 24."); --expression_depth; return; }
  atom();
  while(!problem && precedence(token)>=minimum && precedence(token)) {
    W operator=token, level=precedence(token); next();
    if(operator==AND||operator==OR) {
      zero_test(); W shorted=jump(operator==AND?2u:5u);
      expression(level+1u); zero_test(); boolean(5u); W end=jump(0u);
      patch(shorted,count); ldi(0u,operator==OR?1u:0u); patch(end,count);
    } else {
      op(0x40u,0u,0u,0u); expression(level+1u); op(0x41u,0u,0u,1u);
      if(operator=='+') op(0x20u,1u,0u,0u);
      else if(operator=='-') op(0x21u,1u,0u,0u);
      else if(operator=='*') op(0x24u,1u,0u,0u);
      else if(operator=='/'||operator=='%') { ldi(4u,operator=='%'?1u:0u); call(&div_calls); }
      else {
        op(0x2fu,1u,0u,0u);
        boolean(operator==EQ?2u:operator==NE?5u:operator=='<'?9u:operator==LE?12u:operator=='>'?11u:10u);
      }
    }
  }
  --expression_depth;
}
static void statement(void);
static void declaration(void) {
  if(token!=IDENT || reserved()) { error("Expected a non-reserved variable name."); return; }
  if(find_variable()>=0) { error("Duplicate variable (one function-wide scope)."); return; }
  if(variables==VAR_MAX) { error("Too many variables (maximum 64)."); return; }
  W id=variables++;
  for(W i=0u;i<=token_length;++i) names[id][i]=token_text[i]; next();
  if(token=='=') { next(); expression(1u); } else ldi(0u,0u);
  store(0u,DATA+id); expect(';');
}
static void statement(void) {
  if(problem) return;
  if(++statement_depth>24u) { error("Statement nesting exceeds 24."); --statement_depth; return; }
  if(token=='{') {
    next(); while(!problem && token!='}' && token!=END) statement(); expect('}');
  } else if(token==INT) { next(); declaration(); }
  else if(token==IF) {
    next(); expect('('); expression(1u); expect(')'); zero_test(); W no=jump(2u);
    statement();
    if(token==ELSE) { W end=jump(0u); patch(no,count); next(); statement(); patch(end,count); }
    else patch(no,count);
  } else if(token==WHILE) {
    next(); expect('(');
    if(loop_depth==24u) error("Too many nested loops.");
    else {
      W slot=loop_depth++; loop_start[slot]=count; loop_break[slot]=0u;
      expression(1u); expect(')'); zero_test(); W end=jump(2u);
      statement(); patch(jump(0u),loop_start[slot]); patch(end,count);
      link(loop_break[slot],count); --loop_depth;
    }
  } else if(token==BREAK||token==CONTINUE) {
    W kind=token; next(); expect(';');
    if(!loop_depth) error("break/continue requires a loop.");
    else if(kind==CONTINUE) patch(jump(0u),loop_start[loop_depth-1u]);
    else chain(jump(0u),&loop_break[loop_depth-1u]);
  } else if(token==RETURN) {
    next(); expression(1u); expect(';'); op(0x10u,0u,0u,1u); exit_code();
  } else if(builtin("print")) {
    next(); expect('(');
    if(token==STRING) { literal(); next(); }
    else { expression(1u); op(0x10u,0u,0u,1u); ldi(2u,0u); ldi(3u,0u); syscall(5u); }
    expect(')'); expect(';'); ldi(1u,'\n'); syscall(1u);
  } else if(builtin("putchar")||builtin("sleep")||builtin("exit")) {
    W id=builtin("putchar")?1u:builtin("sleep")?29u:8u;
    next(); expect('('); expression(1u); expect(')'); expect(';');
    op(0x10u,0u,0u,1u); ldi(2u,0u); ldi(3u,0u);
    if(id==8u) exit_code(); else syscall(id);
  } else if(token==IDENT) {
    int id=find_variable(); if(id<0) error("Unknown variable or unsupported function.");
    else { next(); expect('='); expression(1u); expect(';'); store(0u,DATA+(W)id); }
  } else if(token==';') next();
  else error("Expected a statement.");
  --statement_depth;
}
static void runtime_division(void) {
  if(!div_calls) return;
  link(div_calls,count);
  /* Inputs r1=dividend, r0=divisor, r4=modulo. Unsigned magnitudes handle
   * INT_MIN too; overflow wraps to 32 bits, division truncates toward zero. */
  ldi(2u,0u); ldi(3u,0u); ldi(5u,0u);
  op(0x2fu,0u,5u,0u); chain(jump(2u),&fail_jumps);
  op(0x2fu,1u,5u,0u); W positive=jump(10u);
  ldi(2u,1u); ldi(3u,1u); op(0x21u,5u,1u,1u); patch(positive,count);
  op(0x2fu,0u,5u,0u); positive=jump(10u);
  ldi(6u,1u); op(0x2cu,3u,6u,3u); op(0x21u,5u,0u,0u); patch(positive,count);
  op(0x2fu,4u,5u,0u); W modulo=jump(5u);
  op(0x25u,1u,0u,0u); op(0x10u,3u,0u,2u); W sign=jump(0u);
  patch(modulo,count); op(0x26u,1u,0u,0u); patch(sign,count);
  op(0x2fu,2u,5u,0u); W done=jump(2u);
  op(0x21u,5u,0u,0u); patch(done,count); emit(0x36u);
}
static W runtime_keys(void) {
  if(!key_calls) return 0u;
  link(key_calls,count); W again=count;
  load(1u,KEY_READ); load(2u,KEY_WRITE); op(0x2fu,1u,2u,0u); W available=jump(5u);
  ldi(1u,0u); ldi(2u,0u); ldi(3u,0u); syscall(15u); patch(jump(0u),again);
  patch(available,count); ldi(3u,KEY_DATA); op(0x20u,3u,1u,3u); op(0x11u,0u,3u,0u);
  ldi(2u,1u); op(0x20u,1u,2u,1u); ldi(2u,63u); op(0x28u,1u,2u,1u);
  store(1u,KEY_READ); emit(0x36u);
  /* Direct callback entry: finish with INT CALLBACK_RETURN, never RET. */
  W callback=count;
  ldi(1u,255u); op(0x28u,0u,1u,7u); syscall(16u); zero_test(); chain(jump(5u),&fail_jumps);
  load(1u,KEY_WRITE); ldi(2u,1u); op(0x20u,1u,2u,2u);
  ldi(3u,63u); op(0x28u,2u,3u,2u); load(3u,KEY_READ);
  op(0x2fu,2u,3u,0u); chain(jump(2u),&fail_jumps);
  ldi(3u,KEY_DATA); op(0x20u,3u,1u,3u); op(0x12u,7u,3u,0u); store(2u,KEY_WRITE);
  syscall(7u); W fallback=count; patch(jump(0u),fallback);
  return callback;
}
static int compile(void) {
  cursor=count=variables=expression_depth=statement_depth=loop_depth=0u;
  div_calls=key_calls=fail_jumps=0u; problem=0; line_number=token_line=1u;
  W boot=jump(0u); next();
  if(token==INT) {
    next();
    if(builtin("main")) { next(); expect('('); expect(')');
      if(token!='{') error("Expected main body."); else statement();
    } else { declaration(); while(!problem && token!=END) statement(); }
  } else while(!problem && token!=END) statement();
  if(token!=END) error("Only one main function is supported.");
  ldi(1u,0u); exit_code();
  runtime_division(); W callback=runtime_keys();
  patch(boot,count); ldi(14u,0x82000000u); op(0x42u,14u,0u,0u); op(0x48u,14u,0u,0u);
  ldi(0u,0u); for(W i=0u;i<variables;++i) store(0u,DATA+i);
  if(callback) {
    ldi(0u,0u); store(0u,KEY_READ); store(0u,KEY_WRITE);
    ldi(1u,BASE+callback); ldi(2u,0u); ldi(3u,AURORA_IVT_KEYBOARD); syscall(2u);
    zero_test(); chain(jump(5u),&fail_jumps);
  }
  patch(jump(0u),2u);
  link(fail_jumps,count);
  /* A runtime error cannot fall through into embedded data or restart main. */
  const char *message="Runtime error: division by zero or input overflow.";
  token_length=0u; while(message[token_length]) { token_text[token_length]=(W)message[token_length]; ++token_length; }
  literal(); ldi(1u,'\n'); syscall(1u); ldi(1u,2u); exit_code();
  return problem==0;
}
static void key(W event) {
  if(!accepting || input_ready) return;
  W c=aurora_event_character(event);
  if(c=='\n' && skip_lf) { skip_lf=0u; return; } skip_lf=c=='\r';
  if(c=='\r'||c=='\n') { system.io.PutChar('\n'); input[input_used]=0u; input_ready=1u; return; }
  if(input_overflow) return;
  if(c==8u||c==127u) { if(input_used) { --input_used; system.io.PutChar('\b'); } return; }
  if(c<32u||c>126u) return;
  if(input_used==127u) { input_overflow=1u; return; }
  input[input_used++]=c; system.io.PutChar((char)c);
}
static void read_line(const char *prompt,W *out) {
  input_used=input_ready=input_overflow=0u; accepting=1u; text(prompt);
  while(!input_ready) system.WaitEvent();
  accepting=0u;
  if(input_overflow || system.EventDrops()) { text("Input overflow. Nothing saved.\n"); system.Exit(1); }
  for(W i=0u;i<=input_used;++i) out[i]=input[i];
}
static void io_error(int status) {
  text("File error: "); system.io.PutInt(status); text(". Nothing installed.\n"); system.Exit(1);
}
__attribute__((noreturn)) void _start(void) {
  aurora.SetUpStackPointer();
  system.DeferEvents();
  if(system.io.Subscribe(key)!=AURORA_OK) system.Exit(1);
  text("Aurora-32 Mini-C Compiler\nNative code; int, if/else, while, print, getchar.\n"
       "Source: existing text file (max 4096 characters).\nOutput: NEW executable path. q cancels.\n");
  system.ResumeEvents();
  read_line("Source > ",source_path);
  if(same(source_path,"q")) system.Exit(0);
  int id=system.fs.Open((const char *)source_path); if(id<0) io_error(id);
  int status=system.fs.Info((W)id,&info); if(status<0) io_error(status);
  if(info.type!=AURORA_FILE_TEXT || info.words>SOURCE_MAX) { text("Expected text, at most 4096 characters.\n"); system.Exit(1); }
  source_size=info.words; request.id=(W)id; request.offset=0u;
  while(request.offset<source_size) {
    request.count=source_size-request.offset; if(request.count>64u) request.count=64u;
    request.buffer=(W)(unsigned long)(source+request.offset);
    status=system.fs.Read(&request); if(status!=(int)request.count) io_error(status<0?status:AURORA_EIO);
    request.offset+=(W)status;
  }
  source[source_size]=0u;
  read_line("Output > ",output_path);
  if(same(output_path,"q")) system.Exit(0);
  text("Compiling...\n");
  if(!compile()) {
    text("Compile error at line "); system.io.PutInt((int)problem_line);
    text(": "); text(problem); text("\nNothing saved.\n"); system.Exit(1);
  }
  text("Saving executable...\n");
  status=system.fs.Install((const char *)output_path,code,count);
  if(status==AURORA_EEXIST) { text("Output already exists. Choose a new name or remove it in the shell.\n"); system.Exit(1); }
  if(status<0) io_error(status);
  text("Compiled "); system.io.PutInt((int)count); text(" code words. Run: ");
  text((const char *)output_path); text("\n"); system.Exit(0);
  for(;;) system.Yield();
}
