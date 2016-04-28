.686P  
.model flat,stdcall 
option casemap: none  
include PasLib.INC 
.data  
  ;ConstSymbolList:
__Const3=0  
__Const5=1  
__Text6 db "message",0  
__Const6 dword 0  
  ;VarSymbolList
__cvt DWORD 1 DUP(0)  
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
mov eax,8 
call _GetMem@0 
mov dword ptr[__Const6 ],eax 
mov edx,dword ptr[__Const6 ] 
mov eax,offset[__Text6 ] 
call _StrCopy@0 
  ;(ByteToInt     ,0,null,_T0)
mov al,byte ptr [__Const3] 
mov dword ptr ebx,0 
mov bl,byte ptr al 
  ;ebx:_T0,
  ;(ASSIGN_4      ,_T0,null,I)
mov ecx,dword ptr [_DATENTIMEDEMO$_T0] 
  ;ecx:_T0,I,
mov [_DATENTIMEDEMO$I],dword ptr ecx 
  ;(ADD_4         ,I,J,_T1)
mov edx,dword ptr [_DATENTIMEDEMO$I] 
mov [_DATENTIMEDEMO$I],dword ptr edx 
add edx,dword ptr [_DATENTIMEDEMO$J] 
  ;edx:_T1,
  ;(ASSIGN_4      ,_T1,null,I)
mov edi,dword ptr [_DATENTIMEDEMO$_T1] 
  ;edi:_T1,I,
mov [_DATENTIMEDEMO$I],dword ptr edi 
  ;(ByteToInt     ,1,null,_T2)
mov bl,byte ptr [__Const5] 
mov dword ptr eax,0 
mov al,byte ptr bl 
  ;eax:_T2,
  ;(ASSIGN_4      ,_T2,null,NAME)
mov ebx,dword ptr [_DATENTIMEDEMO$_T2] 
  ;ebx:_T2,NAME,
mov [_DATENTIMEDEMO$NAME],dword ptr ebx 
  ;(ASSIGN_S      ,'message',null,HOT)
  ;(ASSIGN_S      ,HOT,null,LESS)
  ;process of end
mov eax,dword ptr[__Const6 ] 
call _FreeMem@0 
pop edi 
pop esi 
pop ebx 
mov esp,ebp 
pop ebp 
ret 0 
_DATENTIMEDEMO endp 
end start 
