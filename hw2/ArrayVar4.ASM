; variant 4
; TASK: 1)create array A
;       2)create vector B using rule A[i]>0 => B[i]=1; A[i]<0 => B[i]=-1; A[i]==0 => B[i]=0;
;       3)output both arrays



format PE console

entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'size of vector? ', 0
        strIncorSize db 'Incorrect size of vector = %d', 10, 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
        strVecElemOut  db '[%d] = %d', 10, 0
        strVecA db 'Vector A:',10,0
        strVecB db 'Vector B:',10,0
        vec_size     dd 0
        i            dd ?
        tmp          dd ?
        tmpStack     dd ?
        vec          rd 100
        vecB          rd 100

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
; vector input
        call VectorInput
; create vector B using rule A[i]>0 => B[i]=1; A[i]<0 => B[i]=-1; A[i]==0 => B[i]=0;
        call VectorCreate
;  print vector A
        push strVecA
        call [printf]
        call VectorOutA
;  print vector B
        push strVecB
        call [printf]
        call VectorOutB
;ending
finish:
        call [getch]
        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------
VectorInput:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jg  getVector

        push vec_size
        push strIncorSize
        call [printf]
        push 0
        call [ExitProcess]

getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector

        ; input element
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
       push strScanInt
       call [scanf]
       add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret
;--------------------------------------------------------------------------
VectorCreate:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
createVecLoop:
        cmp ecx, [vec_size]
        je endSumVector
        mov eax, [ebx]
        or eax,eax
        js negative  ;if A[i]<0
        cmp eax, 0
        je zero  ;if A[i]==0
        ja positive ;if A[i]>0
        jmp endif

        positive:
        mov [vecB +ecx*4] , 1  ; B[i]=1
        jmp endif

        negative:
        mov [vecB +ecx*4] , -1 ; B[i]=-1
        jmp endif

        zero:
        mov [vecB +ecx*4] , 0  ; B[i]=0
        endif :
        inc ecx
        add ebx, 4
        jmp createVecLoop
endSumVector:
        ret
;--------------------------------------------------------------------------
VectorOutB:
        mov [tmpStack], esp
        xor ecx, ecx            ; ecx = 0
        mov ebx, vecB            ; ebx = &vec
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector      ; to end of loop
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:
        mov esp, [tmpStack]
        ret
 ;--------------------------------------------------------------------------
VectorOutA:
        mov [tmpStack], esp
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec           ; ebx = &vec
putVecLoop2:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector2      ; to end of loop
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop2
endOutputVector2:
        mov esp, [tmpStack]
        ret
;---------------------------------------------------------
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'
