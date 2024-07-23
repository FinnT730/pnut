#!/bin/sh
set -e -u

__ALLOC=1 # Starting heap at 1 because 0 is the null pointer.

_malloc() { # $2 = object size
  : $((_$__ALLOC = $2)) # Track object size
  : $(($1 = $__ALLOC + 1))
  : $((__ALLOC += $2 + 1))
}

defarr() { _malloc $1 $2; }

defarr _buf 1024
: $((fd = 0))
_cat_fd() { let fd $2
  :
  endlet $1 fd
}

__c2i_0=48
__c2i_1=49
__c2i_2=50
__c2i_3=51
__c2i_4=52
__c2i_5=53
__c2i_6=54
__c2i_7=55
__c2i_8=56
__c2i_9=57
__c2i_a=97
__c2i_b=98
__c2i_c=99
__c2i_d=100
__c2i_e=101
__c2i_f=102
__c2i_g=103
__c2i_h=104
__c2i_i=105
__c2i_j=106
__c2i_k=107
__c2i_l=108
__c2i_m=109
__c2i_n=110
__c2i_o=111
__c2i_p=112
__c2i_q=113
__c2i_r=114
__c2i_s=115
__c2i_t=116
__c2i_u=117
__c2i_v=118
__c2i_w=119
__c2i_x=120
__c2i_y=121
__c2i_z=122
__c2i_A=65
__c2i_B=66
__c2i_C=67
__c2i_D=68
__c2i_E=69
__c2i_F=70
__c2i_G=71
__c2i_H=72
__c2i_I=73
__c2i_J=74
__c2i_K=75
__c2i_L=76
__c2i_M=77
__c2i_N=78
__c2i_O=79
__c2i_P=80
__c2i_Q=81
__c2i_R=82
__c2i_S=83
__c2i_T=84
__c2i_U=85
__c2i_V=86
__c2i_W=87
__c2i_X=88
__c2i_Y=89
__c2i_Z=90
char_to_int() {
  case $1 in
    [a-zA-Z0-9]) __c=$((__c2i_$1)) ;;
    ' ') __c=32 ;;
    '!') __c=33 ;;
    '"') __c=34 ;;
    '#') __c=35 ;;
    '$') __c=36 ;;
    '%') __c=37 ;;
    '&') __c=38 ;;
    "'") __c=39 ;;
    '(') __c=40 ;;
    ')') __c=41 ;;
    '*') __c=42 ;;
    '+') __c=43 ;;
    ',') __c=44 ;;
    '-') __c=45 ;;
    '.') __c=46 ;;
    '/') __c=47 ;;
    ':') __c=58 ;;
    ';') __c=59 ;;
    '<') __c=60 ;;
    '=') __c=61 ;;
    '>') __c=62 ;;
    '?') __c=63 ;;
    '@') __c=64 ;;
    '[') __c=91 ;;
    '\') __c=92 ;;
    ']') __c=93 ;;
    '^') __c=94 ;;
    '_') __c=95 ;;
    '`') __c=96 ;;
    '{') __c=123 ;;
    '|') __c=124 ;;
    '}') __c=125 ;;
    '~') __c=126 ;;
    *)
      __c=$(LC_CTYPE=C printf "%d" "'$1")
  esac
}

unpack_escaped_string() {
  __buf="$1"
  # Allocates enough space for all characters, assuming that no character is escaped
  _malloc __addr $((${#__buf} + 1))
  __ptr=$__addr
  while [ -n "$__buf" ] ; do
    case "$__buf" in
      '\'*)
        __buf="${__buf#?}"               # remove the current char from $__buf
        case "$__buf" in
          'a'*) __c=7 ;;
          'b'*) __c=8 ;;
          'f'*) __c=12 ;;
          'n'*) __c=10 ;;
          'r'*) __c=13 ;;
          't'*) __c=9 ;;
          'v'*) __c=11 ;;
          '\'*) __c=92 ;;
          '"'*) __c=34 ;;
          "'"*) __c=39 ;;
          '?'*) __c=63 ;;
          '$'*) __c=36 ;; # Not in C, used to escape variable expansion between double quotes
          *) echo "invalid escape in string: $__char"; exit 1 ;;
        esac
        __buf="${__buf#?}"               # remove the current char from $__buf
        ;;
      *)
        char_to_int "${__buf%"${__buf#?}"}"
        __buf="${__buf#?}"                  # remove the current char from $__buf
        ;;
    esac
    : $((_$__ptr = __c))
    : $((__ptr += 1))
  done
  : $((_$__ptr = 0))
}

# Define a string, and return a reference to it in the varible taken as argument.
# If the variable is already defined, this function does nothing.
# Note that it's up to the caller to ensure that no 2 strings share the same variable.
defstr() { # $1 = variable name, $2 = string
  set +u # Necessary to allow the variable to be empty
  if [ $(($1)) -eq 0 ]; then
    unpack_escaped_string "$2"
    : $(($1 = __addr))
  fi
  set -u
}

