; This is an example of what generated LLVM IR looks like
; for the following expression:
;      with a: 3*a

; Library functions must be declared, like in C.
; The syntax also resembles C. The type before the
; function name is the return type. The type names
; surrounded by parentheses are the argument types.
; The declaration can appear anywhere in the file.
declare i32 @calc_read(ptr)
declare void @calc_write(i32)

; The `calc_read()` function (which we will use later)
; takes the variable name as a parameter. The following
; construct defines a constant, holding `a` and the null byte,
; which is used as a string terminator in C.
@a.str = private constant [2 x i8] c"a\00"

; This is the main function. The parameter names are omitted
; because they aren't used. Just like in C, the body of the
; function is enclosed in braces.
define i32 @main(i32, ptr) {
; Each basic block must have a label. Because this
; is the first basic block of the function, we'll name it `entry`
entry:
    %2 = call i32 @calc_read(ptr @a.str) ; Read value for the `a` var, store in %2
    %3 = mul nsw i32 3, %2 ; Variable is multiplied by 3, stored in %3
    call void @calc_write(i32, %3) ; Print result to console
    ret i32 0 ; Return 0 to indicate a successful execution
}

; Each value in the LLVM IR is typed. i32 denotes a 32-bit integer and
; ptr denotes a pointer (pointers are opaque - you don't know the type of
; the data it points to)