"""A Scheme interpreter and its read-eval-print loop."""

from scheme_primitives import *
from scheme_reader import *
from ucb import main, trace

##############
# Eval/Apply #
##############

def scheme_eval(expr, env, _=None): # Optional third argument is ignored
    """Evaluate Scheme expression EXPR in environment ENV.

    >>> expr = read_line('(+ 2 2)')
    >>> expr
    Pair('+', Pair(2, Pair(2, nil)))
    >>> scheme_eval(expr, create_global_frame())
    4
    """
    # Evaluate atoms
    if scheme_symbolp(expr):
        return env.lookup(expr)
    elif self_evaluating(expr):
        return expr

    # All non-atomic expressions are lists (combinations)
    if not scheme_listp(expr):
        raise SchemeError('畸形的列表: {0}'.format(str(expr)))
    first, rest = expr.first, expr.second
    if scheme_symbolp(first) and first in SPECIAL_FORMS:
        return SPECIAL_FORMS[first](rest, env)
    else:
        # BEGIN PROBLEM 5
        "*** YOUR CODE HERE ***"
        operator = scheme_eval(first, env)  # Get the operator
        check_procedure(operator)  # Check the operator
        return operator.eval_call(rest, env)
        # END PROBLEM 5

def self_evaluating(expr):
    """Return whether EXPR evaluates to itself."""
    return scheme_atomp(expr) or scheme_stringp(expr) or expr is None

def scheme_apply(procedure, args, env):
    """Apply Scheme PROCEDURE to argument values ARGS (a Scheme list) in
    environment ENV."""
    check_procedure(procedure)
    return procedure.apply(args, env)

def eval_all(expressions, env):
    """Evaluate each expression im the Scheme list EXPRESSIONS in
    environment ENV and return the value of the last."""
    # BEGIN PROBLEM 8
    if expressions is nil:
        return None
    elif expressions.second is nil:  # Tail context
        return scheme_eval(expressions.first, env, True)
    else:
        scheme_eval(expressions.first, env)
        return eval_all(expressions.second, env)
    # END PROBLEM 8

################
# Environments #
################

class Frame:
    """An environment frame binds Scheme symbols to Scheme values."""

    def __init__(self, parent):
        """An empty frame with parent frame PARENT (which may be None)."""
        self.bindings = {}
        self.parent = parent

    def __repr__(self):
        if self.parent is None:
            return '<全局帧>'
        s = sorted(['{0}: {1}'.format(k, v) for k, v in self.bindings.items()])
        return '<{{{0}}} -> {1}>'.format(', '.join(s), repr(self.parent))

    def define(self, symbol, value):
        """Define Scheme SYMBOL to have VALUE."""
        # BEGIN PROBLEM 3
        "*** YOUR CODE HERE ***"
        self.bindings[symbol] = value
        # END PROBLEM 3

    def lookup(self, symbol):
        """Return the value bound to SYMBOL. Errors if SYMBOL is not found."""
        # BEGIN PROBLEM 3
        "*** YOUR CODE HERE ***"
        if symbol in self.bindings:
            return self.bindings[symbol]
        elif self.parent:
            return self.parent.lookup(symbol)
        # END PROBLEM 3
        raise SchemeError('未知的标识符: {0}'.format(symbol))

    def make_child_frame(self, formals, vals):
        """Return a new local frame whose parent is SELF, in which the symbols
        in a Scheme list of formal parameters FORMALS are bound to the Scheme
        values in the Scheme list VALS. Raise an error if too many or too few
        vals are given.

        >>> env = create_global_frame()
        >>> formals, expressions = read_line('(a b c)'), read_line('(1 2 3)')
        >>> env.make_child_frame(formals, expressions)
        <{a: 1, b: 2, c: 3} -> <Global Frame>>
        """
        child = Frame(self) # Create a new child with self as the parent
        # BEGIN PROBLEM 11
        "*** YOUR CODE HERE ***"
        if len(vals) > len(formals):
            raise SchemeError("提供了太多的值")
        elif len(vals) < len(formals):
            raise SchemeError("提供了太少的值")
        while formals is not nil:
            child.define(formals.first, vals.first)
            formals = formals.second
            vals = vals.second
        # END PROBLEM 11
        return child

