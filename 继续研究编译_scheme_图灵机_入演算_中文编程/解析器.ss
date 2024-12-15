;; ====================== ;;
;; 解析                  ;;
;; ====================== ;;
(define (反引号 头 尾)
  (if (and (pair? 尾) (eq? (car 尾) 'quote)
           (pair? (cdr 尾)) (null? (cddr 尾))
           (pair? 头) (eq? (car 头) 'quote)
           (pair? (cdr 头)) (null? (cddr 头)))
      `'(,(cadr 头) . ,(cadr 尾))
      `(cons ,头 ,尾)))

(define (展开反引号 表达式 层级)
  (if (pair? 表达式)
      (let ((头 (car 表达式))
            (尾 (cdr 表达式)))
        (cond
         ((and (eq? 头 'unquote) (pair? 尾) (null? (cdr 尾)))
          (if (zero? 层级)
              (展开宏 (car 尾))
              (反引号头 ''unquote (展开反引号 尾 (- 层级 1)))))
         ((and (eq? 头 'quasiquote) (pair? 尾) (null? (cdr 尾)))
          (反引号头 ''quasiquote
                   (展开反引号 尾 (+ 层级 1))))
         (else
          (反引号头 (展开反引号 头 层级)
                   (展开反引号 尾 层级)))))
      `',表达式))

(define (展开宏 表达式)
  (if (pair? 表达式)
      (let ((标签 (car 表达式)))
        (cond
         ((eq? 标签 'quote) 表达式)
         ((eq? 标签 'quasiquote) (展开反引号 (cadr 表达式) 0))
         ((eq? 标签 'when)
          (展开宏 `(if ,(cadr 表达式) (begin . ,(cddr 表达式)) (begin))))
         ((eq? 标签 'unless)
          (展开宏 `(if ,(cadr 表达式) (begin) (begin . ,(cddr 表达式)))))
         ((eq? 标签 'or)
          (if (null? (cdr 表达式))
              #f
              (if (null? (cddr 表达式))
                  (展开宏 (cadr 表达式))
                  (let ((t (gensym "t")))
                    `(let ((,t ,(展开宏 (cadr 表达式))))
                       (if ,t ,t ,(展开宏 (cons 'or (cddr 表达式)))))))))
         ((eq? 标签 'and)
          (if (null? (cdr 表达式))
              #t
              (if (null? (cddr 表达式))
                  (展开宏 (cadr 表达式))
                  `(if ,(展开宏 (cadr 表达式))
                       ,(展开宏 (cons 'and (cddr 表达式)))
                       #f))))
         ((eq? 标签 'not)
          `(if ,(展开宏 (cadr 表达式)) #f #t))
         ((eq? 标签 'cond)
          (let ((子句 (cadr 表达式))
                (剩余 (cddr 表达式)))
            (if (eq? (car 子句) 'else)
                (展开宏 (cons 'begin (cdr 子句)))
                `(if ,(展开宏 (car 子句))
                     ,(展开宏 (cons 'begin (cdr 子句)))
                     ,(展开宏 (cons 'cond 剩余))))))
         ((eq? 标签 'let*)
          (let ((绑定 (cadr 表达式)))
            (if (null? 绑定)
                (展开宏 (cons 'begin (cddr 表达式)))
                ;; 绑定: ((x e) . 剩余)
                (let ((x (caar 绑定))
                      (e (cadar 绑定))
                      (剩余 (cdr 绑定)))
                  `(let ((,x ,(展开宏 e)))
                     ,(展开宏 `(let* ,剩余 . ,(cddr 表达式))))))))
         (else (map 展开宏 表达式))))
      表达式))

(define (空环境) '())
(define (添加环境 名称 thunk 环境)
  (cons (cons 名称 thunk) 环境))
(define (追加环境 环境1 环境2)
  (append 环境1 环境2))
(define (添加词法 名称 变量 环境)
  (添加环境 名称
             (lambda () `(变量 ,变量))
             环境))
(define (重命名变量 变量)
  (gensym (symbol->string 变量)))
(define (词法引用 变量列表)
  (map (lambda (v) `(变量 ,v)) 变量列表))

;; Eta-扩展用于按值引用顶层和内在的定义。
(define (适配类型 源 目标 表达式)
  (cond
   ((eq? 源 目标) 表达式)
   ((eq? 源 'void) `(seq ,表达式 (const ,(void))))
   ((and (eq? 源 'scm) (eq? 目标 'i32)) `(call %number-值 ,表达式))
   ((and (eq? 源 'i32) (eq? 目标 'scm)) `(call %make-number ,表达式))
   ((and (eq? 源 'bool) (eq? 目标 'scm)) `(if ,表达式 (const #t) (const #f)))
   (else
    (trace-and-error (cons 源 目标) '适配类型 "未处理的情况"))))
(define (添加顶层环境 环境 定义的)
  (let ((标签 (car 定义的)))
    (unless (eq? (car 定义的) 'define)
      (trace-and-error 定义的 '添加顶层环境 "不匹配的顶层声明"))
    (let ((名称 (definition-name 定义的)))
      (if (function-definition? 定义的)
          (添加环境 名称
                   (lambda ()
                     (let ((参数 (map 重命名变量 (function-formals 定义的))))
                       `(lambda ,参数 (call ,名称 . ,(词法引用 参数)))))
                   环境)
          (添加环境 名称
                   (lambda () `(top ,名称))
                   环境)))))

(define (添加类型化原语 环境 标签 prim)
  (let ((返回类型 (car prim))
        (名称 (cadr prim))
        (参数类型 (map car (cddr prim)))
        (参数名称 (map cadr (cddr prim))))
    (cond
     ((memq 名称 '(%make-number %number-值))
      ;; 所有其他原语都可以适配以接收和返回 Scheme 值，
      ;; 但这个原语的目的是在 Scheme 数字和 i32 之间进行转换，
      ;; 因此我们需要避免添加适配器，以免撤销转换！
      (添加顶层环境 环境 `(define (,名称 . ,参数名称))))
     (else
      (添加环境 名称
               (lambda ()
                 (let ((参数 (map 重命名变量 参数名称)))
                   `(lambda ,参数
                      ,(适配类型
                        返回类型 'scm
                        `(,标签 ,名称 .
                               ,(map2 (lambda (表达式 类型)
                                        (适配类型 'scm 类型 表达式))
                                      (词法引用 参数)
                                      参数类型))))))
               环境)))))

(define (添加内在函数 环境 内在函数)
  (添加类型化原语 环境 'icall 内在函数))
(define (添加导入 环境 导入)
  (添加类型化原语 环境 'call 导入))

(define (添加顶层定义 defs 环境)
  (fold-left 添加顶层环境 环境 defs))
(define (添加顶层定义过滤 filter? defs 环境)
  (fold-left (lambda (环境 定义的)
                (if (filter? 定义的)
                    (添加顶层环境 环境 定义的)
                    环境))
             环境
             defs))
(define (添加内在函数列表 内在函数列表 环境)
  (fold-left 添加内在函数 环境 内在函数列表))
(define (添加导入列表 导入列表 环境)
  (fold-left 添加导入 环境 导入列表))
(define (添加词法变量 变量们 重命名 环境)
  (if (null? 变量们)
      环境
      (添加词法 (car 变量们) (car 重命名)
                (添加词法变量 (cdr 变量们) (cdr 重命名) 环境))))
(define (查找名称 名称 环境)
  (let ((点对 (assq 名称 环境)))
    (unless 点对
      (trace-and-error 名称 '查找名称 "未绑定标识符"))
    ((cdr 点对))))

(define (制作导出环境 库)
  (let* ((名称 (cadr 库))
         (主体 (cddr 库))
         (导出 (cdar 主体))
         (定义 (cddr 主体)))
    (cons 名称
          (添加顶层定义过滤
           (lambda (定义的)
             (let ((名称 (definition-name 定义的)))
               (and 名称 (memq 名称 导出))))
           定义
           (空环境)))))

(define (制作let绑定 变量们 值们)
  (map2 (lambda (变量 值) `(,变量 ,值)) 变量们 值们))

(define (解析表达式 表达式 环境)
  (cond
   ((or (null? 表达式) (number? 表达式) (boolean? 表达式) (char? 表达式) (string? 表达式))
    `(const ,表达式))
   ((symbol? 表达式) (查找名称 表达式 环境))
   ((pair? 表达式)
    (let ((op (car 表达式)))
      (cond
       ((eq? op 'quote) `(const ,(cadr 表达式)))
       ((eq? op 'if)
        (let ((t (cadr 表达式))
              (c (caddr 表达式))
              (a (cadddr 表达式)))
          `(if (call eq? ,(解析表达式 t 环境) (const #f))
               ,(解析表达式 a 环境)
               ,(解析表达式 c 环境))))
       ((eq? op 'let)
        (let* ((变量们 (map car (cadr 表达式)))
               (值们 (解析表达式列表 (map cadr (cadr 表达式)) 环境))
               (变量们* (map 重命名变量 变量们))
               (bindings (制作let绑定 变量们* 值们))
               (body (解析主体 (cddr 表达式)
                               (添加词法变量 变量们 变量们* 环境))))
          `(let ,bindings ,body)))
       ((eq? op 'begin)
        (if (null? (cdr 表达式))
            `(const ,(void))
            (解析begin (解析表达式 (cadr 表达式) 环境) (cddr 表达式) 环境)))
       ((eq? op 'lambda)
        (let* ((实参们 (cadr 表达式))
               (实参们* (map 重命名变量 实参们))
               (body (解析主体 (cddr 表达式)
                               (添加词法变量 实参们 实参们* 环境))))
          `(lambda ,实参们* ,body)))
       (else
        `(apply-procedure . ,(解析表达式列表 表达式 环境))))))
   (else
    (trace-and-error 表达式 '解析表达式 "未识别的表达式"))))

(define (解析begin 头 尾 环境)
  ;; 为了使简化器的工作更容易，前端不应制作一个带有尾部 seq 的 seq。
  (cond
   ((null? 尾) 头)
   ((and (pair? (car 尾)) (eq? (caar 尾) 'begin))
    (解析begin 头 (append (cdar 尾) (cdr 尾)) 环境))
   (else
    (解析begin `(seq (drop ,头) ,(解析表达式 (car 尾) 环境))
                (cdr 尾) 环境))))
(define (解析表达式列表 表达式s 环境)
  (map (lambda (表达式) (解析表达式 表达式 环境)) 表达式s))
(define (解析主体 body 环境)
  (unless (pair? body) (error '解析主体 "空主体"))
  (解析begin (解析表达式 (car body) 环境) (cdr body) 环境))

;; 帮助解析定义
;; 定义的 = (define (name 实参们 ...) body)
;;     | (define name body?)
(define (定义名称 定义的)
  (and (pair? 定义的)
       (eq? (car 定义的) 'define)
       (pair? (cdr 定义的))
       (if (pair? (cadr 定义的))
           (caadr 定义的)
           (cadr 定义的))))
(define (函数定义? 定义的)
  (and (pair? 定义的)
       (eq? (car 定义的) 'define)
       (pair? (cdr 定义的))
       (or (pair? (cadr 定义的))
           (and (pair? (cddr 定义的))
                (pair? (caddr 定义的))
                (eq? 'lambda (caaddr 定义的))))))
(define (函数形参 定义的)
  (if (pair? (cadr 定义的))
      (cdadr 定义的)
      (let ((定义的* (caddr 定义的)))
        (unless (eq? (car 定义的*) 'lambda)
          (trace-and-error 定义的 '函数形参 "expected lambda at caaddr"))
        (cadr 定义的*))))
(define (函数主体 定义的)
  (if (pair? (cadr 定义的))
      (cddr 定义的)
      (let ((定义的* (caddr 定义的)))
        (unless (eq? (car 定义的*) 'lambda)
          (trace-and-error 定义的 '函数形参 "expected lambda at caaddr"))
        (cddr 定义的*))))
(define (定义函数 定义的 环境)
  (let* ((名称 (定义名称 定义的))
         (参数 (函数形参 定义的))
         (参数* (map 重命名变量 参数))
         (主体 (解析主体 (函数主体 定义的)
                         (添加词法变量 参数 参数* 环境))))
    `(,(cons 名称 参数*) ,主体)))


(define (定义值 定义的 环境)
  (let* ((名称 (定义名称 定义的))
         ;; 这里利用了最近的定义出现在环境关联列表开头的特性，
         ;; 以检测定义体中的未绑定引用。
         (环境 (memp (lambda (n.d) (eq? 名称 (car n.d))) 环境))
         (主体 (if (null? (cddr 定义的))
                   `(const ,(void))
                   (解析表达式 (caddr 定义的) 环境))))
    `(define ,名称 ,主体)))
(define (解析定义 定义的 环境)
  (if (函数定义? 定义的)
      (定义函数 定义的 环境)
      (定义值 定义的 环境)))
(define (解析定义列表 定义们 环境)
  (map (lambda (定义的) (解析定义 定义的 环境)) 定义们))
(define (计算导入函数 库 导入们)
  (map (lambda (import) `(%wasm-import ,库 . ,import)) 导入们))
(define (解析库 库 导入环境)
  ;; 目前假设它是正确形成的。我们可以稍后进行错误检查。
  (let* ((body (cddr 库))     ;; 跳过库和名称
         (导出 (cdar body)) ;; 导出的函数名称
         (库导入 (cdadr body))
         (defs (cddr body))
         (导入们 runtime-导入们)
         (环境 (添加内在函数列表 intrinsics
                                (添加导入列表 导入们 (空环境))))
         (body-env (添加导入库导入 导入环境 环境))
         (body-env (添加顶层定义 defs body-env)))
    (cons 导出
          (append (计算导入函数 "rt" 导入们)
                  (解析定义列表 defs body-env)))))
(define (解析库列表 libs)
  (let ((导入环境 (map 制作导出环境 libs)))
    (let ((first (解析库 (car libs) 导入环境))
          (导入函数 (fold-right (lambda (库 functions)
                                   (append (cdr (解析库 库 导入环境))
                                           functions))
                                 '()
                                 (cdr libs))))
      (append first 导入函数))))


(define (添加导入 导入们 导入环境 环境)
  (if (null? 导入们)
      环境
      (begin
        (添加导入 (cdr 导入们)
                  导入环境
                  (查找导入 (从导入获取库名称 (car 导入们))
                             导入环境
                             环境)))))
(define (查找导入 库 导入环境 环境)
  (if (null? 导入环境)
      (begin
        (display 库) (newline)
        (error '查找导入 "找不到库"))
      (if (equal? 库 (caar 导入环境))
          (追加环境 (cdar 导入环境) 环境)
          (查找导入 库 (cdr 导入环境) 环境))))

(define (展开引用 表达式)
  (cond
   ;; 字面量自我求值
   ((or (number? 表达式) (boolean? 表达式) (char? 表达式) (string? 表达式) (null? 表达式))
    表达式)
   ((symbol? 表达式)
    `(string->symbol ,(symbol->string 表达式)))
   ((pair? 表达式)
    `(cons ,(展开引用 (car 表达式)) ,(展开引用 (cdr 表达式))))
   (else
    (trace-and-error 表达式 '展开引用 "无效数据"))))