defstr __str_0 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
_codes=$__str_0
: $((b3 = b2 = b1 = 0))
_encode() {
  let b1; let b2; let b3
  while [ 1 != 0 ] ; do
    _getchar b1 
    if [ $b1 -lt 0 ] ; then
      break
    fi
    _getchar b2 
    printf \\$(((_$((_codes + (b1 >> 2))))/64))$(((_$((_codes + (b1 >> 2))))/8%8))$(((_$((_codes + (b1 >> 2))))%8))
    if [ $b2 -lt 0 ] ; then
      printf \\$(((_$((_codes + (63 & (b1 << 4)))))/64))$(((_$((_codes + (63 & (b1 << 4)))))/8%8))$(((_$((_codes + (63 & (b1 << 4)))))%8))
      printf \\$(((__EQ__)/64))$(((__EQ__)/8%8))$(((__EQ__)%8))
      printf \\$(((__EQ__)/64))$(((__EQ__)/8%8))$(((__EQ__)%8))
      break
    else
      printf \\$(((_$((_codes + (63 & ((b1 << 4) | (b2 >> 4))))))/64))$(((_$((_codes + (63 & ((b1 << 4) | (b2 >> 4))))))/8%8))$(((_$((_codes + (63 & ((b1 << 4) | (b2 >> 4))))))%8))
      _getchar b3 
      if [ $b3 -lt 0 ] ; then
        printf \\$(((_$((_codes + (63 & (b2 << 2)))))/64))$(((_$((_codes + (63 & (b2 << 2)))))/8%8))$(((_$((_codes + (63 & (b2 << 2)))))%8))
        printf \\$(((__EQ__)/64))$(((__EQ__)/8%8))$(((__EQ__)%8))
        break
      else
        printf \\$(((_$((_codes + (63 & ((b2 << 2) | (b3 >> 6))))))/64))$(((_$((_codes + (63 & ((b2 << 2) | (b3 >> 6))))))/8%8))$(((_$((_codes + (63 & ((b2 << 2) | (b3 >> 6))))))%8))
        printf \\$(((_$((_codes + (63 & b3))))/64))$(((_$((_codes + (63 & b3))))/8%8))$(((_$((_codes + (63 & b3))))%8))
      fi
    fi
  done
  printf \\$(((__NEWLINE__)/64))$(((__NEWLINE__)/8%8))$(((__NEWLINE__)%8))
  endlet $1 b3 b2 b1
}

defarr _lut 256
: $((__t1 = c = 0))
_get() {
  let c; let __t1
  while _getchar __t1 ; [ $((c = __t1)) -ge 0 ] ; do
    if [ $((c = _$((_lut + c)))) -ge 0 ] ; then
      break
    fi
  done
  : $(($1 = c))
  endlet $1 __t1 c
}

: $((__t1 = c4 = c3 = c2 = c1 = i = 0))
_decode() {
  let i; let c1; let c2; let c3; let c4; let __t1
  i=0
  while [ $i -lt 256 ] ; do
    : $((_$((_lut + i)) = -1))
    : $((i += 1))
  done
  i=0
  while [ $i -lt 64 ] ; do
    : $((_$((_lut + 255 & _$((_codes + i)))) = i))
    : $((i += 1))
  done
  while _get __t1 ; [ $((c1 = __t1)) -ge 0 ] ; do
    if _get __t1 ; [ $((c2 = __t1)) -lt 0 ] ; then
      exit 1
    fi
    printf \\$((((c1 << 2) | (c2 >> 4))/64))$((((c1 << 2) | (c2 >> 4))/8%8))$((((c1 << 2) | (c2 >> 4))%8))
    if _get __t1 ; [ $((c3 = __t1)) -lt 0 ] ; then
      break
    fi
    printf \\$(((255 & ((c2 << 4) | (c3 >> 2)))/64))$(((255 & ((c2 << 4) | (c3 >> 2)))/8%8))$(((255 & ((c2 << 4) | (c3 >> 2)))%8))
    if _get __t1 ; [ $((c4 = __t1)) -lt 0 ] ; then
      break
    fi
    printf \\$(((255 & ((c3 << 6) | c4))/64))$(((255 & ((c3 << 6) | c4))/8%8))$(((255 & ((c3 << 6) | c4))%8))
  done
  endlet $1 __t1 c4 c3 c2 c1 i
}

: $((myargv = argc = 0))
_main() { let argc $2; let myargv $3
  if [ $argc = 1 ] ; then
    _encode __ 
  elif [ $argc = 2 ] && [ $((_$((_$((myargv + 1)) + 0)))) = $__MINUS__ ] && [ $((_$((_$((myargv + 1)) + 1)))) = $__d__ ] && [ $((_$((_$((myargv + 1)) + 2)))) = $__NUL__ ] ; then
    _decode __ 
  else
    exit 1
  fi
  : $(($1 = 0))
  endlet $1 myargv argc
}

