format PE console
entry start

include 'win32a.inc'


section '.code' code readable executable

start:
    ; input x
    invoke printf, mesInput
    invoke scanf,  formatF, x;

    FINIT
    ;first element of the sum is x, so sum after 1st step is x
    fld [x]
    fst [elem]
    fst [sum]
    ;check if x==0
    ftst
    fstsw ax
    sahf
    jz zero
   ;check x boundaries (must be between -710,710,otherwise print error message)
    fabs
    fstp [tmp]
    fld [boundary]
    fsub [tmp]
    ftst
    fstsw   ax
    sahf
    jb   tooBig      ;print error message
;calculate loop
addElem:
    fld [sum]
    fstp [prevsum]   ;prevSum = sum
    ; calculate  element = element * x * x / ((2 * n) * (2 * n + 1)):
    ;1) calculate numerator (element * x * x)
    fld [elem]
    fmul [x]
    fmul [x]
    ;2)calculate denominator  ((2 * n) * (2 * n + 1))
    mov eax,[n] ; eax = n
    mov ebx,2    ;ebx = 2
    mul ebx  ;eax = 2n
    mov [denominator],eax ; denominator = 2n
    mov ebx,1    ;ebx = 1
    add eax,ebx  ;eax = 2n +1
    mul [denominator] ;  ; eax = 2n * (2n + 1)
    mov [denominator],eax
    ;3)calculate element =  numerator / denominator =  element * x * x / ((2 * n) * (2 * n + 1))
    fidiv [denominator]
    fstp [elem]
    ;sum += element;
    fld [sum]
    fadd [elem]
    fstp [sum]
    ;  n++;
    mov eax,[n]
    add eax, 1
    mov [n], eax
;  if |(sum - prevSum) / sum)| < 0.001) => end calculation
    fld [sum]
    fsub [prevsum]    ; sto = sum - prevSum
    fdiv [sum]  ;st0 = (sum - prevSum / sum)
    fabs
    fstp [tmp]
   fld [error]
    fsub [tmp] ;st0 =  0.001 - (sum - prevSum / sum)
    ftst
    fstsw   ax
    sahf

    jb contCalc   ;need to continue the calculation
    jmp printRes

tooBig: ;x is too big - print error message
    invoke printf, mesError
    jmp end_work

contCalc:  ;clear stack and loop one more time
    fstp [tmp]
    jmp addElem

zero: ;sh(0)=0
    invoke printf, mesRes, dword[x], dword[x+4], \
                         dword[x], dword[x+4]
    jmp end_work

printRes:

   invoke printf, mesRes, dword[x], dword[x+4], \
                         dword[sum], dword[sum+4]
    jmp end_work

    end_work:
    invoke getch
    invoke ExitProcess, 0

;-------------------------------third act - including HeapApi--------------------------
section '.data' data readable writable

    mesInput        db    "To calculate sh(x) input x: ", 0
    formatF     db    '%lf', 0
    mesRes      db    "sh(%lf)= %lf", 10, 0
    mesError    db    "x must be between -710.0 and 710.0", 10, 0
    mesRealResult    db    "x must be between -710.0 and 710.0", 10, 0
    x   dq      ?    ;input
    elem dq      0   ;element in a Taylor series
    sum dq      0    ;sum of the sequence
    prevsum dq      0  ;prevSum - sum before adding current element
    tmp dq      ?
    boundary dq 710.0  ;x must be between [-710, 710]
    denominator dd 0    ;denominator of the element in taylor series (calculated for each element)
    error dq 0.001  ;than error < 0.1% - > end calculation
    n dd 1      ;number of elements in Taylor series

section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll'

    import kernel,\
           ExitProcess, 'ExitProcess'
    import msvcrt,\
           printf, 'printf',\
           sprintf, 'sprintf',\
           scanf, 'scanf',\
           getch, '_getch'