##############
# Procedures #
##############

class Procedure:
    """The supertype of all Scheme procedures."""
    def eval_call(self, operands, env):
        """Standard function-call evaluation on SELF with OPERANDS as the
        unevaluated actual-parameter expressions and ENV as the environment
        in which the operands are to be evaluated."""
        # BEGIN PROBLEM 5
        "*** YOUR CODE HERE ***"
        eval_operands = lambda expr: scheme_eval(expr, env)
        return scheme_apply(self, operands.map(eval_operands), env)  # Every operands should be evaluated
        # END PROBLEM 5

def scheme_procedurep(x):
    return isinstance(x, Procedure)

class PrimitiveProcedure(Procedure):
    """A Scheme procedure defined as a Python function."""

    def __init__(self, fn, use_env=False, name='原语'):
        self.name = name
        self.fn = fn
        self.use_env = use_env

    def __str__(self):
        return '#[{0}]'.format(self.name)

    def apply(self, args, env):
        """Apply SELF to ARGS in ENV, where ARGS is a Scheme list.

        >>> env = create_global_frame()
        >>> plus = env.bindings['+']
        >>> twos = Pair(2, Pair(2, nil))
        >>> plus.apply(twos, env)
        4
        """
        if not scheme_listp(args):
            raise SchemeError('实参不在列表中: {0}'.format(args))
        # Convert a Scheme list to a Python list
        python_args = []
        while args is not nil:
            python_args.append(args.first)
            args = args.second
        # BEGIN PROBLEM 4
        "*** YOUR CODE HERE ***"
        if self.use_env:
            python_args.append(env)
        try:
            return self.fn(*python_args)
        except TypeError:
            raise SchemeError("传递的形参数目错误")
        # END PROBLEM 4

class UserDefinedProcedure(Procedure):
    """A procedure defined by an expression."""

    def apply(self, args, env):
        """Apply SELF to argument values ARGS in environment ENV. Applying a
        user-defined procedure evaluates all expressions in the body."""
        new_env = self.make_call_frame(args, env)
        return eval_all(self.body, new_env)

class LambdaProcedure(UserDefinedProcedure):
    """A procedure defined by a lambda expression or a define form."""

    def __init__(self, formals, body, env):
        """A procedure with formal parameter list FORMALS (a Scheme list),
        whose body is the Scheme list BODY, and whose parent environment
        starts with Frame ENV."""
        self.formals = formals
        self.body = body
        self.env = env

    def make_call_frame(self, args, env):
        """Make a frame that binds my formal parameters to ARGS, a Scheme list
        of values, for a lexically-scoped call evaluated in environment ENV."""
        # BEGIN PROBLEM 12
        "*** YOUR CODE HERE ***"
        return self.env.make_child_frame(self.formals, args)
        # END PROBLEM 12

    def __str__(self):
        return str(Pair('入', Pair(self.formals, self.body)))

    def __repr__(self):
        return '入过程({0}, {1}, {2})'.format(
            repr(self.formals), repr(self.body), repr(self.env))

class MacroProcedure(LambdaProcedure):
    """A macro: a special form that operates on its unevaluated operands to
    create an expression that is evaluated in place of a call."""

    def eval_call(self, operands, env):
        """Macro call evaluation on me with OPERANDS as the unevaluated
        actual-parameter expressions and ENV as the environment in which the
        resulting expanded expression is to be evaluated."""
        # BEGIN Problem 21
        "*** YOUR CODE HERE ***"
        applied = scheme_apply(self, operands, env)
        return scheme_eval(complete_eval(applied), env)
        # END Problem 21

def add_primitives(frame, funcs_and_names):
    """Enter bindings in FUNCS_AND_NAMES into FRAME, an environment frame,
    as primitive procedures. Each item in FUNCS_AND_NAMES has the form
    (NAME, PYTHON-FUNCTION, INTERNAL-NAME)."""
    for name, fn, proc_name in funcs_and_names:
        frame.define(name, PrimitiveProcedure(fn, name=proc_name))

#################
# Special Forms #
#################

# Each of the following do_xxx_form functions takes the cdr of a special form as
# its first argument---a Scheme list representing a special form without the
# initial identifying symbol (if, lambda, quote, ...). Its second argument is
# the environment in which the form is to be evaluated.