# Character constants
readonly __NUL__=0
readonly __NEWLINE__=10
readonly __MINUS__=45
readonly __EQ__=61
readonly __d__=100
# Runtime library
__stdin_buf=
__stdin_buf16=
__stdin_buf256=
__stdin_line_ending=0 # Line ending, either -1 (EOF) or 10 ('\n')
__stdin_end=1
_getchar() {
  if [ -z "$__stdin_buf16" ] && [ $__stdin_end -eq 1 ] ; then          # need to get next line when buffer empty
    if [ $__stdin_line_ending -eq 1 ]; then  # Line is empty, return line ending
      : $(($1 = __stdin_line_ending))
      __stdin_line_ending=0                  # Reset line ending for next getchar call
      return
    fi
    __stdin_end=0
    IFS=                                            # don't split input
    if read -r __stdin_buf ; then                   # read next line into $__stdin_buf
      if [ -z "$__stdin_buf" ] ; then               # an empty line implies a newline character
        : $(($1 = 10))                              # next getchar call will read next line
        return
      fi
    else
      if [ -z "$__stdin_buf" ] ; then               # EOF reached when read fails
        : $(($1 = -1))
        return
      else
        __stdin_line_ending=-1
      fi
    fi
  fi
  if [ -z "$__stdin_buf256" ]; then
    if [ ${#__stdin_buf} -ge 256 ]; then
      __temp="${__stdin_buf#????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????}"
      __stdin_buf256="${__stdin_buf%"$__temp"}"
      __stdin_buf="$__temp"
    else
      __stdin_buf256="$__stdin_buf"
      __stdin_buf=
    fi
  fi
  if [ -z "$__stdin_buf16" ]; then
    if [ ${#__stdin_buf256} -ge 16 ]; then
      __temp="${__stdin_buf256#????????????????}"
      __stdin_buf16="${__stdin_buf256%"$__temp"}"
      __stdin_buf256="$__temp"
    else
      __stdin_buf16="$__stdin_buf256"
      __stdin_buf256=
      __stdin_end=1
    fi
  fi
  case "$__stdin_buf16" in
    " "*) : $(($1 = 32))  ;;
    "e"*) : $(($1 = 101)) ;;
    "="*) : $(($1 = 61))  ;;
    "t"*) : $(($1 = 116)) ;;
    ";"*) : $(($1 = 59))  ;;
    "i"*) : $(($1 = 105)) ;;
    ")"*) : $(($1 = 41))  ;;
    "("*) : $(($1 = 40))  ;;
    "n"*) : $(($1 = 110)) ;;
    "s"*) : $(($1 = 115)) ;;
    "l"*) : $(($1 = 108)) ;;
    "+"*) : $(($1 = 43))  ;;
    "p"*) : $(($1 = 112)) ;;
    "a"*) : $(($1 = 97))  ;;
    "r"*) : $(($1 = 114)) ;;
    "f"*) : $(($1 = 102)) ;;
    "d"*) : $(($1 = 100)) ;;
    "*"*) : $(($1 = 42))  ;;
    *)
      char_to_int "${__stdin_buf16%"${__stdin_buf16#?}"}"
      : $(($1 = __c))
      ;;
  esac
  __stdin_buf16=${__stdin_buf16#?}  # Remove the first character
}

# Convert a Shell string to a C string
unpack_string() {
  __str="$2"
  _malloc $1 $((${#__str} + 1))
  __ptr=$(($1))
  while [ -n "$__str" ] ; do
    # Remove first char from string
    __tail="${__str#?}"
    # Remove all but first char
    __char="${__str%"$__tail"}"
    # Convert char to ASCII
    char_to_int "$__char"
    # Write character to memory
    : $((_$__ptr = __c))
    # Continue with rest of string
    : $((__ptr += 1))
    __str="$__tail"
  done
  : $((_$__ptr = 0))
}

make_argv() {
  __argc=$1; shift;
  _malloc __argv $__argc # Allocate enough space for all elements. No need to initialize.
  __argv_ptr=$__argv

  while [ $# -ge 1 ]; do
    unpack_string _$__argv_ptr "$1"
    : $((__argv_ptr += 1))
    shift
  done
}

# Local variables
__=0
__SP=0
let() { # $1: variable name, $2: value (optional) 
  : $((__SP += 1)) $((__$__SP=$1)) # Push
  : $(($1=${2-0}))                 # Init
}
endlet() { # $1: return variable
           # $2...: function local variables
  __ret=$1 # Don't overwrite return value
  : $((__tmp = $__ret))
  while [ $# -ge 2 ]; do
    : $(($2 = __$__SP)) $((__SP -= 1)); # Pop
    shift;
  done
  : $(($__ret=__tmp))   # Restore return value
}

# Setup argc, argv
__argc_for_main=$(($# + 1))
make_argv $__argc_for_main "$0" $@; __argv_for_main=$__argv
__code=0; # Success exit code
_main __code $__argc_for_main $__argv_for_main; exit $__code
