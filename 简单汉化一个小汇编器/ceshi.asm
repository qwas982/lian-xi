use16


msg db '你好世界',0x0a
len equ $-msg

	
_start:
	移 dx,len
	移 cx,msg
	移 bx,1
	移 ax,4
	中断 0x80
	
	移 bx,0
	移 ax,1
	中断 0x80