def do_define_form(expressions, env):
    """Evaluate a define form."""
    check_form(expressions, 2)
    target = expressions.first
    if scheme_symbolp(target):
        check_form(expressions, 2, 2)
        # BEGIN PROBLEM 6
        "*** YOUR CODE HERE ***"
        value = scheme_eval(expressions.second.first, env)
        env.define(target, value)
        return target
        # END PROBLEM 6
    elif isinstance(target, Pair) and scheme_symbolp(target.first):
        # BEGIN PROBLEM 10
        "*** YOUR CODE HERE ***"
        formals = target.second
        body = expressions.second
        lambda_procedure = LambdaProcedure(formals, body, env)
        env.define(target.first, lambda_procedure)
        return target.first
        # END PROBLEM 10
    else:
        bad_target = target.first if isinstance(target, Pair) else target
        raise SchemeError('非符号: {0}'.format(bad_target))

def do_quote_form(expressions, env):
    """Evaluate a quote form."""
    check_form(expressions, 1, 1)
    # BEGIN PROBLEM 7
    "*** YOUR CODE HERE ***"
    return expressions.first
    # END PROBLEM 7

def do_begin_form(expressions, env):
    """Evaluate a begin form."""
    check_form(expressions, 1)
    return eval_all(expressions, env)

def do_lambda_form(expressions, env):
    """Evaluate a lambda form."""
    check_form(expressions, 2)
    formals = expressions.first
    check_formals(formals)
    # BEGIN PROBLEM 9
    "*** YOUR CODE HERE ***"
    body = expressions.second
    return LambdaProcedure(formals, body, env)
    # END PROBLEM 9

def do_if_form(expressions, env):
    """Evaluate an if form."""
    check_form(expressions, 2, 3)
    if scheme_truep(scheme_eval(expressions.first, env)):
        return scheme_eval(expressions.second.first, env, True)  # Tail context
    elif len(expressions) == 3:
        return scheme_eval(expressions.second.second.first, env, True)  # Tail context

def do_and_form(expressions, env):
    """Evaluate a (short-circuited) and form."""
    # BEGIN PROBLEM 13
    "*** YOUR CODE HERE ***"
    if expressions is nil:
        return True
    elif expressions.second is nil:  # Tail context
        return scheme_eval(expressions.first, env, True)
    else:
        first_expr = scheme_eval(expressions.first, env)
        if scheme_falsep(first_expr):  # The first expression is False
            return False
        elif scheme_truep(first_expr):  # The first expression is True
            return do_and_form(expressions.second, env)
    # END PROBLEM 13

def do_or_form(expressions, env):
    """Evaluate a (short-circuited) or form."""
    # BEGIN PROBLEM 13
    "*** YOUR CODE HERE ***"
    if expressions is nil:
        return False
    elif expressions.second is nil:  # Tail context
        return scheme_eval(expressions.first, env, True)
    else:
        first_expr = scheme_eval(expressions.first, env)
        if scheme_falsep(first_expr):  # The first expression is False
            return do_or_form(expressions.second, env)
        else:  # The first expression is True
            return first_expr
    # END PROBLEM 13

def do_cond_form(expressions, env):
    """Evaluate a cond form."""
    while expressions is not nil:
        clause = expressions.first
        check_form(clause, 1)
        if clause.first == '否则':
            test = True
            if expressions.second != nil:
                raise SchemeError('"否则"必须放在最后')
        else:
            test = scheme_eval(clause.first, env)
        if scheme_truep(test):
            # BEGIN PROBLEM 14
            "*** YOUR CODE HERE ***"
            if clause.second is nil:  # When the true predicate does not have a corresponding result sub-expression, return the predicate value
                return test
            else:  # When a result sub-expression of a cond case has multiple expressions, evaluate them all and return the value of the last expression
                return eval_all(clause.second, env)
            # END PROBLEM 14
        expressions = expressions.second

def do_let_form(expressions, env):
    """Evaluate a let form."""
    check_form(expressions, 2)
    let_env = make_let_frame(expressions.first, env)
    return eval_all(expressions.second, let_env)

