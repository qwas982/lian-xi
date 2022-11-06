\ -*- 文本 -*-
\	一个有时是最小的FORTH编译器和Linux / i386系统的教程。 -*- asm -*-
\	作者：Richard W.M. Jones <rich@annexia.org> http://annexia.org/forth
\	这是公共领域（见下面的公共领域发布声明）。
\	$Id: jonesforth.f,v 1.18 2009-09-11 08:32:33 rich Exp $
\
\	本教程的第一部分在jonesforth.S.中,得到它http://annexia.org/forth。
\
\	公共领域 --------------------------------
\	我，本作品的版权持有人，特此将其释放到公共领域。这适用于全世界。

\	如果这在法律上是不可能的，我授予任何实体为任何目的使用这一作品的权利。
\   除非法律要求有这样的条件。

\	设定 ----------------------------------

\	让我们先把几件家务事做完。 
\   首先，因为我需要绘制大量的ASCII-art图表来解释概念，
\   最好的方法是使用一个固定宽度字体的窗口，
\   并且至少有这么宽:

\<------------------------------------------------------------------------------------------------------------------------>

\		第二，确保TABS被设置为8个字符。 
\   下面应该是一条垂直线。 
\   如果不是，请整理好你的标签。

\		    |
\	        |
\	    	|

\	第三，我假设你的屏幕至少有50个字符高。

\	FORTH 代码的开始 -------------------------

\
\现在我们已经达到了FORTH系统运行和自我托管的阶段。 
\所有进一步的字都可以写成FORTH本身，包括像IF、THEN、. "等在大多数语言中被认为是相当基本的字。
\

\ 关于代码的一些说明:

\我使用缩进来显示结构。 
\然而，空格的多少对FORTH来说没有任何意义，
\只是你必须在字之间至少使用一个空格字符，
\而且字本身不能包含空格。

\	FORTH是区分大小写的。 使用capslock!

\ 原语字/MOD（DIVMOD）将商和余数都留在栈中。 (在i386上，idivl指令无论如何都会给出两者)。 
\ 现在我们可以用/MOD和其他一些原语来定义 / 和 MOD 。

: / /MOD SWAP DROP ;
: MOD /MOD DROP ;

\ 定义一些字符常量

: '\n' 10 ;
: BL   32 ; \ BL（BLank）是一个标准的FORTH字，表示空格。

\ CR prints a carriage return CR打印一个回车键
: CR '\n' EMIT ;

\ SPACE 打印一个空格
: SPACE BL EMIT ;

\ NEGATE在栈中留下一个数字的负数。
: NEGATE 0 SWAP - ;

\ 布尔运算的标准字。
: TRUE  1 ;
: FALSE 0 ;
: NOT   0= ;

\ LITERAL接收栈上的任何东西并编译LIT <foo>。
: LITERAL IMMEDIATE
	' LIT ,		\ 编译LIT
	,		\ 编译字面本身（来自栈）。
	;

