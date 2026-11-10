#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

void infix_to_postfix(const char *expr, char postfix[][32], int *post_count) {
    char stack[100];
    int top = -1;
    *post_count = 0;
    int i = 0;

    while (expr[i] != '\0') {
        if (isspace(expr[i])) { i++; continue; }

        if (isalnum(expr[i])) {
            int len = 0;
            while (isalnum(expr[i])) {
                postfix[*post_count][len++] = expr[i++];
            }
            postfix[*post_count][len] = '\0';
            (*post_count)++;
        } else if (expr[i] == '(') {
            stack[++top] = expr[i++];
        } else if (expr[i] == ')') {
            while (top >= 0 && stack[top] != '(') {
                postfix[*post_count][0] = stack[top--];
                postfix[*post_count][1] = '\0';
                (*post_count)++;
            }
            if (top >= 0) top--;
            i++;
        } else {
            char op = expr[i++];
            while (top >= 0 && stack[top] != '(' && precedence(stack[top]) >= precedence(op)) {
                postfix[*post_count][0] = stack[top--];
                postfix[*post_count][1] = '\0';
                (*post_count)++;
            }
            stack[++top] = op;
        }
    }
    while (top >= 0) {
        postfix[*post_count][0] = stack[top--];
        postfix[*post_count][1] = '\0';
        (*post_count)++;
    }
}

typedef struct {
    int mem[100];
    int acc;
    int pc;
} simpletron;

void simpletron_init(simpletron *s) {
    memset(s->mem, 0, sizeof(s->mem));
    s->acc = 0;
    s->pc = 0;
}

int simpletron_load(simpletron *s, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    simpletron_init(s);
    int val, addr = 0;
    while (fscanf(f, "%d", &val) == 1 && addr < 100) {
        s->mem[addr++] = val;
    }
    fclose(f);
    return 1;
}

void simpletron_run(simpletron *s) {
    while (s->pc >= 0 && s->pc < 100) {
        int ir = s->mem[s->pc];
        int op = abs(ir) / 100;
        int arg = abs(ir) % 100;
        if (ir < 0) op = -op;
        s->pc++;

        if (op == 10) { scanf("%d", &s->mem[arg]); }
        else if (op == 11) { printf("%d\n", s->mem[arg]); }
        else if (op == 12) { printf("\n"); }
        else if (op == 20) { s->acc = s->mem[arg]; }
        else if (op == 21) { s->mem[arg] = s->acc; }
        else if (op == 30) { s->acc += s->mem[arg]; }
        else if (op == 31) { s->acc -= s->mem[arg]; }
        else if (op == 32) { if (s->mem[arg]) s->acc /= s->mem[arg]; }
        else if (op == 33) { s->acc *= s->mem[arg]; }
        else if (op == 34) { if (s->mem[arg]) s->acc %= s->mem[arg]; }
        else if (op == 35) { s->acc = (int)pow(s->acc, s->mem[arg]); }
        else if (op == 40) { s->pc = arg; }
        else if (op == 41) { if (s->acc < 0) s->pc = arg; }
        else if (op == 42) { if (s->acc == 0) s->pc = arg; }
        else if (op == 43) { break; }
    }
}

typedef struct {
    char name[32];
    char type;
    int loc;
} symbol;

typedef struct {
    int pc_loc;
    int line_target;
} flag;

typedef struct {
    symbol syms[100];
    int sym_count;
    flag flags[100];
    int flag_count;
    int code[100];
    int pc;
    int data;
} simple_compiler;

void compiler_init(simple_compiler *c) {
    c->sym_count = 0;
    c->flag_count = 0;
    memset(c->code, 0, sizeof(c->code));
    c->pc = 0;
    c->data = 99;
}

int get_sym(simple_compiler *c, const char *name, char type) {
    for (int i = 0; i < c->sym_count; i++) {
        if (strcmp(c->syms[i].name, name) == 0 && c->syms[i].type == type) {
            return c->syms[i].loc;
        }
    }
    int loc = (type == 'v' || type == 'c') ? c->data-- : c->pc;
    strcpy(c->syms[c->sym_count].name, name);
    c->syms[c->sym_count].type = type;
    c->syms[c->sym_count].loc = loc;
    c->sym_count++;
    return loc;
}

int find_line_sym(simple_compiler *c, int line) {
    char buf[16];
    sprintf(buf, "%d", line);
    for (int i = 0; i < c->sym_count; i++) {
        if (strcmp(c->syms[i].name, buf) == 0 && c->syms[i].type == 'l') {
            return c->syms[i].loc;
        }
    }
    return -1;
}