def make_let_frame(bindings, env):
    """Create a child frame of ENV that contains the definitions given in
    BINDINGS. The Scheme list BINDINGS must have the form of a proper bindings
    list in a let expression: each item must be a list containing a symbol
    and a Scheme expression."""
    if not scheme_listp(bindings):
        raise SchemeError('"让"形式内有差劲的绑定列表')
    # BEGIN PROBLEM 15
    "*** YOUR CODE HERE ***"
    formals, vals = nil, nil
    while bindings is not nil:
        check_form(bindings.first, 2, 2)
        val = scheme_eval(bindings.first.second.first, env)  # Evaluate expressions in this env first
        formals = Pair(bindings.first.first, formals)
        check_formals(formals)  # Check formals
        vals = Pair(val, vals)
        bindings = bindings.second
    return env.make_child_frame(formals, vals)
    # END PROBLEM 15

def do_define_macro(expressions, env):
    """Evaluate a define-macro form."""
    # BEGIN Problem 21
    "*** YOUR CODE HERE ***"
    check_form(expressions, 2)
    target = expressions.first
    if isinstance(target, Pair) and scheme_symbolp(target.first):
        formals = target.second
        body = expressions.second
        macro_procedure = MacroProcedure(formals, body, env)
        env.define(target.first, macro_procedure)
        return target.first
    else:
        bad_target = target.first if isinstance(target, Pair) else target
        raise SchemeError('非符号: {0}'.format(bad_target))
    # END Problem 21


SPECIAL_FORMS = {
    '与': do_and_form,
    '始': do_begin_form,
    '条件': do_cond_form,
    '定义': do_define_form,
    '若': do_if_form,
    '入': do_lambda_form,
    '兰姆达': do_lambda_form,
    '让': do_let_form,
    '或': do_or_form,
    '引号': do_quote_form,
    '定义宏': do_define_macro,
}

# Utility methods for checking the structure of Scheme programs

def check_form(expr, min, max=float('inf')):
    """Check EXPR is a proper list whose length is at least MIN and no more
    than MAX (default: no maximum). Raises a SchemeError if this is not the
    case.

    >>> check_form(read_line('(a b)'), 2)
    """
    if not scheme_listp(expr):
        raise SchemeError('糟糕形式的表达式: ' + str(expr))
    length = len(expr)
    if length < min:
        raise SchemeError('形式内的操作范围太少')
    elif length > max:
        raise SchemeError('形式内的操作范围太多')

def check_formals(formals):
    """Check that FORMALS is a valid parameter list, a Scheme list of symbols
    in which each symbol is distinct. Raise a SchemeError if the list of
    formals is not a well-formed list of symbols or if any symbol is repeated.

    >>> check_formals(read_line('(a b c)'))
    """
    symbols = set()
    def check_and_add(symbol):
        if not scheme_symbolp(symbol):
            raise SchemeError('非符号: {0}'.format(symbol))
        if symbol in symbols:
            raise SchemeError('副本符号: {0}'.format(symbol))
        symbols.add(symbol)

    while isinstance(formals, Pair):
        check_and_add(formals.first)
        formals = formals.second

def check_procedure(procedure):
    """Check that PROCEDURE is a valid Scheme procedure."""
    if not scheme_procedurep(procedure):
        raise SchemeError('{0} 是不可调用: {1}'.format(
            type(procedure).__name__.lower(), str(procedure)))

#################
# Dynamic Scope #
#################

class MuProcedure(UserDefinedProcedure):
    """A procedure defined by a mu expression, which has dynamic scope.
     _________________
    < Scheme is cool! >
     -----------------
            \   ^__^
             \  (oo)\_______
                (__)\       )\/\
                    ||----w |
                    ||     ||
    """

    def __init__(self, formals, body):
        """A procedure with formal parameter list FORMALS (a Scheme list) and
        Scheme list BODY as its definition."""
        self.formals = formals
        self.body = body

    # BEGIN PROBLEM 16
    "*** YOUR CODE HERE ***"
    def make_call_frame(self, args, env):
        """Make a frame that binds my formal parameters to ARGS, a Scheme list
        of values, for a dynamically-scoped call evaluated in environment ENV."""
        return env.make_child_frame(self.formals, args)
    # END PROBLEM 16

    def __str__(self):
        return str(Pair('变异', Pair(self.formals, self.body)))

    def __repr__(self):
        return '变异过程({0}, {1})'.format(
            repr(self.formals), repr(self.body))

