.686P  
.model flat,stdcall 
option casemap: none  
include PasLib.INC 
.data  
  ;ConstSymbolList:
__Const1=1  
__Const2=0  
__Text3 db "message",0  
__Const3 dword 0  
  ;VarSymbolList
__cvt DWORD 1 DUP(0)  
_A$RESULT=-4  
.code  
  ;function:MAIN
  ;process of start
start:   
_MAIN proc 
push ebp 
mov ebp,esp 
sub esp,0 
push ebx 
push esi 
push edi 
mov eax,8 
call _GetMem@0 
mov dword ptr[__Const3 ],eax 
mov edx,dword ptr[__Const3 ] 
mov eax,offset[__Text3 ] 
call _StrCopy@0 
  ;(ByteToInt     ,0,null,_T1)
mov al,byte ptr [__Const2] 
mov dword ptr ebx,0 
mov bl,byte ptr al 
  ;ebx:_T1,
  ;(ASSIGN_4      ,_T1,null,I)
mov ecx,dword ptr [_MAIN$_T1] 
  ;ecx:_T1,I,
mov [_MAIN$I],dword ptr ecx 
  ;(ADD_4         ,I,J,_T2)
mov edx,dword ptr [_MAIN$I] 
mov [_MAIN$I],dword ptr edx 
add edx,dword ptr [_MAIN$J] 
  ;edx:_T2,
  ;(ASSIGN_4      ,_T2,null,I)
mov edi,dword ptr [_MAIN$_T2] 
  ;edi:_T2,I,
mov [_MAIN$I],dword ptr edi 
  ;(ADD_S         ,HOT,'message',_T3)
  ;(ASSIGN_S      ,_T3,null,HOT)
  ;(CALL          ,A,null,null)
call _A 
  ;(RETV          ,null,null,_T4)
mov dword ptr [_MAIN$_T4],eax 
  ;process of end
mov eax,dword ptr[__Const3 ] 
call _FreeMem@0 
pop edi 
pop esi 
pop ebx 
mov esp,ebp 
pop ebp 
ret 0 
_MAIN endp 
  ;function:A
  ;process of start
_A proc 
push ebp 
mov ebp,esp 
sub esp,4 
push ebx 
push esi 
push edi 
  ;(ByteToInt     ,1,null,_T0)
mov bl,byte ptr [__Const1] 
mov dword ptr eax,0 
mov al,byte ptr bl 
  ;eax:_T0,
  ;(ASSIGN_4      ,_T0,null,RESULT)
mov ebx,dword ptr [ebp+_A$_T0] 
  ;ebx:_T0,RESULT,
mov [ebp+_A$RESULT],dword ptr ebx 
  ;process of end
mov eax,ebp+_A$RESULT 
pop edi 
pop esi 
pop ebx 
mov esp,ebp 
pop ebp 
ret 0 
_A endp 
end start 
