.686P  
.model flat,stdcall 
option casemap: none  
include PasLib.INC 
.data  
  ;ConstSymbolList:
__Const0=0  
  ;VarSymbolList
__cvt DWORD 1 DUP(0)  
_DATENTIMEDEMO$I DWORD 1 DUP(0)  
_DATENTIMEDEMO$J DWORD 1 DUP(0)  
_DATENTIMEDEMO$_T0 DWORD 1 DUP(0)  
_DATENTIMEDEMO$_T1=_DATENTIMEDEMO$_T0  
.code  
  ;function:DATENTIMEDEMO
  ;process of start
start:   
_DATENTIMEDEMO proc 
push ebp 
mov ebp,esp 
sub esp,0 
push ebx 
push esi 
push edi 
  ;(ByteToInt     ,0,null,_T0)
mov al,byte ptr [__Const0] 
mov dword ptr ebx,0 
mov bl,byte ptr al 
  ;ebx:_T0,
mov [_DATENTIMEDEMO$_T0],dword ptr ebx 
  ;(ASSIGN_4      ,_T0,null,I)
mov ecx,dword ptr [_DATENTIMEDEMO$_T0] 
  ;ecx:_T0,I,
mov [_DATENTIMEDEMO$I],dword ptr ecx 
  ;(ADD_4         ,I,J,_T1)
mov edx,dword ptr [_DATENTIMEDEMO$I] 
mov [_DATENTIMEDEMO$I],dword ptr edx 
add edx,dword ptr [_DATENTIMEDEMO$J] 
  ;edx:_T1,
mov [_DATENTIMEDEMO$_T1],dword ptr edx 
  ;(ASSIGN_4      ,_T1,null,I)
mov edi,dword ptr [_DATENTIMEDEMO$_T1] 
  ;edi:_T1,I,
mov [_DATENTIMEDEMO$I],dword ptr edi 
  ;process of end
pop edi 
pop esi 
pop ebx 
mov esp,ebp 
pop ebp 
ret 0 
_DATENTIMEDEMO endp 
end start 