def do_mu_form(expressions, env):
    """Evaluate a mu form."""
    check_form(expressions, 2)
    formals = expressions.first
    check_formals(formals)
    # BEGIN PROBLEM 16
    "*** YOUR CODE HERE ***"
    body = expressions.second
    return MuProcedure(formals, body)
    # END PROBLEM 16

SPECIAL_FORMS['变异'] = do_mu_form

###########
# Streams #
###########

class Promise:
    """A promise."""
    def __init__(self, expression, env):
        self.expression = expression
        self.env = env

    def evaluate(self):
        if self.expression is not None:
            self.value = scheme_eval(self.expression, self.env.make_child_frame(nil, nil))
            self.expression = None
        return self.value

    def __str__(self):
        return '#[承诺 ({0}强行)]'.format(
                '非 ' if self.expression is not None else '')

def do_delay_form(expressions, env):
    """Evaluates a delay form."""
    check_form(expressions, 1, 1)
    return Promise(expressions.first, env)

def do_cons_stream_form(expressions, env):
    """Evaluate a cons-stream form."""
    check_form(expressions, 2, 2)
    return Pair(scheme_eval(expressions.first, env),
                do_delay_form(expressions.second, env))

SPECIAL_FORMS['构造流'] = do_cons_stream_form
SPECIAL_FORMS['延迟'] = do_delay_form

##################
# Tail Recursion #
##################

class Thunk:
    """An expression EXPR to be evaluated in environment ENV."""
    def __init__(self, expr, env):
        self.expr = expr
        self.env = env

def complete_eval(val):
    """If VAL is an Thunk, returns the result of evaluating its expression
    part. Otherwise, simply returns VAL."""
    if isinstance(val, Thunk):
        return scheme_eval(val.expr, val.env)
    else:
        return val

def scheme_optimized_eval(expr, env, tail=False):
    """Evaluate Scheme expression EXPR in environment ENV. If TAIL, returns an
    Thunk object containing an expression for further evaluation."""
    # Evaluate atoms
    if scheme_symbolp(expr):
        return env.lookup(expr)
    elif self_evaluating(expr):
        return expr

    if tail:
        # BEGIN PROBLEM 20
        "*** YOUR CODE HERE ***"
        return Thunk(expr, env)
        # END PROBLEM 20
    else:
        result = Thunk(expr, env)

    while isinstance(result, Thunk):
        expr, env = result.expr, result.env
        # All non-atomic expressions are lists (combinations)
        if not scheme_listp(expr):
            raise SchemeError('畸形的列表: {0}'.format(str(expr)))
        first, rest = expr.first, expr.second
        if (scheme_symbolp(first) and first in SPECIAL_FORMS):
            result = SPECIAL_FORMS[first](rest, env)
        else:
            # BEGIN PROBLEM 20
            "*** YOUR CODE HERE ***"
            operator = scheme_optimized_eval(first, env)
            check_procedure(operator)
            result = operator.eval_call(rest, env)
            # END PROBLEM 20
    return result

################################################################
# Uncomment the following line to apply tail call optimization #
################################################################
scheme_eval = scheme_optimized_eval


####################
# Extra Procedures #
####################

def scheme_map(fn, lst, env):
    check_type(fn, scheme_procedurep, 0, '映射')
    check_type(lst, scheme_listp, 1, '映射')
    return lst.map(lambda x: complete_eval(fn.apply(Pair(x, nil), env)))

def scheme_filter(fn, lst, env):
    check_type(fn, scheme_procedurep, 0, '滤镜')
    check_type(lst, scheme_listp, 1, '滤镜')
    head, current = nil, nil
    while lst is not nil:
        item, lst = lst.first, lst.second
        if complete_eval(fn.apply(Pair(item, nil), env)):
            if head is nil:
                head = Pair(item, nil)
                current = head
            else:
                current.second = Pair(item, nil)
                current = current.second
    return head