void compile_line(simple_compiler *c, char *line) {
    int lbl;
    char cmd[32] = "", rest[128] = "";
    if (sscanf(line, "%d %s %[^\n]", &lbl, cmd, rest) < 2) return;

    char lbl_str[16];
    sprintf(lbl_str, "%d", lbl);
    get_sym(c, lbl_str, 'l');

    if (strcmp(cmd, "input") == 0) {
        c->code[c->pc++] = 1000 + get_sym(c, rest, 'v');
    } else if (strcmp(cmd, "print") == 0) {
        c->code[c->pc++] = 1100 + get_sym(c, rest, 'v');
    } else if (strcmp(cmd, "goto") == 0) {
        int target = atoi(rest);
        int loc = find_line_sym(c, target);
        if (loc != -1) {
            c->code[c->pc++] = 4000 + loc;
        } else {
            c->flags[c->flag_count].pc_loc = c->pc;
            c->flags[c->flag_count++].line_target = target;
            c->code[c->pc++] = 4000;
        }
    } else if (strcmp(cmd, "if") == 0) {
        char v1[32], rel[8], v2[32], goto_kw[8];
        int target;
        if (sscanf(rest, "%s %s %s %s %d", v1, rel, v2, goto_kw, &target) == 5) {
            c->code[c->pc++] = 2000 + get_sym(c, v1, isdigit(v1[0]) ? 'c' : 'v');
            c->code[c->pc++] = 3100 + get_sym(c, v2, isdigit(v2[0]) ? 'c' : 'v');
            int op = (strcmp(rel, "==") == 0) ? 4200 : ((strcmp(rel, "<") == 0) ? 4100 : 4000);
            int loc = find_line_sym(c, target);
            if (loc != -1) {
                c->code[c->pc++] = op + loc;
            } else {
                c->flags[c->flag_count].pc_loc = c->pc;
                c->flags[c->flag_count++].line_target = target;
                c->code[c->pc++] = op;
            }
        }
    } else if (strcmp(cmd, "let") == 0) {
        char target[32], expr[128];
        sscanf(rest, "%s = %[^\n]", target, expr);
        int t_loc = get_sym(c, target, 'v');
        char postfix[50][32];
        int post_count;
        infix_to_postfix(expr, postfix, &post_count);

        int stack[50], top = -1;
        for (int i = 0; i < post_count; i++) {
            if (isalnum(postfix[i][0])) {
                stack[++top] = get_sym(c, postfix[i], isdigit(postfix[i][0]) ? 'c' : 'v');
            } else {
                int o2 = stack[top--];
                int o1 = stack[top--];
                c->code[c->pc++] = 2000 + o1;
                char op = postfix[i][0];
                int code_op = (op == '+') ? 3000 : (op == '-') ? 3100 : (op == '*') ? 3300 : (op == '/') ? 3200 : (op == '%') ? 3400 : 3500;
                c->code[c->pc++] = code_op + o2;
                int tmp = c->data--;
                c->code[c->pc++] = 2100 + tmp;
                stack[++top] = tmp;
            }
        }
        int res = stack[top--];
        c->code[c->pc++] = 2000 + res;
        c->code[c->pc++] = 2100 + t_loc;
    } else if (strcmp(cmd, "end") == 0) {
        c->code[c->pc++] = 4300;
    }
}

int compile_file(const char *in_name, const char *out_name) {
    FILE *in = fopen(in_name, "r");
    if (!in) return 0;
    simple_compiler c;
    compiler_init(&c);

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        compile_line(&c, line);
    }
    fclose(in);

    for (int i = 0; i < c.flag_count; i++) {
        int loc = find_line_sym(&c, c.flags[i].line_target);
        if (loc != -1) {
            c.code[c.flags[i].pc_loc] += loc;
        }
    }

    FILE *out = fopen(out_name, "w");
    if (!out) return 0;
    for (int i = 0; i < c.pc; i++) {
        fprintf(out, "%c%04d\n", c.code[i] >= 0 ? '+' : '-', abs(c.code[i]));
    }
    fclose(out);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc > 3 && strcmp(argv[1], "--compile") == 0) {
        compile_file(argv[2], argv[3]);
    } else if (argc > 2 && strcmp(argv[1], "--run") == 0) {
        simpletron s;
        if (simpletron_load(&s, argv[2])) {
            simpletron_run(&s);
        }
    }
    return 0;
}