\现在我们可以使用[ 和 ] 来插入在编译时计算的字面。 
\(回想一下，[ 和 ] 是 FORTH 的字，用于切换到立即模式和非立即模式）。
\在定义中，在"...... "是一个常量表达式的地方使用[ ......] LITERAL，
\你希望只计算一次（在编译时，而不是在每次运行字时计算它）。

: ':'
	[		\ 进入立即模式（临时）。
	CHAR :		\ 将数字58（冒号的ASCII码）推到参数栈上
	]		\ 回到编译模式
	LITERAL		\ 将LIT 58编译为':'字的定义
;

\ 还有一些字符常量的定义方式与上面相同。
: ';' [ CHAR ; ] LITERAL ;
: '(' [ CHAR ( ] LITERAL ;
: ')' [ CHAR ) ] LITERAL ;
: '"' [ CHAR " ] LITERAL ;
: 'A' [ CHAR A ] LITERAL ;
: '0' [ CHAR 0 ] LITERAL ;
: '-' [ CHAR - ] LITERAL ;
: '.' [ CHAR . ] LITERAL ;

\ 在编译时，'[COMPILE] word'会编译'word'，如果它本来是IMMEDIATE。
: [COMPILE] IMMEDIATE
	WORD		\ 获取下一个字
	FIND		\ 在字典中找到它
	>CFA		\ 获取其代码字
	,		\ 并将其编译为
;

\RECURSE对正在编译的当前字进行递归调用。

\通常情况下，当一个字正在被编译时，它被标记为HIDDEN，
\所以对同一字的引用是对该字的前一个定义的调用。 
\但是我们仍然可以通过LATEST指针访问我们目前正在编译的字，
\所以我们可以使用它来编译一个递归调用。

: RECURSE IMMEDIATE
	LATEST @	\ LATEST 指向目前正在编译的字
	>CFA		\ 获取代码字
	,		\ 编译它
;

\	控制结构 ---------------------

\到目前为止，我们只定义了非常简单的定义。 
\在我们能更进一步之前，我们确实需要制作一些控制结构，如IF ... THEN和循环。 
\幸运的是，我们可以在FORTH中直接定义任意的控制结构。

\请注意，我在这里定义的控制结构只能在编译后的字中工作。 
\如果你试图在立即模式下使用IF等输入表达式，那么它们将无法工作。
\让这些东西在立即模式下工作是留给读者的一个练习。

\ condition IF true-part THEN rest
\ 编译为: --> condition 0BRANCH OFFSET true-part rest
\ 其中OFFSET是'rest'的偏移量
\ condition IF true-part ELSE false-part THEN
\ 编译为: --> condition 0BRANCH OFFSET true-part BRANCH OFFSET2 false-part rest
\ 其中OFFSET是false-part的偏移，OFFSET2是rest的偏移。

\IF是一个IMMEDIATE字，它编译了0BRANCH，
\后面是一个假动作偏移量，并把0BRANCH的地址放在栈上。 
\之后当我们看到THEN时，我们从栈中弹出该地址，
\计算偏移量，并回填偏移量。

: IF IMMEDIATE
	' 0BRANCH ,	\ 编译0BRANCH
	HERE @		\ 保存栈上的偏移量位置
	0 ,		\ 编译一个假动作偏移量
;

: THEN IMMEDIATE
	DUP
	HERE @ SWAP -	\ 计算出保存在栈中的地址的偏移量
	SWAP !		\ 将偏移量存储在回填的位置
;

: ELSE IMMEDIATE
	' BRANCH ,	\ 明确的分支到刚刚超过false-part位置
	HERE @		\ 保存栈上的偏移量位置
	0 ,		\ 编译一个假动作偏移量
	SWAP		\ 现在回填原来的（IF）偏移量
	DUP		\ 与上面的THEN字相同
	HERE @ SWAP -
	SWAP !
;

\ BEGIN 循环部分 条件 UNTIL
\	-- 编译为: --> 循环部分 条件 0BRANCH OFFSET
\	其中OFFSET指向循环部分
\ 这就像C语言中的do { loop-part } while (condition)。

: BEGIN IMMEDIATE
	HERE @		\ 在栈上保存位置
;

: UNTIL IMMEDIATE
	' 0BRANCH ,	\ 编译0BRANCH
	HERE @ -	\ 计算出保存在栈上的地址的偏移量
	,		\ 在这里编译偏移量
;

\ BEGIN 循环部分 AGAIN
\	-- 编译为: --> 循环部分 BRANCH OFFSET
\	其中OFFSET指向循环部分
\ 换句话说，一个无限循环，只能用EXIT来返回。
: AGAIN IMMEDIATE
	' BRANCH ,	\ 编译BRANCH
	HERE @ -	\ 计算回来的偏移量
	,		\ 在这里编译偏移量
;

\ BEGIN condition WHILE loop-part REPEAT
\	-- 编译为: --> condition 0BRANCH OFFSET2 loop-part BRANCH OFFSET
\	其中 OFFSET 指向条件（起始），OFFSET2 指向整段代码的后面
\  所以这就像C语言中的 while (condition) { loop-part } 循环。
: WHILE IMMEDIATE
	' 0BRANCH ,	\ 编译0BRANCH
	HERE @		\ 保存栈中offset2的位置
	0 ,		\ 编译一个假动作offset2
;

: REPEAT IMMEDIATE
	' BRANCH ,	\ 编译BRANCH
	SWAP		\ 获取原始偏移量（从BEGIN开始）。
	HERE @ - ,	\ 并在BRANCH之后编译它
	DUP
	HERE @ SWAP -	\ 计算 offset2
	SWAP !		\ 并将其回填到原来的位置
;

\UNLESS与IF相同，但 test 内容相反.

\注意 [COMPILE] 的使用。由于 IF 是IMMEDIATE的，我们不希望它在 UNLESS 编译时被执行，
\而是在 UNLESS 运行时被执行（这恰好是使用 UNLESS 的字被编译的时候 -- whew！）。 
\所以我们用[COMPILE]来扭转将IF标记为立即执行的效果。
\当我们想编写自己的控制字时，通常会使用这个技巧，而不必全部用原语0BRANCH和BRANCH来实现，
\而是重复使用更简单的控制字，比如（在这个例子中）IF。

: UNLESS IMMEDIATE
	' NOT ,		\ 编译NOT(反转 test)
	[COMPILE] IF	\ 通过调用正常的IF继续
;

\	注释 -------------------------------------

\FORTH允许（...）作为函数定义的注释。 
\这是由一个叫做（）的IMMEDIATE字来实现的，它只是将输入的字符丢掉，直到碰到相应的）。
: ( IMMEDIATE
	1		\ 通过跟踪深度，允许嵌套的括弧。
	BEGIN
		KEY		\ 读下一个字符
		DUP '(' = IF	\ 开括弧?
			DROP		\ 丢掉开括弧
			1+		\ 深度增加
		ELSE
			')' = IF	\ 关括弧?
				1-		\ 深度减少
			THEN
		THEN
	DUP 0= UNTIL		\ 继续，直到我们到达匹配的关括弧，深度为0
	DROP		\ 丢掉深度计数器
;

(
	从现在开始，我们可以使用 ( ... )作为注释.

	栈备注 ----------------------------------------------------------------------

	在FORTH风格中，我们也可以用( ... -- ... )来显示一个字对参数栈的影响。 
	比如说:

	( n -- )	表示该字从参数栈中消耗一个整数（n）。
	( b a -- c )	表示该字使用了两个整数（a和b，其中a在栈顶）
				并返回一个单一的整数（c）。
	( -- )		表示该字对栈没有影响
)

( 一些更复杂的栈例子，显示了栈的备注. )
: NIP ( x y -- y ) SWAP DROP ;
: TUCK ( x y -- y x y ) SWAP OVER ;
: PICK ( x_u ... x_1 x_0 u -- x_u ... x_1 x_0 x_u )
	1+		( 因为'u'在栈中，所以增加一个 )
	4 *		( 乘以字的大小 )
	DSP@ +		( 加到栈指针上 )
	@    		( 并拾取 )
;

( 有了循环结构，我们现在可以写SPACES，将n个空格写到stdout上. )
: SPACES	( n -- )
	BEGIN
		DUP 0>		( while n > 0 )
	WHILE
		SPACE		( 打印一个空格 )
		1-		( 直到我们倒数至0 )
	REPEAT
	DROP
;

( 操纵BASE的标准字. )
: DECIMAL ( -- ) 10 BASE ! ;
: HEX ( -- ) 16 BASE ! ;

(
	打印号码 ----------------------------------------------------------------------

	FORTH的标准字.（DOT）是非常重要的。 
	它将栈顶的数字取出并打印出来。 
	然而，我首先要实现一些低级别的FORTH字:

	U.R	( u width -- )	打印一个无符号的数字，垫起到一定的宽度。
	U.	( u -- )	它打印出一个无符号的数字
	.R	( n width -- )	它打印一个有符号的数字，垫起到一定的宽度。

	例如:
		-123 6 .R
	将会打印出这些字符:
		<space> <space> - 1 2 3

	换句话说，数字向左填充到一定数量的字符。

	即使宽度更宽广，也会打印出完整的数字，
	这就是我们能够定义普通函数 U. 和 . 的原因
	（我们只是将宽度设置为0，因为知道无论如何都会打印完整的数字）。

	. 和 朋友 的另一个问题是，他们服从变量BASE中的当前基数。
	BASE可以是2到36范围内的任何东西

	当我们定义 .&c 的时候，我们也可以定义 .S ，这是一个有用的调试工具。 
	这个字可以从顶到底打印出当前的栈（非破坏性的）。
)

( 这是U的潜在递归定义。)
: U.		( u -- )
	BASE @ /MOD	( 宽度为 REM quot )
	?DUP IF			( 如果商数<>0，那么 )
		RECURSE		( 打印商数 )
	THEN

	( 打印余数 )
	DUP 10 < IF
		'0'		( 十进制数字 0..9 )
	ELSE
		10 -		( 十六进制及以上的数字 A..Z )
		'A'
	THEN
	+
	EMIT
;

(
	FORTH字 .S 打印出栈的内容。 
	它并不改变栈的内容。
	在调试时非常有用。
)

: .S		( -- )
	DSP@		( 获取当前栈指针 )
	BEGIN
		DUP S0 @ <
	WHILE
		DUP @ U.	( 打印栈元素 )
		SPACE
		4+		( 上移 )
	REPEAT
	DROP
;

( 这个字返回一个无符号数字在当前基数下的宽度（以字符为单位）)
: UWIDTH	( u -- 宽度 )
	BASE @ /	( rem quot )
	?DUP IF		( 如果商数<>0，那么 )
		RECURSE 1+	( 返回 1 + 递归 调用 )
	ELSE
		1		( 返回 1 )
	THEN
;

: U.R		( u 宽度 -- )
	SWAP		( 宽度 u )
	DUP		( 宽度 u u )
	UWIDTH		( 宽度 u u宽度 )
	ROT		( u u宽度 宽度 )
	SWAP -		( u 宽度-u宽度 )
	( 在这一点上，如果要求的宽度更窄，我们在栈上会有一个负数。
	否则栈上的数字就是要打印的空格数。 
	但无论如何，SPACES不会打印负数的空格，所以现在调用SPACES是安全的 ... )
	SPACES
	( ... 然后再调用U.的潜在实现。 )
	U.
;

(
	.R 打印一个有符号的数字，填充到一定的宽度。 
	我们不能只打印符号并调用U.R，因为我们希望符号在数字的旁边（'-123'而不是'- 123'）。
)
: .R		( n 宽度 -- )
	SWAP		( 宽度 n )
	DUP 0< IF
		NEGATE		( 宽度 u )
		1		( 保存一个标志，以记住它是 负数 | 宽度 n 1 )
		SWAP		( 宽度 1 u )
		ROT		( 1 u 宽度 )
		1-		( 1 u 宽度-1 )
	ELSE
		0		( 宽度 u 0 )
		SWAP		( 宽度 0 u )
		ROT		( 0 u 宽度 )
	THEN
	SWAP		( flag 宽度 u )
	DUP		( flag 宽度 u u )
	UWIDTH		( flag 宽度 u u宽度 )
	ROT		( flag u u宽度 宽度 )
	SWAP -		( flag u 宽度-u宽度 )

	SPACES		( flag u )
	SWAP		( u flag )

	IF			( 是负数的吗？ 打印"-"字符 )
		'-' EMIT
	THEN

	U.
;

( 最后，我们可以用 .R 来定义字 . ，并在后面加一个空格。 )
: . 0 .R SPACE ;

( 真实的 U. ，注意后面的空格. )
: U. U. SPACE ;

( ? 拾取一个地址的整数并打印出来。)
: ? ( addr -- ) @ . ;

( c a b WITHIN 如果a <= c且c < b，则返回true。 )
(  或不含ifs的定义: OVER - >R - R>  U<  )
: WITHIN
	-ROT		( b c a )
	OVER		( b c a c )
	<= IF
		> IF		( b c -- )
			TRUE
		ELSE
			FALSE
		THEN
	ELSE
		2DROP		( b c -- )
		FALSE
	THEN
;

( DEPTH 返回栈的深度。 )
: DEPTH		( -- n )
	S0 @ DSP@ -
	4-			( 调整，因为当我们推DSP时，S0在栈上。 )
;

(
	ALIGNED接收一个地址并将其四舍五入（对齐）到下一个4字节边界。
)
: ALIGNED	( addr -- addr )
	3 + 3 INVERT AND	( (addr+3) & ~3 )
;

(
	ALIGN使HERE指针对齐，所以下一个被附加的字将被正确对齐。
)
: ALIGN HERE @ ALIGNED HERE ! ;

(
	字符串 ----------------------------------------------------------------------

	S "string "在FORTH中用来定义字符串。 
它把字符串的地址和它的长度留在栈中，（长度在栈顶）。 
S "后面的空格是FORTH字之间的正常空格，不是字符串的一部分。

	这个定义很棘手，因为它必须根据我们是在编译还是在立即模式下做不同的事情。 
(因此这个字被标记为IMMEDIATE，所以它可以检测到这一点并做不同的事情）。

	在编译模式下，我们将LITSTRING <string length> <string rounded up 4 bytes>附加到当前字。 
原语的LITSTRING在执行当前字的时候会做正确的事情。

	在立即模式下，没有一个特别好的地方来放置字符串，但在这种情况下，我们把字符串放在这里（但我们_不_改变这里）。 
这是一个临时位置，可能很快就会被覆盖。
)

( C，将一个字节附加到当前编译的字上。 )
: C,
	HERE @ C!	( 在编译后的镜像中存储该字符 )
	1 HERE +!	( 将HERE的指针增加1个字节 )
;

: S" IMMEDIATE		( -- addr len )
	STATE @ IF	( 编译? )
		' LITSTRING ,	( 编译 LITSTRING )
		HERE @		( 在栈上保存长度字的地址 )
		0 ,		( 假动作长度 - 我们还不知道它是什么 )
		BEGIN
			KEY 		( 获取字符串的下一个字符 )
			DUP '"' <>
		WHILE
			C,		( 拷贝字符 )
		REPEAT
		DROP		( 丢掉末尾的双引号字符 )
		DUP		( 获取长度字的保存地址 )
		HERE @ SWAP -	( 计算出长度 )
		4-		( 减去4（因为我们是从长度字的开始测量的）。)
		SWAP !		( 并回填长度位置 )
		ALIGN		( 对于剩余的代码，四舍五入到下一个4字节的倍数。 )
	ELSE		( 立即模式 )
		HERE @		( 获取临时空间的起始地址 )
		BEGIN
			KEY
			DUP '"' <>
		WHILE
			OVER C!		( 保存下一个字符 )
			1+		( 递增地址 )
		REPEAT
		DROP		( 丢掉最后的 " 字符 )
		HERE @ -	( 计算出长度 )
		HERE @		( 推开始的地址 )
		SWAP 		( addr len )
	THEN
;

(
	. "是FORTH中的打印字符串操作符。 例如：." 要打印的东西" 操作符后面的空格是字与字之间需要的普通空格，不属于打印的内容。

	在立即模式下，我们只是不断地读取字符并打印它们，直到我们到达下一个双引号。

	在编译模式下，我们使用 S" 来存储字符串，然后在之后添加TELL。:
		LITSTRING <string length> <string rounded up to 4 bytes> TELL

	值得注意的是，使用[COMPILE]将对立即字 S" 的调用变成对该字的编译。 
	它把它编译到 ." 的定义中，而不是编译到这个字运行时的定义中（对你来说够复杂了？）
)

: ." IMMEDIATE		( -- )
	STATE @ IF	( 编译? )
		[COMPILE] S"	( 读取字符串，并编译LITSTRING，等等。 )
		' TELL ,	( 编译最终的TELL )
	ELSE
		( 在立即模式下，只需读取字符并打印，直到我们到达结束的双引号。 )
		BEGIN
			KEY
			DUP '"' = IF
				DROP	( 丢掉双引号字符 )
				EXIT	( 从该函数返回 )
			THEN
			EMIT
		AGAIN
	THEN
;

(
	常量与变量 ----------------------------------------------------------------------

	在FORTH中，全局常量和变量的定义是这样的:

	10 CONSTANT TEN		当TEN被执行时，它将整数10留在栈上。
	VARIABLE VAR		当VAR被执行时，它将VAR的地址留在栈上。

	常量可以被读取但不能被写入，例如:

	TEN . CR		prints 10

	你可以通过执行以下操作来读取一个变量（在本例中称为VAR）:

	VAR @			在栈中留下VAR的值
	VAR @ . CR		打印VAR的值
	VAR ? CR		与上述相同，因为 ? 与 @ 相同。

	并通过以下操作更新该变量:

	20 VAR !		sets VAR to 20

	请注意，变量是未初始化的（但请看后面的VALUE，它提供了初始化的变量，句法稍显简单）。

	我们如何定义CONSTANT和VARIABLE这两个字？

	诀窍是为变量本身定义一个新字（例如，如果变量被称为 "VAR"，那么我们将定义一个新字，称为VAR）。 
	这很容易做到，因为我们通过CREATE字暴露了字典条目的创建（上面是对 ：的定义的一部分）。
	调用WORD [TEN] CREATE（其中[TEN]表示 "TEN "是输入的下一个字）会留下字典条目:

				   +--- HERE
				   |
				   V
	+---------+---+---+---+---+
	| LINK    | 3 | T | E | N |
	+---------+---+---+---+---+
                   len

	对于CONSTANT，我们可以继续添加DOCOL（代码字），
	然后是LIT，后面是常量本身，然后是EXIT，形成一个小的单词定义，返回常量:

	+---------+---+---+---+---+------------+------------+------------+------------+
	| LINK    | 3 | T | E | N | DOCOL      | LIT        | 10         | EXIT       |
	+---------+---+---+---+---+------------+------------+------------+------------+
                   len              codeword

	请注意，这个字的定义与你写的完全相同，
	如果你写的是 : TEN 10 ;

	请阅读下面代码的人注意:
	DOCOL是我们在汇编部分定义的一个常量字，它返回同名的汇编符号的值。
)

: CONSTANT
	WORD		( 获取名称(名称跟随CONSTANT) )
	CREATE		( 做字典条目 )
	DOCOL ,		( 追加DOCOL(这个字的代码字字段) )
	' LIT ,		( 追加代码字LIT )
	,		( 在栈顶追加值 )
	' EXIT ,	( 追加代码字EXIT )
;

(
	VARIABLE有点难,因为我们需要一个地方来放置这个变量。 
用户内存(由HERE指向的内存区域,我们之前只是在那里存储了新的字定义)没有什么特别的地方。 
我们可以从这个内存区域中切下一些位来存储任何我们想要的东西，
所以VARIABLE的一个可能的定义可能是这样的:

	   +--------------------------------------------------------------+
	   |								  |
	   V								  |
	+---------+---------+---+---+---+---+------------+------------+---|--------+------------+
	| <var>   | LINK    | 3 | V | A | R | DOCOL      | LIT        | <addr var> | EXIT       |
	+---------+---------+---+---+---+---+------------+------------+------------+------------+
        		     len              codeword

	其中<var>是存储变量的地方，而<addr var>则指向它。

	为了使其更加通用，让我们定义几个字，我们可以用它们来从用户内存中分配任意的内存。

	首先是ALLOT，其中n ALLOT分配了n个字节的内存。 
(注意在调用这个时，最好确保n是4的倍数，或者至少在下次编译字时，HERE已经被留成4的倍数）。
)

: ALLOT		( n -- addr )
	HERE @ SWAP	( here n )
	HERE +!		( adds n to HERE, 在这之后，HERE的旧值仍在栈上。 )
;

(
	第二，cells。 在FORTH中，"n CELLS ALLOT "这句话的意思是分配n个整数，不管这个机器架构上整数的自然大小。 
在这台32位机器上，CELLS只是将栈顶乘以4。
)
: CELLS ( n -- n ) 4 * ;

(
	因此，现在我们可以用与上面的CONSTANT相同的方式轻松定义VARIABLE。 
请参考上图，看看这样创建的字是什么样子的。
)
: VARIABLE
	1 CELLS ALLOT	( 分配1个cell的内存，把指针推到这个内存上 )
	WORD CREATE	( 编写字典中的条目（名称跟随VARIABLE） )
	DOCOL ,		( 追加DOCOL（这个字的代码字字段）)
	' LIT ,		( 追加代码字LIT )
	,		( 将指针追加到新的内存中 )
	' EXIT ,	( 追加代码字EXIT )
;

(
	值 ----------------------------------------------------------------------

	VALUEs 与 VARIABLEs 类似，但句法更简单。 
一般来说，当你想要一个经常被读取，而不经常被写入的变量时，你会使用它们。

	20 VALUE VAL 	创建VAL，初始值为20
	VAL		直接将值（20）推到栈上。
	30 TO VAL	更新VAL，将其设置为30
	VAL		直接将值（30）推到栈上

	请注意，'VAL'本身并不返回值的地址，而是返回值本身，这使得值的使用比变量更简单、更明显（没有通过'@'的转接）。
其代价是一个更复杂的实现，尽管有这样的复杂性，但在运行时没有性能上的损失。

	一个天真的 "TO "的实现将是相当缓慢的，每次都要进行字典搜索。
但是因为这是FORTH，我们可以完全控制编译器，所以我们可以更有效地编译TO，
	把:
		TO VAL
	变成:
		LIT <addr> !
	并计算出 <addr> (值的地址)在编译的时候.

	现在，这就是聪明之处。 我们将像这样编译我们的值:

	+---------+---+---+---+---+------------+------------+------------+------------+
	| LINK    | 3 | V | A | L | DOCOL      | LIT        | <value>    | EXIT       |
	+---------+---+---+---+---+------------+------------+------------+------------+
                   len              codeword

	其中<value>是实际值本身。 
注意，当VAL执行时，它将把值推到栈上，这就是我们想要的结果.

	但TO将用什么来表示地址<addr>？ 为什么当然是一个指向<value>的指针？:

		code compiled	- - - - --+------------+------------+------------+-- - - - -
		by TO VAL		  | LIT        | <addr>     | !          |
				- - - - --+------------+-----|------+------------+-- - - - -
							     |
							     V
	+---------+---+---+---+---+------------+------------+------------+------------+
	| LINK    | 3 | V | A | L | DOCOL      | LIT        | <value>    | EXIT       |
	+---------+---+---+---+---+------------+------------+------------+------------+
                   len              codeword

	换句话说，这是一种自我修改的代码。

	(请注意那些想修改这个FORTH以增加内联的人：以这种方式定义的值不能被内联).
)
: VALUE		( n -- )
	WORD CREATE	( 编写字典条目（名称跟随VALUE）。 )
	DOCOL ,		( 追加DOCOL )
	' LIT ,		( 追加 the 代码字 LIT )
	,		( 追加初始值 )
	' EXIT ,	( 追加 the 代码字 EXIT )
;

: TO IMMEDIATE	( n -- )
	WORD		( 获取值的名称 )
	FIND		( 查字典吧 )
	>DFA		( 获取第一个数据字段的指针（"LIT"） )
	4+		( 递增到指向值 )
	STATE @ IF	( 编译吗？)
		' LIT ,		( 编译 LIT )
		,		( 编译该值的地址 )
		' ! ,		( 编译 ! )
	ELSE		( 立即模式 )
		!		( 直接更新它 )
	THEN
;

( x +TO VAL 将x添加到VAL中 )
: +TO IMMEDIATE
	WORD		( 获取值的名称 )
	FIND		( 查字典吧 )
	>DFA		( 获取第一个数据字段的指针（"LIT"） )
	4+		( 递增到指向值 )
	STATE @ IF	( 编译吗？ )
		' LIT ,		( 编译 LIT )
		,		( 编译该值的地址 )
		' +! ,		( 编译 +! )
	ELSE		( 立即模式 )
		+!		( 直接更新它 )
	THEN
;

(
	打印字典 ----------------------------------------------------------------------

	ID.接收一个字典条目的地址，并打印出该字的名称。

	比如说。LATEST @ ID.将打印最后一个被定义的字的名称。
)
: ID.
	4+		( 跳过链接指针 )
	DUP C@		( 获取标志/长度字节 )
	F_LENMASK AND	( 屏蔽掉标志--只想要长度 )

	BEGIN
		DUP 0>		( 长度 > 0? )
	WHILE
		SWAP 1+		( addr len -- len addr+1 )
		DUP C@		( len addr -- len addr char | 获取下一个字符)
		EMIT		( len addr char -- len addr | 并打印它)
		SWAP 1-		( len addr -- addr len-1    | 从长度上减去1 )
	REPEAT
	2DROP		( len addr -- )
;

(
	'WORD word FIND ?HIDDEN'如果'word'被标记为隐藏，则返回真。

	'WORD word FIND ?IMMEDIATE'如果'WORD'被标记为立即，则返回真。
)
: ?HIDDEN
	4+		( 跳过链接指针 )
	C@		( 获取标志/长度字节 )
	F_HIDDEN AND	( 屏蔽F_HIDDEN标志并返回它（作为一个真值） )
;
: ?IMMEDIATE
	4+		( 跳过链接指针 )
	C@		( 获取标志/长度字节 )
	F_IMMED AND	( 屏蔽F_IMMED标志并返回它（作为一个真值） )
;

(
	WORDS打印字典中定义的所有字，从最近定义的字开始。但是它不打印隐藏的字。

	该实现只是使用链接指针从LATEST向后迭代。
)
: WORDS
	LATEST @	( 从LATEST字典条目开始 )
	BEGIN
		?DUP		( 当链接指针不为空指针时 )
	WHILE
		DUP ?HIDDEN NOT IF	( 忽略隐藏的文字 )
			DUP ID.		( 但如果没有隐藏，则打印出字 )
			SPACE
		THEN
		@		( 解除对链接指针的引用 - 转到上一个字 )
	REPEAT
	CR
;

(
	遗忘 ----------------------------------------------------------------------

	到目前为止，我们只分配了字和内存。 FORTH提供了一个相当原始的方法来取消分配。

	FORGET word'从字典中删除了'word'的定义，以及在它之后定义的所有内容，包括之后分配的任何变量和其他内存。

	实现起来非常简单--我们查找字（会返回字典条目地址）。 
然后我们设置 HERE 指向该地址，因此实际上所有未来的分配和定义都将覆盖从该字开始的内存。 
我们还需要设置LATEST来指向前一个字。

	请注意，你不能忘记内建字（好吧，你可以尝试，但可能会导致故障）。

	XXX：因为我们写VARIABLE是为了将变量存储在字之前分配的内存中，在目前的实现中，VARIABLE FOO FORGET FOO将泄露1个cell 的内存。
)
: FORGET
	WORD FIND	( 找到这个字，获得字典中的条目地址 )
	DUP @ LATEST !	( 设置LATEST指向前一个字 )
	HERE !		( 并将HERE与字典中的地址存储在一起 )
;

(
	转储 ----------------------------------------------------------------------

	DUMP用于转储内存中的内容，采用'传统' hexdump格式。

	注意，DUMP的参数(地址、长度)与WORD和S"等字符串字兼容。

	您可以通过执行以下操作来转储您定义的最后一个字的原始代码:

		LATEST @ 128 DUMP
)
: DUMP		( addr len -- )
	BASE @ -ROT		( 将当前的BASE保存在栈底 )
	HEX			( 并切换到十六进制模式 )

	BEGIN
		?DUP		( while len > 0 )
	WHILE
		OVER 8 U.R	( 打印地址 )
		SPACE

		( 在这一行最多打印16个字 )
		2DUP		( addr len addr len )
		1- 15 AND 1+	( addr len addr linelen )
		BEGIN
			?DUP		( while linelen > 0 )
		WHILE
			SWAP		( addr len linelen addr )
			DUP C@		( addr len linelen addr byte )
			2 .R SPACE	( 打印字节 )
			1+ SWAP 1-	( addr len linelen addr -- addr len addr+1 linelen-1 )
		REPEAT
		DROP		( addr len )

		( 打印ASCII码的等价物 )
		2DUP 1- 15 AND 1+ ( addr len addr linelen )
		BEGIN
			?DUP		( while linelen > 0)
		WHILE
			SWAP		( addr len linelen addr )
			DUP C@		( addr len linelen addr byte )
			DUP 32 128 WITHIN IF	( 32 <= c < 128? )
				EMIT
			ELSE
				DROP '.' EMIT
			THEN
			1+ SWAP 1-	( addr len linelen addr -- addr len addr+1 linelen-1 )
		REPEAT
		DROP		( addr len )
		CR

		DUP 1- 15 AND 1+ ( addr len linelen )
		TUCK		( addr linelen len linelen )
		-		( addr linelen len-linelen )
		>R + R>		( addr+linelen len-linelen )
	REPEAT

	DROP			( 恢复栈 )
	BASE !			( 恢复保存的 BASE )
;

(
	情况 ----------------------------------------------------------------------

	CASE……ENDCASE是我们在FORTH中执行switch语句的方式。
这方面没有普遍一致的句法，所以我使用ISO标准FORTH (ANS-FORTH)强制使用的句法。

		( 栈上的某个值 )
		CASE
		test1 OF ... ENDOF
		test2 OF ... ENDOF
		testn OF ... ENDOF
		... ( default case 默认情况 )
		ENDCASE

	CASE语句通过对比栈中的值与test1, test2, ..., testn 是否相等来测试它，并在OF ... ENDOF中执行匹配的代码。
如果没有一个测试值匹配，那么就执行默认情况。 
在默认情况下的......里面，值仍然在栈顶（它被ENDCASE隐含地DROP了）。 
当ENDOF被执行时，它在ENDCASE之后跳转（即-没有 "落空"，也不需要像C语言那样的break语句）。

	默认情况可以被省略。 
事实上，测试也可以省略，这样你就只有一个默认情况，尽管这可能不是很有用。

	一个例子（假设'q'等等 是将字母的ASCII值推到栈上的字）:

		0 VALUE QUIT
		0 VALUE SLEEP
		KEY CASE
			'q' OF 1 TO QUIT ENDOF
			's' OF 1 TO SLEEP ENDOF
			( default case: )
			." Sorry, I didn't understand key <" DUP EMIT ." >, try again." CR
		ENDCASE

	(在某些版本的FORTH中，支持更高级的测试，如范围等。其他版本的FORTH需要你写OTHERWISE来表示默认情况。
正如我上面所说，这个FORTH试图遵循ANS FORTH的标准）。

	CASE...ENDCASE的实现有点非同小可。 
我是按照这里的实现来做的:
	http://www.uni-giessen.de/faq/archiv/forthfaq.case_endcase/msg00000.html

	一般的计划是将代码编译为一系列的IF语句:

	CASE				(在立即模式的参数栈中推0)
	test1 OF ... ENDOF		test1 OVER = IF DROP ... ELSE
	test2 OF ... ENDOF		test2 OVER = IF DROP ... ELSE
	testn OF ... ENDOF		testn OVER = IF DROP ... ELSE
	... ( default case )		...
	ENDCASE				DROP THEN [THEN [THEN ...]]

	CASE语句将0推到立即模式的参数栈中，
	这个数字被用来计算当我们到达ENDCASE时需要多少个THEN语句，以便每个IF都有一个匹配的THEN。 
	这个计数是隐式完成的。 
	如果你还记得上面IF的实现，每个IF都会在立即模式栈中推送一个代码地址，
	这些地址是非零的，所以当我们到达ENDCASE时，栈中包含了一些非零的数字，后面是一个零。 
	非零的数量就是IF被调用了多少次，所以我们需要用THEN来匹配它的次数。

	这段代码使用了[COMPILE]，这样我们在编译下面的字时，
	就会编译对IF、ELSE、THEN的调用，而不是实际调用它们。

	就像我们所有的控制结构一样，
	它们只在字的定义中起作用，而不是在立即模式下。
)

: CASE IMMEDIATE
	0		( 推0来标记栈底 )
;

: OF IMMEDIATE
	' OVER ,	( 编译OVER )
	' = ,		( 编译 = )
	[COMPILE] IF	( 编译 IF )
	' DROP ,  	( 编译 DROP )
;

: ENDOF IMMEDIATE
	[COMPILE] ELSE	( ENDOF与ELSE相同 )
;

: ENDCASE IMMEDIATE
	' DROP ,	( 编译 DROP )

	( 继续编译THEN，直到我们到达我们的零标记 )
	BEGIN
		?DUP
	WHILE
		[COMPILE] THEN
	REPEAT
;

(
	反编译 ----------------------------------------------------------------------

	CFA>是>CFA的对立面。 它接收一个代码字并试图找到匹配的字典定义。 
(事实上，它对任何进入一个字的指针都有效，而不仅仅是代码字指针，这是做栈跟踪所需要的）。

	在这个FORTH中，这就不那么容易了。 
事实上，我们必须在字典中搜索，因为我们没有一个方便的后置指针（在其他版本的FORTH中经常如此）。 
由于这种搜索，CFA>不应该在对性能要求很高的时候使用，所以它只用于调试工具，如反编译器和打印栈跟踪。

	若未找到匹配，这个字会返回0。
)
: CFA>
	LATEST @	( 从LATEST 字典条目开始 )
	BEGIN
		?DUP		( 当链接指针不为空指针时 )
	WHILE
		2DUP SWAP	( cfa curr curr cfa )
		< IF		( 当前的字典条目 < cfa? )
			NIP		( 将当前的字典条目留在栈上 )
			EXIT
		THEN
		@		( 跟随链接指针返回 )
	REPEAT
	DROP		( 恢复栈 )
	0		( 对不起，没有找到 )
;

(
	SEE反编译一个FORTH字.

	我们搜索该字的字典条目，然后再搜索下一个字（实际上就是编译后的字的结尾）。 
	这就产生了两个指针:

	+---------+---+---+---+---+------------+------------+------------+------------+
	| LINK    | 3 | T | E | N | DOCOL      | LIT        | 10         | EXIT       |
	+---------+---+---+---+---+------------+------------+------------+------------+
	 ^									       ^
	 |									       |
	Start of word							      End of word

	有了这些信息，我们就可以对这个字进行反编译了。 
我们需要识别 "元字"，如LIT、LITSTRING、BRANCH等，并分别处理这些字.
)
: SEE
	WORD FIND	( 找到要反编译的字典条目 )

	( 现在我们再次搜索，寻找字典中的下一个字。 
这就给我们提供了我们要反编译的字的长度。 (嗯，大部分情况下是这样）。 )
	HERE @		( 最后一个编译字的结束地址 )
	LATEST @	( word last curr )
	BEGIN
		2 PICK		( word last curr word )
		OVER		( word last curr word curr )
		<>		( word last curr word<>curr? )
	WHILE			( word last curr )
		NIP		( word curr )
		DUP @		( word curr prev (即成为: word last curr) )
	REPEAT

	DROP		( 在这一点上，栈是：字首字尾 )
	SWAP		( 字的末尾，字的开头 )

	( 用：NAME [IMMEDIATE] 开始定义。 )
	':' EMIT SPACE DUP ID. SPACE
	DUP ?IMMEDIATE IF ." IMMEDIATE " THEN

	>DFA		( 获取数据地址，即DOCOL之后的点|字尾数据首 )

	( 现在我们开始反编译，直到我们碰到字尾。 )
	BEGIN		( 末端开始 )
		2DUP >
	WHILE
		DUP @		( 结束时开始的代码字 )

		CASE
		' LIT OF		( 它是LIT吗？ ? )
			4 + DUP @		( 获取下一个字，它是整数常数 )
			.			( 并打印它 )
		ENDOF
		' LITSTRING OF		( 它是LITSTRING吗 ? )
			[ CHAR S ] LITERAL EMIT '"' EMIT SPACE ( 打印 S"<space> )
			4 + DUP @		( 获取长度字 )
			SWAP 4 + SWAP		( 末端 开始+4 长度 )
			2DUP TELL		( 打印该字符串 )
			'"' EMIT SPACE		( 用最后的引号来结束这个字符串 )
			+ ALIGNED		( 末端 开始+4+len，已对齐 )
			4 -			( 因为我们要在下面增加4个 )
		ENDOF
		' 0BRANCH OF		( 是0BRANCH吗 ? )
			." 0BRANCH ( "
			4 + DUP @		( 打印偏移量 )
			.
			." ) "
		ENDOF
		' BRANCH OF		( 是否是BRANCH ? )
			." BRANCH ( "
			4 + DUP @		( 打印偏移量 )
			.
			." ) "
		ENDOF
		' ' OF			( 是 '（TICK）吗 ? )
			[ CHAR ' ] LITERAL EMIT SPACE
			4 + DUP @		( 获取下一个代码字 )
			CFA>			( 并强制将其作为字典条目打印出来 )
			ID. SPACE
		ENDOF
		' EXIT OF		( 是EXIT吗？ )
			( 我们希望最后一个字是EXIT，如果是的话，我们就不打印它，因为EXIT通常是由 ; 暗示的。
			 EXIT也可能出现在字的中间，这时需要打印出来。 )
			2DUP			( end start end start )
			4 +			( end start end start+4 )
			<> IF			( end start | 我们还没有到最后 )
				." EXIT "
			THEN
		ENDOF
					( default case: 默认情况: )
			DUP			( 在默认情况下，我们总是需要在使用前DUP一下 )
			CFA>			( 查阅代码字，获取字典中的条目 )
			ID. SPACE		( 并打印它 )
		ENDCASE

		4 +		( end start+4 )
	REPEAT

	';' EMIT CR

	2DROP		( 恢复栈 )
;

(
	执行牌 ----------------------------------------------------------------------

	标准FORTH定义了一个叫做 "执行牌"（或 "xt"）的概念，与C语言中的函数指针非常相似。 
我们将执行牌映射到一个代码字地址。

			DOUBLE的执行牌是这个代码字的地址。
						    |
						    V
	+---------+---+---+---+---+---+---+---+---+------------+------------+------------+------------+
	| LINK    | 6 | D | O | U | B | L | E | 0 | DOCOL      | DUP        | +          | EXIT       |
	+---------+---+---+---+---+---+---+---+---+------------+------------+------------+------------+
                   len                         pad  codeword					       ^

	有一个用于执行牌的汇编原语，EXECUTE ( xt -- ) ，它可以运行这些牌.

	你可以用>CFA为一个现有的字做一个执行牌，即：WORD [foo] FIND >CFA会把foo的xt推到栈中，其中foo是输入的下一个字。 
所以一个非常慢的运行DOUBLE的方法可能是:

		: DOUBLE DUP + ;
		: SLOW WORD FIND >CFA EXECUTE ;
		5 SLOW DOUBLE . CR	\ 打印 10

	我们还提供了一个更简单、更快速的方法来获取任何字FOO的执行牌:

		['] FOO

	(读者练习：（1）['] FOO 和 ' FOO 之间有什么不同？ 
		(2) ' 、 ['] 和 LIT 之间有什么关系？)

	更有用的是定义匿名字 和/或 将xt赋给变量。

	要定义一个匿名字（并把它的xt推到栈上），使用 :NONAME ... ;
	如本例中:

		:NONAME ." anon word was called" CR ;	\ 将xt推到栈上
		DUP EXECUTE EXECUTE			\ 执行了两次Anon字

	栈参数按预期工作:

		:NONAME ." called with parameter " . CR ;
		DUP
		10 SWAP EXECUTE		\ 打印 '用参数10调用'
		20 SWAP EXECUTE		\ 打印 '用参数20调用'

	请注意，上面的代码有一个内存泄漏：匿名字仍然被编译到数据段中，所以即使你失去了对xt的跟踪，这个字仍然继续占用内存。 
	追踪xt从而避免内存泄漏的一个好方法是将其赋给一个CONSTANT、VARIABLE或VALUE:

		0 VALUE ANON
		:NONAME ." anon word was called" CR ; TO ANON
		ANON EXECUTE
		ANON EXECUTE

	 :NONAME 的另一个用途是创建一个可以快速调用的函数数组（想想：快速切换语句）。 
	这个例子是根据ANS FORTH标准改编的:

		10 CELLS ALLOT CONSTANT CMD-TABLE
		: SET-CMD CELLS CMD-TABLE + ! ;
		: CALL-CMD CELLS CMD-TABLE + @ EXECUTE ;

		:NONAME ." alternate 0 was called" CR ;	 0 SET-CMD
		:NONAME ." alternate 1 was called" CR ;	 1 SET-CMD
			\ etc...
		:NONAME ." alternate 9 was called" CR ;	 9 SET-CMD

		0 CALL-CMD
		1 CALL-CMD
)
: :NONAME
	0 0 CREATE	( 创建一个没有名称的字--我们需要一个字典头，因为 ; 期望它。 )
	HERE @		( 当前的HERE值是代码字的地址，即xt )
	DOCOL ,		( 编译DOCOL (the 代码字) )
	]		( 进入编译模式 )
;

: ['] IMMEDIATE
	' LIT ,		( 编译 LIT )
;

(
	异常 ----------------------------------------------------------------------

	令人惊讶的是，异常可以直接在FORTH中实现，事实上相当容易。

	一般的用法如下:

		: FOO ( n -- ) THROW ;

		: TEST-EXCEPTIONS
			25 ['] FOO CATCH	\ 执行25个FOO，捕捉任何异常
			?DUP IF
				." called FOO and it threw exception number: "
				. CR
				DROP		\ 我们必须放弃FOO(25)的论证。
			THEN
		;
		\ 打印：调用FOO，它抛出了异常号：25

	CATCH运行一个执行牌并检测它是否抛出任何异常。 
	CATCH的栈签名是相当复杂的:

		( a_n-1 ... a_1 a_0 xt -- r_m-1 ... r_1 r_0 0 )		如果xt没有抛出一个异常
		( a_n-1 ... a_1 a_0 xt -- ?_n-1 ... ?_1 ?_0 e )		如果xt DID抛出异常'e'的话

	其中 a_i 和 r_i 是xt被EXECUTEd之前和之后的（任意数量的）实参和返回栈内容。 
请注意，特别是在抛出异常的情况下，栈指针被恢复，所以栈上有n个_something_，位于实参 a_i 原来所在的位置。 
我们并不能保证栈上有什么--也许是原来的实参，也许是其他乱七八糟的东西--这主要取决于被执行的字的实现。

	THROW、ABORT和其他一些抛出的异常。

	异常数字是非零的整数。 
按照惯例，正数可用于特定应用程序的异常，而负数则具有ANS FORTH标准中定义的某些含义。 
(例如，-1是由ABORT抛出的异常）。

	0 THROW不做任何事情。 这是THROW的栈签名:

		( 0 -- )
		( * e -- ?_n-1 ... ?_1 ?_0 e )	栈被恢复到相应的CATCH的状态。

	实现取决于CATCH和THROW的定义以及它们之间共享的状态。

	到此为止，返回栈仅仅包括一个返回地址的列表，返回栈的顶部是返回地址，在当前字退出时，我们将在这里继续执行。 
	然而CATCH将在返回栈上推送一个更复杂的 "异常栈帧"。 
	异常栈帧记录了一些关于调用CATCH时的执行状态的东西。

	当被调用时，THROW沿着返回栈向上走（这个过程被称为 "松开"），直到它找到异常栈帧。 
然后它使用异常栈帧中的数据来恢复状态，允许在匹配的CATCH之后继续执行。 
(如果它松开栈，没有找到异常栈帧，那么它就会打印一条信息，然后返回到提示符，这也是所谓的 "未捕获的异常 "的正常行为）。

	这就是异常栈帧的样子。 
(按照惯例，返回栈显示为从高到低的内存地址向下增长）。

		+------------------------------+
		| 来自CATCH的返回地址    		|   注意，当调用CATCH时，这已经在返回栈上了。
		|                              |   
		+------------------------------+
		| 原始参数栈指针    			|
		|                      		   |
		+------------------------------+  ^
		|    异常栈标记        			|  |
		| (EXCEPTION-MARKER)           |  |   通过THROW松开栈的方向。
		+------------------------------+  |   
						  |
						  |

	EXCEPTION-MARKER标志着这个条目是一个异常栈帧，而不是一个普通的返回地址，这就是THROW在展开栈时 "注意到 "的东西。 
(如果你想实现更先进的异常，比如 TRY...WITH，那么你需要使用不同的标记值，如果你想让新旧异常栈帧的布局共存的话）。

	如果执行的字没有抛出一个异常，会发生什么？ 它最终会返回并调用EXCEPTION-MARKER，所以EXCEPTION-MARKER最好做一些合理的事情，而不需要我们修改EXIT。 
这很好地给了我们一个合适的EXCEPTION-MARKER的定义，即一个只是丢弃栈帧并自己返回的函数（因此从原来的CATCH中 "返回"）。

	从中可以看出，在FORTH中，异常是一种相对轻量级的机制。
)
: EXCEPTION-MARKER
	RDROP			( 丢掉原来的参数栈指针 )
	0			( 没有异常，这就是常规的返回路径。 )
;

: CATCH		( xt -- exn? )
	DSP@ 4+ >R		( 在返回栈上保存参数栈指针（由于xt的存在，+4） )
	' EXCEPTION-MARKER 4+	( 在EXCEPTION-MARKER内推RDROP的地址 ... )
	>R			( ...到返回栈中，所以它的作用就像一个返回地址一样 )
	EXECUTE			( 执行嵌套函数 )
;

: THROW		( n -- )
	?DUP IF			( 只有在异常代码<>0时才采取行动 )
		RSP@ 			( 获取返回栈指针 )
		BEGIN
			DUP R0 4- <		( RSP < R0 )
		WHILE
			DUP @			( 获取返回栈的条目 )
			' EXCEPTION-MARKER 4+ = IF	( 在返回栈中发现了EXCEPTION-MARKER。 )
				4+			( 跳过返回栈上的EXCEPTION-MARKER。 )
				RSP!			( 恢复返回栈的指针 )

				( 恢复参数栈. )
				DUP DUP DUP		( 保留一些工作空间，以便这个字的栈不与被恢复的栈部分重合。 )
				R>			( 获取保存的参数栈指针 | n dsp )
				4-			( 在栈中预留空间以存储n )
				SWAP OVER		( dsp n dsp )
				!			( 在栈上写n )
				DSP! EXIT		( 恢复参数栈指针，立即退出 )
			THEN
			4+
		REPEAT

		( 没有匹配的捕获--打印一个消息并重新启动INTERPRETER。 )
		DROP

		CASE
		0 1- OF	( 中止 )
			." ABORTED" CR
		ENDOF
			( default case 默认情况 )
			." UNCAUGHT THROW "
			DUP . CR
		ENDCASE
		QUIT
	THEN
;

: ABORT		( -- )
	0 1- THROW
;

( 通过在返回栈上行走来打印一个栈跟踪。 )
: PRINT-STACK-TRACE
	RSP@				( 从该函数的调用者开始 )
	BEGIN
		DUP R0 4- <		( RSP < R0 )
	WHILE
		DUP @			( 获取返回栈的条目 )
		CASE
		' EXCEPTION-MARKER 4+ OF	( 是异常栈帧吗？ )
			." CATCH ( DSP="
			4+ DUP @ U.		( 打印已保存的栈指针 )
			." ) "
		ENDOF
						( default case )
			DUP
			CFA>			( 查找代码字，获取字典条目 )
			?DUP IF			( 并打印它 )
				2DUP			( dea addr dea )
				ID.			( 打印字典条目中的字 )
				[ CHAR + ] LITERAL EMIT
				SWAP >DFA 4+ - .	( 打印偏移 )
			THEN
		ENDCASE
		4+			( 向上移动栈 )
	REPEAT
	DROP
	CR
;

(
	C 字符串 ----------------------------------------------------------------------

	FORTH字符串由栈或内存中保存的起始地址和长度表示。

	大多数FORTH都不处理C语言字符串，
	但我们需要它们来访问Linux内核留在栈中的进程参数和环境，并进行一些系统调用。

	操作	      输入		 输出		  FORTH字	   说明
	----------------------------------------------------------------------

	Create FORTH string		addr len	S" ..."

	Create C string			c-addr		Z" ..."

	C -> FORTH	c-addr		addr len	DUP STRLEN

	FORTH -> C	addr len	c-addr		CSTRING		分配在一个临时缓冲区中，所以应该立即消耗/复制。FORTH字符串不应包含NULs。
									
									

	例如，DUP STRLEN TELL打印出一个C语言字符串。
)

(
	Z" ... "与S" ... "类似，只是该字符串以ASCII NUL字符终结。

	为了使它更像一个C语言的字符串，在运行时Z "只是把字符串的地址留在栈上（而不是像S "那样的地址和长度）。 
为了实现这一点，我们需要在字符串中添加额外的NUL，并在之后添加一条DROP指令。 
除此以外，这个实现只是一个修改过的S"。
)
: Z" IMMEDIATE
	STATE @ IF	( 编译吗？ )
		' LITSTRING ,	( 编译LITSTRING )
		HERE @		( 在栈中保存长度字的地址 )
		0 ,		( 假动作长度 - 我们还不知道它是什么 )
		BEGIN
			KEY 		( 获取字符串的下一个字符 )
			DUP '"' <>
		WHILE
			HERE @ C!	( 在编译后的镜像中存储该字符 )
			1 HERE +!	( 将HERE指针递增1个字节 )
		REPEAT
		0 HERE @ C!	( 增加ASCII NUL字节 )
		1 HERE +!
		DROP		( 丢掉末尾的双引号字符 )
		DUP		( 获取长度字的保存地址 )
		HERE @ SWAP -	( 计算出长度 )
		4-		( 减去4（因为我们是从长度字开始测量的）。 )
		SWAP !		( 并回填长度位置 )
		ALIGN		( 对于剩余的代码，四舍五入到下一个4字节的倍数。 )
		' DROP ,	( 编译DROP(丢掉长度) )
	ELSE		( 立即模式 )
		HERE @		( 获取临时空间的起始地址 )
		BEGIN
			KEY
			DUP '"' <>
		WHILE
			OVER C!		( 保存下一个字符 )
			1+		( 递增地址 )
		REPEAT
		DROP		( 丢掉最后的 " 字符 )
		0 SWAP C!	( 存储最后的ASCII NUL )
		HERE @		( 推开始地址 )
	THEN
;

: STRLEN 	( str -- len )
	DUP		( 保存起始地址 )
	BEGIN
		DUP C@ 0<>	( 找到零字节？ )
	WHILE
		1+
	REPEAT

	SWAP -		( 计算出长度 )
;

: CSTRING	( addr len -- c-addr )
	SWAP OVER	( len saddr len )
	HERE @ SWAP	( len saddr daddr len )
	CMOVE		( len )

	HERE @ +	( daddr+len )
	0 SWAP C!	( 存储终结的NUL字符 )

	HERE @ 		( 推开始地址 )
;

(
	环境 ----------------------------------------------------------------------

	Linux将进程实参和环境在栈上提供给我们。

	当我们启动时，栈顶的指针被早期的汇编代码保存在FORTH变量S0中，从这个指针开始，我们可以读出命令行参数和环境。

	从S0开始，S0本身指向argc（命令行实参的数量）。

	S0+4指向argv[0]，S0+8指向argv[1]等等，直到argv[argc-1]。

	argv[argc]是一个NULL指针。

	之后，栈包含环境变量，一组指向NAME=VALUE形式的字符串的指针，直到我们得到另一个NULL指针。

	我们定义的第一个字，ARGC，推动了命令行实参的数量（注意，与C语言的argc一样，这包括命令的名称）。
)
: ARGC
	S0 @ @
;

(
	n ARGV获取第n个命令行实参。

	例如，如果要打印命令名称，你应该这样做:
		0 ARGV TELL CR
)
: ARGV ( n -- str u )
	1+ CELLS S0 @ +	( 获取argv[n]条目的地址 )
	@		( 获取字符串的地址 )
	DUP STRLEN	( 并获取其长度/将其变成FORTH字符串 )
;

(
	ENVIRON返回第一个环境字符串的地址。 字符串的列表以一个NULL指针结束。

	例如，要打印环境中的第一个字符串，你可以这样做:
		ENVIRON @ DUP STRLEN TELL
)
: ENVIRON	( -- addr )
	ARGC		( 栈上要跳过的命令行形参的数目 )
	2 +		( 跳过命令行计数和命令行args后的NULL指针 )
	CELLS		( 转换为偏移量 )
	S0 @ +		( 加到基础栈地址 )
;

(
	系统调用和文件  ----------------------------------------------------------------------

	与系统调用有关的杂项字，以及对文件的标准访问。
)
( BYE通过调用Linux exit(2)系统调用而退出。)
: BYE		( -- )
	0		( 返回代码 (0) )
	SYS_EXIT	( 系统调用号 )
	SYSCALL1
;

(
	UNUSED返回用户内存（数据段）中剩余的cells 数。

	在我们的实现中，我们将使用Linux的brk(2)系统调用来找出数据段的末端，并从其中减去HERE。
)
: GET-BRK	( -- brkpoint )
	0 SYS_BRK SYSCALL1	( 调用 brk(0) )
;

: UNUSED	( -- n )
	GET-BRK		( 根据内核获取数据段的末端 )
	HERE @		( 获取数据段的当前位置 )
	-
	4 /		( 返回cells的数量 )
;

(
	MORECORE将数据段递增指定数量的（4字节）cells。

	NB. 要求的单元格数量通常应该是1024的倍数。 
原因是Linux不能将数据段扩展到小于一个页面（4096字节或1024 cells）。

	这个FORTH不会 "按需 "自动增加数据段的大小（即当使用Ⅳ（COMMA）、ALLOT、CREATE等）。 
相反，程序员需要意识到一个大的分配会占用多少空间，检查UNUSED，并在必要时调用MORECORE。 
一个简单的编程练习是改变数据段的实现，以便在程序需要更多内存时自动调用MORECORE。
)
: BRK		( brkpoint -- )
	SYS_BRK SYSCALL1
;

: MORECORE	( cells -- )
	CELLS GET-BRK + BRK
;

(
	标准FORTH提供了一些简单的文件访问原语，我们在Linux系统调用的基础上对其进行建模。

	主要的复杂问题是将FORTH字符串（地址和长度）转换为Linux内核的C字符串。

	注意在这个实现中没有缓冲。
)
: R/O ( -- fam ) O_RDONLY ;
: R/W ( -- fam ) O_RDWR ;

: OPEN-FILE	( addr u fam -- fd 0 (若成功) | c-addr u fam -- fd errno (如果有错误) )
	-ROT		( fam addr u )
	CSTRING		( fam cstring )
	SYS_OPEN SYSCALL2 ( open (filename, flags) )
	DUP		( fd fd )
	DUP 0< IF	( errno? )
		NEGATE		( fd errno )
	ELSE
		DROP 0		( fd 0 )
	THEN
;

: CREATE-FILE	( addr u fam -- fd 0 (若成功) | c-addr u fam -- fd errno (如果有错误) )
	O_CREAT OR
	O_TRUNC OR
	-ROT		( fam addr u )
	CSTRING		( fam cstring )
	420 -ROT	( 0644 fam cstring )
	SYS_OPEN SYSCALL3 ( open (filename, flags|O_TRUNC|O_CREAT, 0644) )
	DUP		( fd fd )
	DUP 0< IF	( errno? )
		NEGATE		( fd errno )
	ELSE
		DROP 0		( fd 0 )
	THEN
;

: CLOSE-FILE	( fd -- 0 (若成功) | fd -- errno (如果有错误) )
	SYS_CLOSE SYSCALL1
	NEGATE
;

: READ-FILE	( addr u fd -- u2 0 (若成功) | addr u fd -- 0 0 (if EOF) | addr u fd -- u2 errno (if error) )
	>R SWAP R>	( u addr fd )
	SYS_READ SYSCALL3

	DUP		( u2 u2 )
	DUP 0< IF	( errno? )
		NEGATE		( u2 errno )
	ELSE
		DROP 0		( u2 0 )
	THEN
;

(
	PERROR打印一个错误消息，类似于C的perror(3)，但我们没有大量的字符串可用，所以我们能做的就是打印错误消息。
)
: PERROR	( errno addr u -- )
	TELL
	':' EMIT SPACE
	." ERRNO="
	. CR
;

(
	汇编代码 ----------------------------------------------------------------------

	这只是一个简单的汇编程序的概要，允许你用汇编语言编写FORTH原语。

	汇编原语以正常方式开始 ': NAME'，但以 ;CODE 结束。
	 ;CODE 更新了头，因此代码字不是DOCOL，而是指向汇编后的代码（在字的DFA部分）。

	我们提供了一个方便的宏NEXT（你猜到了它的作用）。 
但是你不需要使用它，因为 ;CODE 会在你的字的末尾放一个NEXT。

	其余的由一些立即字组成，这些字扩展为机器码追加在字的定义上。 
i386的汇编空间只有很小的一部分被覆盖，只够编写下面几个汇编原语。
)

HEX 十六进制

( 相当于NEXT宏 )
: NEXT IMMEDIATE AD C, FF C, 20 C, ;

: ;CODE IMMEDIATE
	[COMPILE] NEXT		( 用NEXT宏来结束这个字 )
	ALIGN			( 机器码是以字节为单位进行汇编的，所以不一定在末端对齐。 )
	LATEST @ DUP
	HIDDEN			( 解除隐藏字 )
	DUP >DFA SWAP >CFA !	( 改变代码字以指向数据区 )
	[COMPILE] [		( 回到立即模式 )
;

( i386的寄存器 )
: EAX IMMEDIATE 0 ;
: ECX IMMEDIATE 1 ;
: EDX IMMEDIATE 2 ;
: EBX IMMEDIATE 3 ;
: ESP IMMEDIATE 4 ;
: EBP IMMEDIATE 5 ;
: ESI IMMEDIATE 6 ;
: EDI IMMEDIATE 7 ;

( i386栈指令 )
: PUSH IMMEDIATE 50 + C, ;
: POP IMMEDIATE 58 + C, ;

( RDTSC指令 )
: RDTSC IMMEDIATE 0F C, 31 C, ;

DECIMAL 十进制

(
	RDTSC是一个汇编器原语，它读取奔腾的时间戳计数器（一个非常细粒度的计数器，计算处理器的时钟周期）。 
因为TSC是64位宽，我们必须把它推到栈的两个槽中。
)
: RDTSC		( -- lsb msb )
	RDTSC		( 将结果写入%edx:%eax中 )
	EAX PUSH	( push lsb )
	EDX PUSH	( push msb )
;CODE

(
	INLINE可用于将一个汇编原语内联到当前（汇编）字中。

	例如:

		: 2DROP INLINE DROP INLINE DROP ;CODE

	将建立一个高效的汇编字2DROP，其中包含DROP的内联汇编代码，后面是DROP（例如，在这种情况下，两个'pop %eax'指令）。

	另一个例子。 考虑一下这个普通的FORTH定义:

		: C@++ ( addr -- addr+1 byte ) DUP 1+ SWAP C@ ;

	(它等同于C操作'*p++'，其中p是char的指针）。 
如果我们注意到所有用于定义C@++的字实际上都是汇编器的原语，那么我们可以写一个更快的（但等价的）定义，比如说:

		: C@++ INLINE DUP INLINE 1+ INLINE SWAP INLINE C@ ;CODE

	值得注意的一点是，这种 "串联" 式的编程风格允许你可移植地编写汇编字。 
上述定义适用于任何CPU架构。

	要成功使用INLINE，必须满足几个条件:

	(1) 你目前必须定义一个汇编字（即 : ... ;CODE）。

	(2) 你所内联的字必须是已知的汇编字。 
如果你试图内联一个FORTH字，你会得到一个错误消息。

	(3) 汇编原语必须是与位置无关的代码，并且必须以一个NEXT宏结束。

	读者的练习：(a) 对INLINE进行泛化，使其在构建FORTH字时能够内联FORTH。
	(b) 进一步概括INLINE，使它在试图将FORTH内联到汇编程序中时能做一些合理的事情，反之亦然。

	INLINE的实现是非常简单的。 我们在字典中找到这个字，
	检查它是否是一个汇编字，然后逐个字节地把它复制到当前的定义中，直到我们到达NEXT宏（它没有被复制）。
)
HEX
: =NEXT		( addr -- next? )
	   DUP C@ AD <> IF DROP FALSE EXIT THEN
	1+ DUP C@ FF <> IF DROP FALSE EXIT THEN
	1+     C@ 20 <> IF      FALSE EXIT THEN
	TRUE
;

DECIMAL

( (INLINE)是低级别的内联函数。 )
: (INLINE)	( cfa -- )
	@			( 记住代码字指向的代码 )
	BEGIN			( 拷贝字节，直到我们碰到NEXT宏 )
		DUP =NEXT NOT
	WHILE
		DUP C@ C,
		1+
	REPEAT
	DROP
;

: INLINE IMMEDIATE
	WORD FIND		( 在字典里找到这个字 )
	>CFA			( 代码字 )

	DUP @ DOCOL = IF	( 检查代码字 <> DOCOL（即-不是FORTH字） )
		." Cannot INLINE FORTH words" CR ABORT
	THEN

	(INLINE)
;

HIDE =NEXT

(
	注意 ----------------------------------------------------------------------

	DOES>是不可能用这个FORTH实现的，因为我们没有一个单独的数据指针。
)

(
	欢迎消息 ----------------------------------------------------------------------

	打印版本和OK提示。
)

: WELCOME
	S" TEST-MODE" FIND NOT IF
		." JONESFORTH VERSION " VERSION . CR
		UNUSED . ." CELLS REMAINING" CR
		." OK "
	THEN
;

WELCOME
HIDE WELCOME