def scheme_reduce(fn, lst, env):
    check_type(fn, scheme_procedurep, 0, '简化')
    check_type(lst, lambda x: x is not nil, 1, '简化')
    check_type(lst, scheme_listp, 1, '简化')
    value, lst = lst.first, lst.second
    while lst is not nil:
        value = complete_eval(fn.apply(scheme_list(value, lst.first), env))
        lst = lst.second
    return value

################
# Input/Output #
################

def read_eval_print_loop(next_line, env, interactive=False, quiet=False,
                         startup=False, load_files=()):
    """Read and evaluate input until an end of file or keyboard interrupt."""
    if startup:
        for filename in load_files:
            scheme_load(filename, True, env)
    while True:
        try:
            src = next_line()
            while src.more_on_line:
                expression = scheme_read(src)
                result = scheme_eval(expression, env)
                if not quiet and result is not None:
                    print(result)
        except (SchemeError, SyntaxError, ValueError, RuntimeError) as err:
            if (isinstance(err, RuntimeError) and
                '超过最大递归深度' not in getattr(err, 'args')[0]):
                raise
            elif isinstance(err, RuntimeError):
                print('错误: 超过最大递归深度')
            else:
                print('错误:', err)
        except KeyboardInterrupt:  # <Control>-C
            if not startup:
                raise
            print()
            print('键盘中断')
            if not interactive:
                return
        except EOFError:  # <Control>-D, etc.
            print()
            return

def scheme_load(*args):
    """Load a Scheme source file. ARGS should be of the form (SYM, ENV) or
    (SYM, QUIET, ENV). The file named SYM is loaded into environment ENV,
    with verbosity determined by QUIET (default true)."""
    if not (2 <= len(args) <= 3):
        expressions = args[:-1]
        raise SchemeError('"载入" 给定不正确数目的实参: '
                          '{0}'.format(len(expressions)))
    sym = args[0]
    quiet = args[1] if len(args) > 2 else True
    env = args[-1]
    if (scheme_stringp(sym)):
        sym = eval(sym)
    check_type(sym, scheme_symbolp, 0, '载入')
    with scheme_open(sym) as infile:
        lines = infile.readlines()
    args = (lines, None) if quiet else (lines,)
    def next_line():
        return buffer_lines(*args)

    read_eval_print_loop(next_line, env, quiet=quiet)

def scheme_open(filename):
    """If either FILENAME or FILENAME.scm is the name of a valid file,
    return a Python file opened to it. Otherwise, raise an error."""
    try:
        return open(filename)
    except IOError as exc:
        if filename.endswith('.scm'):
            raise SchemeError(str(exc))
    try:
        return open(filename + '.scm')
    except IOError as exc:
        raise SchemeError(str(exc))

def create_global_frame():
    """Initialize and return a single-frame environment with built-in names."""
    env = Frame(None)
    env.define('求值',
               PrimitiveProcedure(scheme_eval, True, '求值'))
    env.define('应用',
               PrimitiveProcedure(scheme_apply, True, '应用'))
    env.define('载入',
               PrimitiveProcedure(scheme_load, True, '载入'))
    env.define('过程?',
               PrimitiveProcedure(scheme_procedurep, False, '过程?'))
    env.define('映射',
               PrimitiveProcedure(scheme_map, True, '映射'))
    env.define('滤镜',
               PrimitiveProcedure(scheme_filter, True, '滤镜'))
    env.define('简化',
               PrimitiveProcedure(scheme_reduce, True, '简化'))
    env.define('未定义', None)
    add_primitives(env, PRIMITIVES)
    return env

@main
def run(*argv):
    import argparse
    parser = argparse.ArgumentParser(description='CS 61A Scheme 解释器')
    parser.add_argument('-载入', '-i', action='store_true',
                       help='以交互方式跑文件')
    parser.add_argument('文件', nargs='?',
                        type=argparse.FileType('r'), default=None,
                        help='要跑的Scheme文件')
    args = parser.parse_args()

    next_line = buffer_input
    interactive = True
    load_files = []

    if args.文件 is not None:
        if args.载入:
            load_files.append(getattr(args.文件, '名称'))
        else:
            lines = args.文件.readlines()
            def next_line():
                return buffer_lines(lines)
            interactive = False
            
    print('"使用[控制+Z]退出"')
    read_eval_print_loop(next_line, create_global_frame(), startup=True,
                         interactive=interactive, load_files=load_files)
