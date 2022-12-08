# -*- coding:utf-8 -*-

################ Scheme Interpreter in Python

## (c) Peter Norvig, 2010; See http://norvig.com/lispy2.html

################ Symbol, Procedure, classes

from __future__ import division
import re, sys, StringIO

class Symbol(str): pass

def Sym(s, symbol_table={}):
    "Find or create unique Symbol entry for str s in symbol table."
    if s not in symbol_table: symbol_table[s] = Symbol(s)
    return symbol_table[s]

_quote, _if, _set, _define, _lambda, _begin, _definemacro, = map(Sym, 
"引号   if   设置!  定义   兰姆达   开头   定义-宏".split())

_quasiquote, _unquote, _unquotesplicing = map(Sym,
"准引号   反引号   反引号-拼接".split())

class Procedure(object):
    "A user-defined Scheme procedure."
    def __init__(self, parms, exp, env):
        self.parms, self.exp, self.env = parms, exp, env
    def __call__(self, *args): 
        return eval(self.exp, Env(self.parms, args, self.env))

################ parse, read, and user interaction

def parse(inport):
    "Parse a program: read and expand/error-check it."
    # Backwards compatibility: given a str, convert it to an InPort
    if isinstance(inport, str): inport = InPort(StringIO.StringIO(inport))
    return expand(read(inport), toplevel=True)

eof_object = Symbol('#<终文件-对象>') # Note: uninterned; can't be read

class InPort(object):
    "An input port. Retains a line of chars."
    tokenizer = r"""\s*(,@|[('`,)]|"(?:[\\].|[^\\"])*"|;.*|[^\s('"`,;)]*)(.*)"""
    def __init__(self, file):
        self.file = file; self.line = ''
    def next_token(self):
        "Return the next token, reading new text into line buffer if needed."
        while True:
            if self.line == '': self.line = self.file.readline()
            if self.line == '': return eof_object
            token, self.line = re.match(InPort.tokenizer, self.line).groups()
            if token != '' and not token.startswith(';'):
                return token

def readchar(inport):
    "Read the next character from an input port."
    if inport.line != '':
        ch, inport.line = inport.line[0], inport.line[1:]
        return ch
    else:
        return inport.file.read(1) or eof_object

def read(inport):
    "Read a Scheme expression from an input port."
    def read_ahead(token):
        if '(' == token: 
            L = []
            while True:
                token = inport.next_token()
                if token == ')': return L
                else: L.append(read_ahead(token))
        elif ')' == token: raise SyntaxError('不期望 )')
        elif token in quotes: return [quotes[token], read(inport)]
        elif token is eof_object: raise SyntaxError('不期望 终文件 在列表内')
        else: return atom(token)
    # body of read:
    token1 = inport.next_token()
    return eof_object if token1 is eof_object else read_ahead(token1)

quotes = {"'":_quote, "`":_quasiquote, ",":_unquote, ",@":_unquotesplicing}

def atom(token):
    'Numbers become numbers; #t and #f are booleans; "..." string; otherwise Symbol.'
    if token == '#真': return True
    elif token == '#假': return False
    elif token[0] == '"': return token[1:-1].decode('字符串_跳出')
    try: return int(token)
    except ValueError:
        try: return float(token)
        except ValueError:
            try: return complex(token.replace('i', 'j', 1))
            except ValueError:
                return Sym(token)

def to_string(x):
    "Convert a Python object back into a Lisp-readable string."
    if x is True: return "#真"
    elif x is False: return "#假"
    elif isa(x, Symbol): return x
    elif isa(x, str): return '"%s"' % x.encode('字符串_跳出').replace('"',r'\"')
    elif isa(x, list): return '('+' '.join(map(to_string, x))+')'
    elif isa(x, complex): return str(x).replace('j', 'i')
    else: return str(x)

def load(filename):
    "Eval every expression from a file."
    repl(None, InPort(open(filename)), None)

def repl(prompt='树语言> ', inport=InPort(sys.stdin), out=sys.stdout):
    "A prompt-read-eval-print loop."
    sys.stderr.write("树语言 version 2.0\n")
    while True:
        try:
            if prompt: sys.stderr.write(prompt)
            x = parse(inport)
            if x is eof_object: return
            val = eval(x)
            if val is not None and out: print >> out, to_string(val)
        except Exception as e:
            print '%s: %s' % (type(e).__name__, e)

