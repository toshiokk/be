



--------------------------------------------------------------------------------------
### LHV, RHV
Four memory area
- Program instruction area           00001000h
- Program constant data area         00008000h
- Static data area                   00800000h
- Stack data area                    00fff000h

static short var_x;
var_x = 1;
  (("1" is a right hand value.))
  (("x" is a left hand value.))
    1-A. A value 1 may appear in the assembly code as a immediate value.
        mov ax, 0001h        ; `00001000 c0 01 00`
    1-B. A value 1 may allocated in const data area.
      val_1: dw 0001h        ; `00008000 01 00`
        mov ax, (val_1)
    2. or allocated in static memory.
      var_x: dw 0000h        ; 16 bits variable `00800000 00 00 00 00`
        mov (var_x), ax      ; store 16 bits value to memory

static short* ptr_x;  // pointer (variable that contain memory address)
ptr_x = &var_x;    // assign a value to the pointer (pointer hold memory address of variable "x")
    ptr_x: dd 00000000h      ; define pointer `00800004 00 00 00 00`
      mov esi, offset var_x  ; get address of var_x in register ESI
      mov (ptr_x), esi       ; set address to "ptr_x"
short var_y = *ptr_x;  // get a 16 bit memory value and store to the "var_y"
    var_y: dw 0000h          ; `00800008 00 00`
      mov esi, (ptr_x)       ; get address contained in "ptr_x"
      mov ax, (esi)          ; get value thru a pointer
      mov (var_y), ax        ; store to "var_y"

short& ref_x = x;
    ref_x: dd 00000000h      ; `0080000c 00 00 00 00` ("reference"'s actual existance is a pointer)
      mov esi, offset var_x  ; get address of var_x in register ESI
      mov (ref_x), esi       ; set address to "ref_x"
short var_y = ref_x;  // get a 16 bit memory value and store to the "var_y"
    var_y: dw 00000000h      ; `00800008 00 00`
      mov esi, (ref_x)       ; get address contained in "ref_x"
      mov ax, (esi)          ; get value thru a pointer
      mov (var_y), ax        ; store to "var_y"

short&& rref_x = x;
    rref_x: dd 00000000h      ; `0080000c 00 00 00 00` ("RHV reference"'s actual existance is a pointer)
      mov esi, offset var_x  ; get address of var_x in register ESI
      mov (ref_x), esi       ; set address to "ref_x"
short var_y = ref_x;  // get a 16 bit memory value and store to the "var_y"
    var_y: dw 00000000h      ; `00800008 00 00`
      mov esi, (ref_x)       ; get address contained in "ref_x"
      mov ax, (esi)          ; get value thru a pointer
      mov (var_y), ax        ; store to "var_y"