################ Environment class

class Env(dict):
    "An environment: a dict of {'var':val} pairs, with an outer Env."
    def __init__(self, parms=(), args=(), outer=None):
        # Bind parm list to corresponding args, or single parm to list of args
        self.outer = outer
        if isa(parms, Symbol): 
            self.update({parms:list(args)})
        else: 
            if len(args) != len(parms):
                raise TypeError('期望 %s, 给定 %s, ' 
                                % (to_string(parms), to_string(args)))
            self.update(zip(parms,args))
    def find(self, var):
        "Find the innermost Env where var appears."
        if var in self: return self
        elif self.outer is None: raise LookupError(var)
        else: return self.outer.find(var)

def is_pair(x): return x != [] and isa(x, list)
def cons(x, y): return [x]+y

def callcc(proc):
    "Call proc with current continuation; escape only"
    ball = RuntimeWarning("抱歉, 不能再继续此延续.")
    def throw(retval): ball.retval = retval; raise ball
    try:
        return proc(throw)
    except RuntimeWarning as w:
        if w is ball: return ball.retval
        else: raise w

def add_globals(self):
    "Add some Scheme standard procedures."
    import math, cmath, operator as op
    self.update(vars(math))
    self.update(vars(cmath))
    self.update({
     '+':op.add, '-':op.sub, '*':op.mul, '/':op.div, '非':op.not_, 
     '>':op.gt, '<':op.lt, '>=':op.ge, '<=':op.le, '=':op.eq, 
     '相等?':op.eq, '等于?':op.is_, '长度':len, '构造':cons,
     '切头':lambda x:x[0], '切尾':lambda x:x[1:], '追加':op.add,  
     '列表':lambda *x:list(x), '列表?': lambda x:isa(x,list),
     '空指针?':lambda x:x==[], '符号?':lambda x: isa(x, Symbol),
     '布尔?':lambda x: isa(x, bool), '点对?':is_pair, 
     '端口?': lambda x:isa(x,file), '应用':lambda proc,l: proc(*l), 
     '求值':lambda x: eval(expand(x)), '载入':lambda fn: load(fn), '调用/当前继续':callcc,
     '打开-输入-文件':open,'关闭-输入-端口':lambda p: p.file.close(), 
     '打开-输出-文件':lambda f:open(f,'w'), '关闭-输出-端口':lambda p: p.close(),
     '终文件-对象?':lambda x:x is eof_object, '读-字符':readchar,
     '读':read, '写':lambda x,port=sys.stdout:port.write(to_string(x)),
     '显示':lambda x,port=sys.stdout:port.write(x if isa(x,str) else to_string(x))})
    return self

isa = isinstance

global_env = add_globals(Env())

################ eval (tail recursive)

def eval(x, env=global_env):
    "Evaluate an expression in an environment."
    while True:
        if isa(x, Symbol):       # variable reference
            return env.find(x)[x]
        elif not isa(x, list):   # constant literal
            return x                
        elif x[0] is _quote:     # (引号 exp)
            (_, exp) = x
            return exp
        elif x[0] is _if:        # (if test conseq alt)
            (_, test, conseq, alt) = x
            x = (conseq if eval(test, env) else alt)
        elif x[0] is _set:       # (设置! var exp)
            (_, var, exp) = x
            env.find(var)[var] = eval(exp, env)
            return None
        elif x[0] is _define:    # (定义 var exp)
            (_, var, exp) = x
            env[var] = eval(exp, env)
            return None
        elif x[0] is _lambda:    # (lambda (var*) exp)
            (_, vars, exp) = x
            return Procedure(vars, exp, env)
        elif x[0] is _begin:     # (开头 exp+)
            for exp in x[1:-1]:
                eval(exp, env)
            x = x[-1]
        else:                    # (proc exp*)
            exps = [eval(exp, env) for exp in x]
            proc = exps.pop(0)
            if isa(proc, Procedure):
                x = proc.exp
                env = Env(proc.parms, exps, proc.env)
            else:
                return proc(*exps)

################ expand

def expand(x, toplevel=False):
    "Walk tree of x, making optimizations/fixes, and signaling SyntaxError."
    require(x, x!=[])                    # () => Error
    if not isa(x, list):                 # constant => unchanged
        return x
    elif x[0] is _quote:                 # (引号 exp)
        require(x, len(x)==2)
        return x
    elif x[0] is _if:                    
        if len(x)==3: x = x + [None]     # (if t c) => (if t c None)
        require(x, len(x)==4)
        return map(expand, x)
    elif x[0] is _set:                   
        require(x, len(x)==3); 
        var = x[1]                       # (设置! non-var exp) => Error
        require(x, isa(var, Symbol), "能 设置! 只有符号")
        return [_set, var, expand(x[2])]
    elif x[0] is _define or x[0] is _definemacro: 
        require(x, len(x)>=3)            
        _def, v, body = x[0], x[1], x[2:]
        if isa(v, list) and v:           # (定义 (f args) body)
            f, args = v[0], v[1:]        #  => (定义 f (lambda (args) body))
            return expand([_def, f, [_lambda, args]+body])
        else:
            require(x, len(x)==3)        # (定义 non-var/list exp) => Error
            require(x, isa(v, Symbol), "能 定义 只有符号")
            exp = expand(x[2])
            if _def is _definemacro:     
                require(x, toplevel, "定义-宏 仅允许在顶层")
                proc = eval(exp)       
                require(x, callable(proc), "宏 必须是一个过程")
                macro_table[v] = proc    # (定义-宏 v proc)
                return None              #  => None; add v:proc to macro_table
            return [_define, v, exp]
    elif x[0] is _begin:
        if len(x)==1: return None        # (开头) => None
        else: return [expand(xi, toplevel) for xi in x]
    elif x[0] is _lambda:                # (lambda (x) e1 e2) 
        require(x, len(x)>=3)            #  => (lambda (x) (开头 e1 e2))
        vars, body = x[1], x[2:]
        require(x, (isa(vars, list) and all(isa(v, Symbol) for v in vars))
                or isa(vars, Symbol), "illegal lambda argument list")
        exp = body[0] if len(body) == 1 else [_begin] + body
        return [_lambda, vars, expand(exp)]   
    elif x[0] is _quasiquote:            # `x => expand_quasiquote(x)
        require(x, len(x)==2)
        return expand_quasiquote(x[1])
    elif isa(x[0], Symbol) and x[0] in macro_table:
        return expand(macro_table[x[0]](*x[1:]), toplevel) # (m arg...) 
    else:                                #        => macroexpand if m isa 宏
        return map(expand, x)            # (f arg...) => expand each

def require(x, predicate, msg="wrong length"):
    "Signal a syntax error if predicate is false."
    if not predicate: raise SyntaxError(to_string(x)+': '+msg)

_append, _cons, _let = map(Sym, "追加 构造 让".split())

def expand_quasiquote(x):
    """Expand `x => 'x; `,x => x; `(,@x y) => (append x y) """
    if not is_pair(x):
        return [_quote, x]
    require(x, x[0] is not _unquotesplicing, "can't splice here")
    if x[0] is _unquote:
        require(x, len(x)==2)
        return x[1]
    elif is_pair(x[0]) and x[0][0] is _unquotesplicing:
        require(x[0], len(x[0])==2)
        return [_append, x[0][1], expand_quasiquote(x[1:])]
    else:
        return [_cons, expand_quasiquote(x[0]), expand_quasiquote(x[1:])]

def let(*args):
    args = list(args)
    x = cons(_let, args)
    require(x, len(args)>1)
    bindings, body = args[0], args[1:]
    require(x, all(isa(b, list) and len(b)==2 and isa(b[0], Symbol)
                   for b in bindings), "illegal binding list")
    vars, vals = zip(*bindings)
    return [[_lambda, list(vars)]+map(expand, body)] + map(expand, vals)

macro_table = {_let:let} ## More macros can go here

eval(parse("""(开头

(定义-宏 与 (兰姆达 实参 
   (若 (空指针? 实参) #真
       (若 (= (长度 实参) 1) (切头 实参)
           `(若 ,(切头 实参) (与 ,@(切尾 实参)) #假)))))

;; More macros can also go here

)"""))

if __name__ == '__main__':
    repl()

