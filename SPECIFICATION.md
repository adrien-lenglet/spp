# The S++ programming language specification

This specification is in three (3) parts:
- 1. [Addendum](#0-early-stage-addendums): pending parts of the specification, likely for proper insertion within the document. Still canonical and up-to-date. Can move around a bit.
- 2. [Core specification](#1-builtin-data-types): the main structure of the specification. The most stable part yet.
- 3. [Next design steps](#next-design-steps): what should come next in this document. Subject to change at any moment.

## 0. Early-stage addendums

This specification covers most of the specificities to S++ and design choices, so that a reasonble person could provide an implementation also based on the precedent that the closest programming languages (C++, TypeScript, Ruby) did set. This section lists areas not yet integrated into the specification that are not specific enough to be listed yet, but not obvious enough that they shall not be mentionned at all. Or simply parts of the specification for which their location is not determined yet. The reader is expected to be familiar with [the rest of this document](#1-builtin-data-types) before reading section 0.

### 0.1. Operator precedence

S++ defines the following operators, by top to bottom precedence and listed by brackets of equivalent operators. Note that expressions are enclosed in an hierarchical structure by `()` parentheses. The top-level expression does not need to be `()`-enclosed.
1. Member access operators, left-to-right
	- `[SUPER_VALUE].[MEMBER_IDENTIFIER]`: the dot operator has the uttermost precedence within an S++ expression. Members are accessed by reference, naturally.
2. Call operator, left-to-right
	- `[CALLABLE] [EXTRA] ([ARGUMENTS])`: not to be confused with a parentheses-enclosed sub-expression, denotes function-like call. `[EXTRA]` represents additional optional operators like `lossy` and `reinterpret`, which are only valid in a builtin type conversion situation.
	- `[ARRAY-LIKE] [[INDEX]]`: array subscript. Awkward notation, for an `array` and an `index`, subscript can be expressed as `array[index]`.
3. Upper unary operators, right-to-left
	- `!`: boolean NOT
	- `~`: binary NOT
	- `+`: unary PLUS. Returns `[ZERO] - [VALUE]`, with `[VALUE]` the value to the right and `[ZERO]` the one value for the type of `[VALUE]` (result of the `zero` class method).
	- `-`: unary MINUS. Returns `[ZERO] + [VALUE]`, with `[VALUE]` the value to the right and `[ZERO]` the one value for the type of `[VALUE]` (result of the `zero` class method).
	- `++`: increment operator. This can be placed to the left of a value, which will evaluate to the incremented value. Or can be placed to the right, which will evalute to the value right before executing the statement and will increment the value right after the execution of the statement. In both cases, the incrementation is in the amount of one (result of the `one` class method). For `[VALUE]++`, the incrementation operation is equivalent to `[VALUE] + <- type_of([VALUE]).one()`.
		- The choice of including this kind of operator is not trivial, but with standard tool-assisted code inspection, the existing confusion can be cleared up. This is clearly a feature that most experts would expect from a convenient language.
	- `--`: decrement operator. Follows the same rules as `++`, but decrementing of course. For `[VALUE]--`, the decrementation operation is equivalent to `[VALUE] - <- type_of([VALUE]).one()`.
4. Upper arithmetic associative operators, left-to-right
	- `*`: multiplication
	- `/`: division
	- `%`: modulo
5. Lower arithmetic associative operators, left-to-right
	- `+`: addition
	- `-` subtraction
7. Upper binary associative operators, left-to-right
	- `<<`: shifted-to-left-by operator
	- `>>`: shifted-to-right-by operator
6. Lower binary associative operators, left-to-right
	- `|`: binary OR
	- `^`: binary XOR
	- `&`: binary AND
8. Comparison operators, left-to-right. Note that this phase has an arbitrary arity: a `[MIN] _< value < [MAX]` value is perfectly valid in S++, and is equivalent to `[MIN] _< value and value < [MAX]`.
	- `=`: equal-to operator
	- `=/=`: different-from operator. As an alternative, you can use `not [VALUE_LEFT] = [VALUE_RIGHT]`, which is equivalent in simple cases
	- `>`: greater-than operator
	- `<`: lesser-than operator
	- `>_`: greater-than-or-equal-to operator
	- `_<`: lesser-than-or-equal-to operator
9. Boolean associative operators, left-to-right
	- `and`: boolean AND
	- `or`: boolean OR
	- `xor`: boolean XOR
9. Lower unary operators, right-to-left
	- `not`: boolean NOT, equivalent to `!` in its operation but with way lower precedence
11. Conditional ternary operators
	- An `if [PREDICATE] then [VALUE_IF_TRUE] else [VALUE_IF_FALSE]` expression, defined by the combined usage of `if`, `then` and `else` operators.
		- This is not to be confused with a conditional scope, which are differentiated by the absence of `()` enclosed predicate here
12. Assignment operators, right-to-left
	- `<-`: assignment operator
	- `<<-`: back-insert operator
	- `+ <-`: accumulation operator
	- `- <-`: decrease operator
	- `* <-`: multiply-by operator
	- `/ <-`: divide-by operator
	- `% <-`: remainder-of divide-by operator
	- `<< <-`: shift-to-left-by operator
	- `>> <-`: shift-to-right-by operator
	- `| <-`: binary-or-by operator
	- `^ <-`: binary-xor-by operator
	- `& <-`: binary-and-by operator

### 0.2. Operator overloading

Any scope can define operator overloads, which must be assigned to `function`s with the correct amount of arguments:
```spp
// Assume `T` is a `type`
T: type

class {
	m_privateValue: T

public:
	// Boolean NOT operator overload
	not <- function() {
		return !m_privateValue	// Return anything you like here, for real
	}

	// Multiplication operator overload
	* <- function(other: type_of(this)) {
		return m_privateValue * other.m_privateValue
	}

	// Call operator overload
	() <- function(arguments: ...) {
		// Do anything here
	}
}
```

Call overloading can be implemeted by using the `or` operator between multiple successive function values, which will yield a single overall function, for which all sub-functions are mutually exclusive. For more call complex overloading patterns, use the `is_a`/`is_an` builtin function, which returns a function returning whether the passed value can be bound to the type passed as the argument to `is_a`/`is_an`. Also, the `can_call` builtin function returns a function returning whether passed arguments to the passed value to `can_call` would succeed in their binding.

While operator and call overloading are permitted in S++, default argument values are not.

### 0.3. Destructors and move semantics

A scope can assign a `destructor`-named member which must be a 0-argument function and perform arbitrary work. `destructor` is guaranteed to be called right before the object goes out of scope, unless it has been copied from using the `move` builtin function. Copies (triggered by assignments) happen by simply copying the exact memory contents from source to target. Copying is authorized by default, but can be made illegal on a scope by specifying `assign <- delete`. All scopes with a defined `destructor` automatically have `assign <- delete`, unless it is being explicitly permitted using the `assign <- default` statement.

### 0.4. `this` value

`this` always refers to the scope it is being used into. Its usage is generally discouraged as it may produce undesired copies through attempts at aliasing. Just name variables right on the first declaration. However, `this` may come very handy in producing very generic compile-time code (type checking and so on).

### 0.5. Scope inspection & metaprogramming

It is possible to iterate through all accessible members of any scope using a `for` loop. The string of the public member identifier will be passed, which makes each scope essentially a compile-time string generator. The dot operator can be used with a string as the right value to allow for compile-time metaprogramming. This is valid for any value, including `this`.

## 1. Builtin data types

S++ agrees with C++ that compile-time computations and metaprogramming are powerful features. The user should be able to generate specialized code at compile-time, at the cost of increased binary size. S++ does not follow the more contemporary approach of dynamic languages of using runtime polymorphism based off a single set of generic functions and methods in the binary, as Java (as JVM-based languages such as Scala and Dart) and JavaScript (including TypeScript) do.

In this way, S++ is heavily templated and expects the user to use them. Even builtin data types are in fact templates to remove redundancy and improve elegance in its usage. All functions in S++ are better thought as templates, taking in either compile-time or deferred evaluated arguments.

### 1.1 Scalar data types

```spp
// Abstract scalar, cannot be instanciated but can be used as a type parameter
// to accept values which implement such interface
scalar <- interface;

// Abstract natural integer, encompasses any integer which can represent
// zero and strictly positive integers, but not strictly negative ones.
natural <- interface extends scalar;

// Abstract signed integer, encompasses any integer including the ones which
// can represent zero, positive and negative values.
integer <- interface extends natural;

// Abstract real number, can represent any positive number with a finite representation,
// is not required to present an absolute precision on any value. Negative values cannot
// be represented by this type.
positive_real <- interface extends scalar;

// Abstract real number, can represent any positive or negative number with a finite representation,
// is not required to present an absolute precision on any value.
real <- interface extends positive_real;

// Abstract transcendental number. Because of their nature, they have their own class of `scalar`s
// as they really cannot be treated as anything other than a symbol with special properties.
transcendental <- interface extends scalar;

// Boolean type
bool <- class implements scalar;

// Signed two's complement integer with bit count `N`
signed <- function(N: compile_time host_unsigned): class implements integer;

// Unsigned integer with bit count `N`
unsigned <- function(N: compile_time host_unsigned): class implements natural;

// Signed two's complement fixed-point scalar with bit count `IntegerBitCount + FractionalBitCount`
// A single bit from `IntegerBitCount` will be used to store the sign
fixed_signed <- function(IntegerBitCount: compile_time host_unsigned, FractionalBitCount: compile_time host_unsigned): class implements real;

// Unsigned fixed-point scalar with bit count `IntegerBitCount + FractionalBitCount`
fixed_unsigned <- function(IntegerBitCount: compile_time host_unsigned, FractionalBitCount: compile_time host_unsigned): class implements positive_real;

// IEEE 754 float with bit count `N`
ieee754_float <- float <- function(N: compile_time host_unsigned): class implements real

// Used by `minifloat`
unsigned_minifloat <- function(ExponentSize: compile_time host_unsigned, MantissaSize: compile_time host_unsigned, ExponentBias: compile_time host_unsigned): class implements positive_real;

// Used by `minifloat`
signed_minifloat <- function(ExponentSize: compile_time host_unsigned, MantissaSize: compile_time host_unsigned, ExponentBias: compile_time host_unsigned): class implements real;

// General-purpose float
minifloat <- function(IsSigned: compile_time bool, ExponentSize: compile_time host_unsigned, MantissaSize: compile_time host_unsigned, ExponentBias: compile_time host_unsigned) {
	if (IsSigned)
		return unsigned_minifloat(ExponentSize, MantissaSize, ExponentBias);
	else
		return signed_minifloat(ExponentSize, MantissaSize, ExponentBias);
}
```

The semicolon is optional in most circumstances within use of the language. Here, it denotes a declaration (not definition) of the class, similarly to C++. Objects declared without a definition cannot be exported out of a module. Here, these objects are declared & defined intrisically within the language.

All of these builtin data types may be inherited from. In addition, S++ presents commonly used scalar data types, builtin as well.

```spp
// Unsigned integer of the native register size of the platform the S++ compiler runs on
// Under a 64-bit system (as AMD64, ARM64, and so on), this is equivalent to `unsigned(64)`
// Under a 32-bit system (as IA-32, ARMv7, and so on), this is equivalent to `unsigned(32)`
// This is guaranteed to be wide enough to hold a pointer represented as an integer.
host_unsigned <- host_u <- native class implements natural;

// Signed two's complement integer of the native register size of the platform the S++ compiler runs on
// Under a 64-bit system (as AMD64, ARM64, and so on), this is equivalent to `signed(64)`
// Under a 32-bit system (as IA-32, ARMv7, and so on), this is equivalent to `signed(32)`
host_signed <- host_s <- native class implements integer;

// IEEE 754 float of the native register size of the platform the S++ compiler runs on
// Under a 64-bit system (as AMD64, ARM64, and so on), this is equivalent to `float(64)`
// Under a 32-bit system (as IA-32, ARMv7, and so on), this is equivalent to `float(32)`
host_float <- host_f <- native class implements real;

// Similar to `host_unsigned`, `host_signed` and `host_device` respectively,
// these types target the system the application is being built for (and not being built on)
device_unsigned <- dev_u <- native class implements natural;
device_signed <- dev_s <- native class implements integer;
device_float <- dev_f <- native class implements real;

// Equivalent to `signed` with the number in the identifier being `N`
s8 <- signed(8), s16 <- signed(16), s32 <- signed(32), s64 <- signed(64), s128 <- signed(128), s256 <- signed(256)
// Equivalent to `unsigned` with the number in the identifier being `N`
u8 <- unsigned(8), u16 <- unsigned(16), u32 <- unsigned(32), u64 <- unsigned(64), u128 <- unsigned(128), u256 <- unsigned(256)

// Equivalent to `ieee754_float` with the number in the identifier being `N`
// `f80` is the non-IEEE 754, x86 extended precision format
// All others are IEEE 754 standard
f16 <- ieee754_float(16), f32 <- ieee754_float(32), f64 <- ieee754_float(64), f128 <- ieee754_float(128), f256 <- ieee754_float(256)
f80 <- class implements real;
```

By default, S++ presents every value of these data types to every application, without any restriction. Non-natively supported data types are emulated in software at an increased runtime cost. There is an understanding that the user would go on and emulate these data types anyway, so S++ builts them in to present increased robustness.

Because many users will want to use non-software emulated data types exclusively, some keywords are introduced to issue a warning when software emulation would be necessary.  The `native` keywords hints that the following data type must be supported in hardware in its loading and storing. Warnings will be issued if a particular non-hardware supported operation is performed. Other `native`-based keywords are available:
- `native_load_store`: more relaxed version of `native`. Takes precedence over `native`. Only loading and storing of the data type must be supported in hardware. All arithmetic operations are allowed to be performed in software if not available in hardware.
- `non_native`: takes precedence over every other `native` keyword. Allows every software trick to emulate the data type if not available in hardware, without issuing any warning.

By default, the host and device-native data types are denoted as `native`. A `native`-based keyword can be prepended to every data type as default in the current module using the compile-time command: `default_native([NATIVE_KEYWORD])`. This cannot be changed for required modules unless modifying them.

### 1.2. Type conversions

S++ presents two operators to be combined to realise conversions between two different data types:
- `[TARGET_DATA_TYPE]([SOURCE_VALUE])`: lossless data conversion. `[SOURCE_VALUE]` must be entirely representable within `[TARGET_DATA_TYPE]`. This is the indended way of performing sign extension for two's complement integers. This is the default data conversion within the language, other types of conversion need to be explicitely called.
- `[TARGET_DATA_TYPE] lossy ([SOURCE_VALUE])`: lossy data conversion. `[TARGET_DATA_TYPE]` must be able to represent at least the sign of `[SOURCE_VALUE]`. A lossy conversion is allowed to truncate bits of the source that do not fit into the target data type.
- `[TARGET_DATA_TYPE] reinterpret ([SOURCE_VALUE])`: reinterpreted data conversion. `[TARGET_DATA_TYPE]` must be at least as wide in bits as `[SOURCE_VALUE]`. Bits will be taken as-is from the LSB of the source to the LSB of the target and the rest zero-padded.
- `[TARGET_DATA_TYPE] lossy reinterpret ([SOURCE_VALUE])`: reinterpreted lossy data conversion. The only requiment is that `[TARGET_DATA_TYPE]` and `[SOURCE_VALUE]` must both be scalars. Only `min(bits_of([TARGET_DATA_TYPE]), bits_of([SOURCE_DATA_TYPE])` will be taken from the LSB of the source to the LSB of the target, and the rest zero-padded.

From these definitions, the user will observe that even a `lossy` conversion will not be able to convert a signed data type to an unsigned one. There is then a pratical bias in `lossy` converting an unsigned scalar to a signed one when comparisons or accumulations between different data types are necessary.


### 1.3. Scalar literals

Here S++ shines with its unconventionality. Because of the implementation simplicity requirement, scalar literals are represented in little-endian notation.  
Little-endian is the standard on a S++ system. Conventional big-endian notation (denoted `arabic` here) is optional but not recommended for optimal compiling speed. The early versions of S++ will not include a `arabic` literal parser.

Because little-endian is basically not ever used for human interfaces, their usage must be prepended by a `*` symbol. For example, the arabic `10` decimal scalar for the number ten is represented by `*01` as standard decimal in S++, while both of them hold the exact same value. Both `arabic` and default notations are supported. By default, S++ standard functions output in little-endian decimal while always explicitly specifying the `*` prefix symbol.

Honestly, it is tempting to go all the way in there and not support `arabic` notation altogether. That would be in the spirit of the language. But that would make the language much less attractive to beginners so both options are redundently available.

As for the literals themselves, they have their own lossless unnamed type, just wide enough to represent the value with full precision. They can only be represented on the host during compile-time. Lossless compile-time operations involving multiple literals is possible. They may need to be `lossy` converted to the target type if not representable in binary.  
Futhermore, literals are in two sets: binary fixed-point unsigned integers and positive reals, the former being a subset of the latter. Bitwise operations can only be operated on binary fixed-point unsigned integers.

Let's explore how to express a positive literal (a negative one is defined by using the prefix `-` unary operator):

The general notation is `*([BASE])([FRACTION].)[INTEGER](x(-)[EXPONENT])`, where:
- `([BASE])` is decimal if not supplied, or may be `0h` for hexadecimal notation, `0o` for octal, `0q` for base-4 and `0b` for binary
- `([FRACTION].)` is the fractional part of the literal if the literal needs to be fractional, represented in the selected base. As this is little-endian, the fractional part starts off with the least significant digit to end with the digit right under the point in weight. There is no limit to the size of this part.
- `[INTEGER]` is the mandatory section of the literal, represented in the selected base. Starts with the least significant digit to end with the most significant digit. There is no limit to the size of this part but must feature at least a single digit.
- `(x(-)[EXPONENT])` is the optional exponent of the overall literal, which defaults to zero. The conventional `e` is not used, as it is reserved for hexadecimal (you read that right, you can exponentiate hexadecimal literals). The exponent follows the selected base. Useful to represent large numbers without any trouble.

In `arabic` notation, the general notation is `([BASE])INTEGER(.[FRACTION])(x(-)[EXPONENT])`.

Note that each digit sequence can contain a `_` which will get ignored while parsing. These characters can be inserted to increase readability of the literal.

The value of the literal (which is also the internal representation) is `(INTEGER + FRACTION * pow(BASE, -FRACTION_WIDTH)) * pow(BASE, EXPONENT_SIGN * EXPONENT)`, where `FRACTION_WIDTH` is the number of digits in `BASE` within `FRACTION`. All of the individual components are `natural`s, but the overall value may be promoted to a `positive_real` by evaluating it.  
The compile-time literal system should support complex arithmetic operations such as exponential and trigonometric ones (`pow`, `log(Base)`, `nth_root(N)`, `sqrt <- nth_root(2)`, `exp`, `ln <- log(e)`, `cos`, `sin`, `tan`, `arccos`, `arcsin` and `arctan`).  
`transcendental` constants with their exact value such as `pi`, `tau` and `e` are available. Scalar literals are guaranteed to be lossless, even under complex arithmetic, exponential or trigonometric operations. When losslessness cannot be guaranteed, the evaluation is deferred to the convertion to a non-literal scalar data type. These operations will be printed as-is if requested, without lossy evaluation. This is the only exception to the usually eager evaluation approach of S++.

- The implementation must only have bitwise operators, `+`, `-` and `*` being considered lossless literal operators. The conversion of the result of all others operators to a non-literal scalar must be considered `lossy`, and then force the user to use an explicit `lossy` conversion. Exceptions must not apply even if the result of the operation actually happens to be representable without loss for these operations. This case is assumed to be non-trivial and may cause significant discrepancies among implementations if would be left to their discretion: lossy behavior must be assumed anyway.
- Note that the lossy conversion of complex operations over scalar literals to a non-literal scalar data type is not guaranteed to be exact. It is left to the implementation, even if obviously it would be ideal to be able to compute each bit with absolute precision (and an extra one to round the LSB to nearest). Implementations based on floating-point math functions are heavily encouraged to perform computations on the most precise float available on the host. Hosts with fixed-point support only are encouraged to use at least 32 bits of fraction and 32 bits of integer, and ideally 64 bits of both if wide enough registers are available and that carry propagation is cheap enough. Applications are requested to assume that any implementation will provide a reasonable degree of precision and performance that the host can provide, that would make circumventing such a system unreasonable in the vast majority of use-cases. Regardless of machine-specific details, the application should expect the base implementation of S++ to detect remarkable operations on `transendental` and `real` scalars, such that they get converted to lossless equivalent operations that will likely result in more accuracy when evaluated by the platform-dependent implementation.

### 1.4. String literals

S++ declares string literals similarly to C++, using double quotes. Example: `"Hello world!"`. This results into a compile-time array of the current character scalar. An encoding keyword can be prepended to the string literal to change its encoding from the current one.

The default encoding is UTF-8 and will produce arrays of `unsigned(8)` (or equivalently, `u8`). The encoding in the current module can be changed using the compile-time command `default_encoding([ENCODING_KEYWORD])`. The full list of available encodings is:
- `encoding_utf8`: UTF-8, will yield an array of `unsigned(8)` wrapped into a `string_utf8` object. Access to this object yields `unsigned(32)` code points.
- `encoding_utf16`: UTF-16, will yield an array of `unsigned(16)` wrapped into a `string_utf16` object. Access to this object yields `unsigned(16)` code points.
- `encoding_ucs2`: UCS-2, will yield an array of `unsigned(16)` wrapped into a `string_utf16` object. Fixed-size characters, the code points are presented as-is in the array.
- `ascii`: ASCII encoding, will yield an array of `unsigned(8)` wrapped into a `string_ascii` object. Only the ASCII table can be encoded within the literal. Fixed-size characters, the code points are presented as-is in the array.

Both `string_utf8` and `string_ascii` implement `string_u8_non_zero`, which statically guarantees that no `u8` that encodes the code point sequence is zero.

Note that none of these encodings present a null terminating character. The `to_null_terminated` method of these strings should be used to obtain a C (or similar)-compatible string.

## 2. Scope, assignment and evaluation

### 2.1. Scopes

A scope defines a name space. A name space is a collection of unique identifiers which each resolve to an object. Non-declared identifiers resolve to the `undefined` value. Scopes can be nested one into each other by enclosing a sub-scope into `{}` braces. Within a sub-scope, identifiers in parent scopes can be resolved, however shadowing is not allowed (i.e. no object can be declared in a sub-scope which identifier is equivalent to the one of an object in any super scope).

A S++ module implicitly contains a top-level scope which gets enriched by the user when adding code to it. There is no distinction between a top-level scope and any sub-scope in its usage and properties. Scopes are objects like any other in the language and can be named using a variable assignment. Variables within a scope can be accessed using the `.` operator.

### 2.2. Variables

A variable is a binding point that is defined within a particular scope, which identifier is unique within that scope. All undeclared variables resolve to the `undefined` value within a scope. `undefined` can be seen as the primitive value and type of every variable, until an assignment overrides it with a primary type which can get enriched with subsequent assignments of various types. A variable has then from zero (undeclared) to an indefinite amount of possible types. S++ statically evaluates the set of possible types for a variable throughout its entire lifetime, and at runtime allocates memory space only once at declaration for the storage for the largest type along with storing the type identifier.

Scalar values (along with their aggregation into objects) can generally be compile-time evaluated as well as runtime evaluated. Some classes of values such as scopes, class/struct/sequence definitions and functions cannot be runtime evaluated. In general, compile-time only values and compile-time and runtime values should not be mixed in their assignment to the same variable. However in practice, the only error that could arise is the runtime usage of a compile-time only value. As long as the application stays in compile-time domain, there is no restriction to the types a variable can be enriched with. A variable staying exclusively in compile-time domain typically acquires a single type at most that gets replaced at each assignment, as other previous types get pruned out by static analysis. In other words, compile-time programming in S++ is almost indistinguishable from typical dynamically-typed programming from a language usage standpoint.

Variable assignment happens using the `=` operator. The left token must be the variable identifier and the expression for its value must lie on the right.

```spp
// At top-level scope
// `a` would resolve to `undefined` if used here

topLevel <- u32(0hBAAD_BEEF)

// Named scope called `app`
app <- {
	// `314` binary unsigned fixed-point scalar literal, converted to a non-literal scalar with no loss
	a <- dev_s(314)

	// `2.71` real with usage of a parent scope variable, yielding another real
	b <- 2.71 + topLevel
}

// `a` would still resolve to `undefined` if used here
// However, `app.a` and `app.b` could be resolved to scalar values
```

As mentioned, variables can acquire multiple types:

```spp
// `a` is `undefined` at this point

a <- true	// `a` is `bool` at this point

...		// Some runtime-dependent branch gets us to:
a <- u32(314)	// `a` is `bool | u32` at this point

...		// Some other runtime-dependent branch gets us to:
a <- void	// `a` is `bool | u32 | void` at this point
```

The `void` value is similar to `undefined` as they are both symbols which require no storage at runtime (aside from the type identifier), but deviates in its purpose and semantic.
- `undefined` can only be generated by language semantics from the absence of a binding to a variable, or more generally by the absence of user definitions in the usage of the language that would otherwise yield any sort of value (for example, a function implicitly returning at end of scope or returning with an empty expression). The user cannot assign a variable to `undefined`, as it would violate the requirement of `undefined` meaning zero user-defined type to a variable. However, the user can still generate `undefined` values in general (most likely for type checking, for example). In the context of a function, `undefined` is the type for any argument type and the type for any return type.
- `void` can only get user-generated. It is the conventional value to denote emptyness. An object of type `T` which may be or may not be defined could be expressed as a `T | void` type. The main practical difference with `undefined` is that the user explicitly assigns `void` to a variable, where `undefined` is the default value that cannot be assigned. The user is expected not to create other symbols that have the same practical usage as `void` and just use `void` instead. `void` is also the value returned by a function without an explicit returned value.

### 2.3. Evaluation

S++ is a eagerly evaluated language. S++ attempts to evaluate at compile-time every expression, at long at it does not result in implicit data or semantic loss.

To achieve that, S++ marks every variable with either a compile-time value during parsing of statements, or a deferred unknown value under circumstances where compile-time evaluation is not reasonably possible. Examples of promotion to deferred evaluation include:
- Usage of a runtime-defined data, such as reading the keyboard in real-time or acquiring data from a file or the network.
	- File or network acquisition can be made compile-time using the appropriate potential `compile_time` functions such as `read` (which works with a filepath or URL). `read` is a `run_time` function, which can be overriden to `compile_time` by specifying the same keyword in front of the value.
- Usage of not yet provided data in the compilation process: for example using an input variable of a function
	- If such variable happens to be compile-time evaluated, the target variable will be promoted from deferred back to compile-time evaluation.

When a function is invoked from the entry point of an application (at any scope depth), machine code is outputed only for variables for which assignment is still deferred, even after compile-time promotions.

Because deferred evaluation is likely to be the status of most user variables, at least while parsing the program ad hoc and not including instanciations at the entry point, comprehensive analysis is realized to enforce that individual modules of code will perform as expected before invoking them.

## 3. Primitives

### 3.1. Functions

Functions get declared using the `function` keyword, followed by a `()` enclosed, comma-separated list of arguments, then by optionally the return type, and then by `{}` enclosed sequence of statements. Each function argument consist of an identifier, followed by an optional comma defining the base type of the argument. That base type will then get enriched by invoking the function and binding each argument to a variable. Without any base type specified, the meaning is zero user-defined types (as for non-declared variables) and that then the argument will acquire the exact same type of the variable bound to it at invocation.

```spp
// `arg0` is of type `u32`, `arg1` is `undefined` and will acquire the type provided on invocation
aFunction <- function(arg0: u32, arg1): dev_u | void {
	...
}
```

Functions are best understood as templates. They do not produce machine code until fully invoked by runtime evaluation. Their arguments can be any primitive of the language.

Function arguments all get bound to their passed variable. Accessing an argument within a function has the exact same effect as accessing it within caller scope. It means that calling a function can enrich the type of passed arguments. In some way, S++ functions can be thought as typed macros for that precise behavior.

Functions can implicitly capture surrouding variables by simply referencing them. Capturing variables using a function puts constraints onto the lifetime of the function with regards to captured variables. Any captured variable must not be destroyed (go out of scope) before the capturing function gets destroyed. Because functions are essentially all inlined, this requirement is rather easy to satisfy, as for example returning a function does not necessarily mean destroying it nor variables that it could have captured.

Functions can be stored and typed. Their type begins with the `function` keyword, followed by a `()` enclosed, comma-separated list of arguments and then may feature the return type. Storing functions does not relax the lifetime requirements of captured variables mentionned in the earlier paragraph.

```spp
// `deviceIndex` is of type `u32`, `callback` is a function taking a single `deviceEvent` argument and not returning a value.
listenDevice <- function(deviceIndex: u32, callback: function(deviceEvent)) {
	...
}
```

A function not explicitly returning a value returns `void`. The type `undefined`, as for arguments, refers to any return type.  

A function may have its last argument denoting any number of arguments using the `...` type:
```spp
callProxyFunction <- function(args: ...) {
	return proxiedFunction(args...)
}
```

`...` must be explicitly denoted for variable argument count functions. `...` cannot be implicitly bound to an `undefined` argument type, that would be very confusing.  
These argument packs are opaque in nature and are meant to exactly bind variables to a proxy function. To unpack them, pass them to a receiving function using the `...` postfix operator. These argument packs cannot directly be stored. Though, they can be expanded to a single tuple by copy of each element: `[args...]`

The type `...` implements `__tuple_methods` discussed right below, while not being a tuple itself. Running `type_of` on an argument of type `...` yields `...` and not the associated tuple type.

### 3.2. Tuples

Tuples are a grouping of multiple values inside a array-like object which type may differ for each tuple member, and are all compile-time known.

Use the `[value0, value1, value2, ...]` syntax to create a tuple. Its type will be `[type_of(value0), type_of(value1), type_of(value2), ...]`, which is a compile-time tuple of types. The `type` of any tuple is `[...]`.

A tuple has the following members available, given `TupleType` the type of the tuple itself and `TUnion` the union of the all possible types making up the tuple:
```spp
TupleType: type
TUnion: type

__tuple_methods <- interface {
	// How many members in the tuple
	size: dev_u

	// Iterate through each member of the tuple, from the first to the last one.
	// Return `false` to stop iterating.
	iterate <- function(handler: function(value: TUnion) bool): void;

	// Get the index-th element in the tuple
	[] <- function(index: compile_time host_u): TupleType[index];

	// Set the index-th element in the tuple, also returns assigned value for single-statement, multiple-assignments
	[]<- <- function(index: compile_time host_u, value: TupleType[index]): TupleType[index];
}
```

### 3.3. Arrays

One core feature of S++ is arrays. Arrays are instantiated by specifying the size first, and then the type. In general, S++ syntax is based on the order of expected usage of the primitive, so naturally the type comes last. Arrays have a zero-argument constructor. The `type` of any array is `[]`.

```spp
array <- []u32()
```

Arrays in S++ are preferrably not with their size explicitly defined, except in circumstances where the size is well-known, static, and boundaries must be enforced. In general, it is more conveninent to let the static analysis figure out the largest size necessary. If static analysis declares the array unbounded, then runtime code generation is impossible.

There are two phases in using arrays with runtime size:
- 1. Back-insertion
- 2. Reading and writing into the array

Back-insertion is made by using the `<<-` operator on the array on the left and the value to insert on the right. On the first indexed read or write into the array, the back-insertion phase completes. A range for the maximum and minimum size of the array is issued. No more back-insertions are allowed. Indices for reading and writing are statically analysed and enforced to fit within the determined range of the minimum size of the array.

As for arrays with explicitly defined, compile-time boundaries, the two phases are:
- 1. Initialization
- 2. Reading and writing into the array

The intialization phase completes at the first indexed read into the array. Every member of the defined range must have been initialized when the initialization phase completes.

Defining an explicit runtime boundary on an array is illegal. Back-insertion patterns allowing array size inferrence must be used for runtime array sizes.

### 3.4. Objects

Objects like instances of `class`, `struct` or `sequence` get instantiated by calling their constructor (i.e. calling the type name) and their members accessed using the `.` operator. These three types all implement `object` (including a scope), which itself implements `type`.

#### 3.4.1 Sequences

Sequences get defined using the `sequence` keyword, followed by the constructor arguments (similarly to a function arguments declaration), followed by `implements` or `extends` keywords to build upon another object type. The sequence itself is declared much like a function body which scope can be externally accessed. The default visibility attribute for sequence scopes is `public`. The `type` of any sequence is `sequence`.

```spp
payloadGroup <- sequence(argExtendedFlags: u32 | void, argRegister: u32 | void) {
	hasExtendedFlags <- not argExtendedFlags = void
	hasRegister <- not argRegister = void

	if (hasExtendedFlags) {
		extendedFlags <- argExtendedFlags
	}
	if (hasRegister) {
		register <- argRegister
	}
}
```

A sequence can be thought of as a function which gets run once at construction, then has its scope retrieved and presented for subsequent reads and writes. Sub-scopes guarded by a conditional are mixed into the sequence scope only if the predicate can be evaluated exclusively from the sequence scope and captured compile-time variable. All other scopes are discarded, their contents destroyed at constructor completion and will not be subsequently presented.

It is illegal to perform a sequence write that would modify its structure and layout. All other writes are permitted. As for functions, captures are allowed with the same lifetime conditions. `sequence` assignment results in the copy of the contents of the `sequence`.

A sequence can safely represent complex objects in memory, as for example C strings. Here is an example of a constructor of a C string from a generic `u8`-composed string:

```spp
CString <- sequence(source: StringU8NonZero) {
private:
	isZero <- function(char: u8) {
		return char = 0
	}

	// `present` means the instance gets accessed first as `data` then falls back to the sequence itself
	// The function passed as argument checks if the passed member of the array marks the end of the array
	// `present` overrides the current visibility attribute (here, `private`) to make the member always accessible
	present data <- [isZero]u8

	for (char in source) {
		data <<- char
	}
	// It must be statically analysed that the last back-inserted member of the array will satisfy
	// the end-of-array function `isZero`, and that each previous member do not.
	data <<- 0
}
```

Such `CString` can then be iterated over using a `for` loop for example, which will yield each character in the `data` member of the `sequence`, not including the null terminating character.

#### 3.4.2 Structs

Structs are similar to `sequence`s, except of course that their keyword is `struct`. The default visibility attribute for struct scopes is `public`. The `type` of any struct is `struct`.

Structs cannot embed any runtime variable-size member inside of them. The exact layout of a given `struct` must be unique for every possible instance and known at compile-time. That means that member arrays which size depends on a runtime value or on a constructor argument are not allowed inside a `struct`. More obviously, a struct scope cannot be enriched by any conditional scope as well. A struct can still capture variables and is subject to the same lifetime requirements as `function`s and `sequence`s.

Structs match 1-to-1 C++ `struct`s layout, except of course that they cannot represent pointers or references. By the capture exception on `struct` layout variability, S++ allows the user to define parameterized structs through a compile-time function:

```spp
structWithArray <- function(T: type, N: compile_time dev_u) {
	return struct(defaultValue: T) {
		array <- [N]T()

		for (i in count(N)) {
			array[i] <- defaultValue
		}
	}
}
```

#### 3.4.3 Classes

Classes share every attribute of `struct`s, except that their keyword is `class` and that the default visibility attribute for class scopes is `private`. The `type` of any class is `class`.

## 4. Static analysis

S++ relies heavily on static analysis to enforce safe behavior. Static analysis is performed on each deferred or runtime executed statement (compile-time values by definition, do not need analysis). Static analysis happens in two consecutive phases:
- 1. Type set evaluation and inference of the set of possible values at each assignment and conditional, along with statement e=xecution count deduction in parametrized scope
- 2. Statement execution count usage

Phase 1. is used to narrow down the state of variables at each statement of the program, for futher analysis and scalar boundary checking.  
Phase 2. is necessary in circumstances where the execution count of a specific statement needs to be evaluated. For example, runtime-sized arrays need to compute how many times does the back-insertion statement gets executed at most, to properly allocate the buffer at variable declaration. This phase relies on the analysis of phase 1. and iterator analysis usage to output a range of array size.

### 4.1. Type set/possible value set/iterator analysis

At each assignment of a value to a given variable, its type gets enriched with that possibly new type being added to the set of possible types. This was already discussed at section 2.2. Moreover, at each assignment of a variable and until new assignement, the possible set of values is determined, only valid for the scope where the variable is available onwards. A mix of boolean algebra and arithmetic is performed to deduct these sets.

Conditional blocks will narrow down the possible assigned values of a variable. A subset of possibly assigned values will be available on top of the conditionally executed scope and discarded at the end of the scope. This is contrary to assignments for which possible value set will persist afterwards and until variable destruction or new assignment. Conditional blocks narrowing-down are localised to their scope where assignments are localised to the variable scope. Both rules are relatively intuitive in their behavior.

```spp
// Assume `N` in scope, and is a `natural`
N: natural

f <- function(x: natural) {
	// At this point in analysis, `0 _< x < +inf` (`natural` definition, would be `unknown` with `x` as `undefined`)

	if (x >_ 0 && x < N) {
		// `0 _< x < N` (intersection of assigned `0 _< x < +inf` and checked `0 _< x < N`)

		// Statements not assigning `x`
		...
	} else {
		// `x >_ N` (intersection of assigned `0 _< x < +inf` and checked `x >_ N`)

		// Statements not assigning `x`
		...
	}

	// At this point, `x` is back to `0 _< x < +inf` as we are outside of any conditional

	if (x >_ 0 && x < N * 2) {
		// `0 _< x < N * 2`
		x <- N / 2
	} else {
		// `x >_ N`
		x <- 0
	}

	// `x = N / 2 | x = 0`
}
```

Iterators which depend on compile-time values get fully unrolled to a sequence of scopes. Deferred iterators produce an evaluated range of iteration count as well as a range of iterator variable value overall. Past the iterator, assigned variables have a range of possible values being the union of their possible values at each exit point of the iterator and possibly also unioned with the range of possible values before the iterator if zero iteration is possible.

```spp
// `count` is builtin, but presented here for completeness
count <- function(max: natural) {
	`0 _< max < +inf`

	// `i = 0`, `1` time
	i <- 0
	while (i < max) {
		// `max` iterations

		// `0 _< i < max`
		yield i

		// `0 _< i _< max`
		i + <- 1
	}
}

f <- function(x: natural, max: natural) {
	// `0 _< x < +inf`

	// `x = 5`
	x <- 5

	for (i in count(max)) {
		// Analysis of iterator `count` invocation:
		// `max` iterations, and  `0 _< i < max`

		if (i & 1 = 0) {
			`5 _< x < 5 + max`
			x + <- 1
		}
	}

	// `5 _< x _< 5 + max`
}
```

From a lower-level perspective, the actual analysed code is the following, as the iterator is simply inlined:

```spp
f <- function(x: natural, max: natural) {
	// `0 _< x < +inf`

	// `x = 5`
	x <- 5

	// `__count__i = 0`, `1` time
	__count__i <- 0

	while (i < max) {
		if (__count__i & 1 = 0) {
			x + <- 1
		}

		__count__i + <- 1

		// `__count__i` gets incremented with `1` units at each iteration
		// Given that `__count__i = 0` before the iterator, it means `max` iterations
		// Range of `__count__i` is then: `0 _< __count__i < max`
	}

	// `5 _< x _< 5 + max`
}
```

Let's assume that the caller of the iterator does actually modify the iterator conditionally, with a more baroque first value:

```spp
f <- function(x: natural, max: natural) {
	// `0 _< x < +inf`

	// `x = 5`
	x <- 5

	// `__count__i = 2`, `1` time
	__count__i <- 2

	while (i < max) {
		if (__count__i & 1 = 0) {
			x + <- 1
			__count__i + <- 2
		}

		__count__i + <- 1

		// `__count__i` gets incremented with `1` to `3` units at each iteration
		// Given that `__count__i = 2` before the iterator, it means at least `ceil((max - 2) / 3)` and at most `max - 2` iterations
		// Range of `__count__i` is then: `2 _< __count__i < 2 + (max - 2) * 3`
		// `__count__i` cannot be assigned in a sub-iterator or else this analysis fails.
	}

	// `5 _< x _< 5 + max`
}
```

Finally, let's see a two-dimensionnal approach with zero-based iterators:

```spp
f <- function(x: natural, width: natural, height: natural) {
	// `0 _< x < +inf`

	// `x = 5`
	x <- 5

	for (i in count(height)) {
		// `height` iterations, `0 _< i < height`

		for (j in count(width)) {
			// `width` iterations, `0 _< j < width`

			// Overall this is executed `height * width` times
			x++
		}
	}

	// `x = 5 + height * width`
}
```

The sub-iterator `count(width)` is ignored for first-level iterator `count(height)` analysis, as `i` is not assigned within the sub-iterator. If `i` was to be assigned within the sub-iterator, both iterators analysis would fail.  
The iterator analysis system only supports strictly increasing natural iterators.

When the analysis resolves to a set of possible values with a single value in it, the implementation will not prune full runtime code with every iterator out and instead replace it with the inferred expression. Useless code removal is considered expensive to perform and the user should know better. The user is expected to use that presented analysis to identify and remove useless code themselves. No warning will be issued by the S++ compiler, as the accurate relevancy of some piece of code would have needed to be evaluated at that point, which is virtually equivalent to performing useless code removal to begin with.

### 4.2. Statement execution count usage and analysis leverage

Using the comprehensive analysis discussed in 4.1., runtime-sized arrays can be figured out. The sum of every back-insertion execution maximum count is performed, which gives the upper limit of elements for the array. The type in the array also has a maximum size required to be stored memory. The actual number of bytes allocated for the runtime-sized array is the maximum size of the array multiplied by the maximum size of the array element.

In general, access to arrays is checked using the deducted range of the index. The index must be strictly within the minimum size of the array. Iterating over the full size of the array is always permitted, but the user should understand it is not a reasonable way of providing index access between the minimum size and the maximum size. The language and deduction capabilities should be improved so that the minimum size doesn't get underestimated in these use-cases prompting for extended index support.

## 5. Stacks

### Preamble and discussion around state-of-the-art

All runtime objects can be allocated on the execution stack. The execution stack can grow to a large extent: stack address bit count is configured on the `entry_point` of the process, and can take up to the vast majority of the adress space for the most extreme applications. This should account for a large amount of practical use-cases. However, once allocated, this space is obviously static and there is no reasonable way to dynamically extend an array previously allocated and initialized. S++ presents ways to easily allocate arrays of the arbitrarily correct size for the purpose of immediatly using it, but no mean to accomodate for future unpredictable size requirements.

This is a realistic need that is encountered by most applications that run indefinitely and manage arbitrarily large amounts of data. Even purely processing applications often need dynamic working space that can be readily extended on demand. In traditional execution environments, this need is commonly addressed by a continuous, extensible chunk of memory called the "heap". The heap manages a collection of chunks of memory, which is left to the allocator discretion.

The heap usually brings two issues that are usually not reasonably definitely solved in a cheap manner, assuming any general-purpose allocator:
- Potentially very expensive and unpredictable allocation execution time. There is no telling how much time will be needed to allocate a certain chunk of memory. The allocator may need to walk through a large portion of the heap to find suitable available segments of memory. This is unacceptable in most real-time scenarios. As the heap gets fragmented through the application lifetime, allocation times get more and more unpredictable going forward.
- Potentially low data locality. There is no reasonable way of enforcing that two distincts chunks of memory stay as close as possible in the addressing space. As the application goes on and fragmentation increases, it's very likely that two allocations made at different points in time will be separated from each other in memory. In some real-time scenarios with significant stress, this will make the overall performance non-deterministic. Even the random delays incurred by seemingly random cache misses may prove unacceptable given the requirements.

Because of these two issues, the most extreme applications often need to have custom allocators to fit their needs and enforce data locality, as general-purpose allocators are usually application-agnostic and unspecialized. A perhaps less brutal solution may be to pool chunks of memory to enforce locality and high-availability, but this also involves implementing an allocator for resources within the pool, not to mention that designing minimal memory and execution overhead can be often challenging for a particular need.

S++ acknowledges that while the heap is a convenient tool for naive and not demanding applications, its structural attributes cannot be reconciled with reliable peak-performance in a general-purpose way.  

### 5.1. S++ approach and overview

S++ proposes no heap, but instead a system permitting the allocation of stacks with arbitrary addressing spaces, that can grow at a minimal and consistent cost. S++ stacks are memory-safe, and allow the application to allocate objects of an arbitrary type (even complex, polymorphic ones) on top of it. S++ stacks can be split on a current top of stack, and the new section allocated on it and safely destroyed by dropping the split handle.

On top of it, stacks are suitable to have a new concept of "segments" allocated on top of them. Each segment has its beginning address aligned with its size (which must be a power of 2), and can contain objects which hold special "short" references, which are very compact indices within the segment. Objects containing short references must be allocated in-place within a segment (which can be seen as fixed-sized stacks) of the same size than the exact addressing capability of the reference in question.

S++ also proposes "long" references which can, within a particular stack, reference any object present in the current stack or any other one. Long references usually need to store a full address and are then considered memory-expensive. S++ provides means to enforce that no reference, either long or short, can be left "dangling" (i.e. the underlying object gets destroyed before the reference does), majorly by using static analysis of stack sections (which have been "split"). Said stack sections control the lifetime of objects they hold, and then tracking their lifetime compared to the one of references pointing to them is sufficient. References and objects can be abstracted away in their lifetime by the stack segment they are allocated into.

### 5.2. Stack creation

A `stack` is a runtime-only object. Compile-time objects do not need to use `stack`s, as they are effectively relaxed in their growth, typing opportunities and can do whatever they want. A `stack` is purely a runtime trick to guarantee memory safety and locality on runtime data structures with a complexity vastly superior to the static analysis capabilities. Some amount of analysis is still done on top these concepts, which allows the inferrence of the high-level split section memory structure of the stack to be done.

A new stack can be allocated by contructing a `stack` object:

```spp
// `AddressBitCount` is the range of virtual memory to allocate for the stack, only limited by total virtual memory space
// `T` is the type of objects that can be allocated on top of the stack: any `type` is acceptable, including the ones with multiple possibilities
// `AlignmentBitCount` is the address alignment of the base of the stack. If a stack is created for allocating segments of address bit count `N`,
// pass at least `N` here to avoid padding insertion.
stack <- class(AddressBitCount: compile_time dev_u, T: type, AlignmentBitCount: compile_time dev_u <- default_page_size) {
public:
	// Iterate through the current sub-stack from the base to the top,
	// return `false` to stop iterating
	iterate <- function(handler: function(value: T) bool): void;

	// Get the long reference class. This is guaranteed to take the same space as a `device_unsigned`.
	// The result `present`s the underlying referenced value,
	// making using the reference roughly equivalent to directly using the underlying value.
	// Instantiate the resulting class, passing a value stored in `this`, to keep a reference of such value.
	// The stack producing this reference type must not be destroyed before any instance of the reference:
	// this is statically asserted by the compiler.
	ref <- function(ToRefType: type): class;


	// Methods below can be called only if the stack top is currently owned by `this`

	// Allocate `value` on top of the stack, and get a reference to that placed value within the stack
	<<- <- function(value: T): ref(T);

	// Split the stack, the return value now owns the stack top (which becomes its stack base) until destroyed
	// At return value destruction, the stack top of `this` is the same as it was when `split` was called
	// Even if `SubT = T`, `this.iterate` will still stop before the result base
	split <- function(SubT: type <- T): stack;

	// Return all physical memory past the top to the runtime
	trim <- function();
}
```

Please note that invocations of the `stack` constructor can only be performed by the main thread, not to inhibit static analysis. More info below and at `6.4. Threads & concurrency`.

The argument `AddressBitCount` specifies the maximum size of the allocated stack, and will result in that addressing space being fully allocated for that `stack`. Note that the actual physical addressing space should not be fully allocated if virtual addressing is available on the device. The pages or segments not yet allocated should be marked so that an interruption is generated on an access on these ranges, resulting in the runtime physically allocating the accessed pages on the fly. A binary exponential allocation strategy is recommended, starting with a single page and doubling at each range access missing the physical memory. Exponentiation bases smaller than 2 are permitted on platforms where physical memory is limited, at the cost of more frequent runtime interventions and less trivial allocation size computation.

Overally, the `AddressBitCount` only indicates the range of virtual memory to allocate for the stack. The application should make sure that that range is generous and reasonably not ever filled all the way. The total available virtual memory range is perhaps quite a lot less plentiful than one may imagine: for example the AMD64 architecture only relies on a minimum of 48 bits of virtual addressing space, despite having 64 bits wide registers. That would mean, assuming the entire addressing space is filled with 4GiB stacks with no overhead, that "only" 65536 of these stacks could be allocated at any time inside a single process. The S++ implementation must determine the location of each stack at compile-time and issue an error if the application exceeds the addressable space available onto the device. Applications are expected to generously overestimate the required address space for the vast majority of use-cases, and to get ported to specific platforms with more aggressive memory allocation strategies if necessary. Conventional tools to generate more portable `AddressBitCount`s will be discussed in the future, possibly removing this argument to `stack` and replacing it with a more abstract enumeration value of some sort.

The application should expect the runtime to reserve at least a single page of virtual memory after the virtual memory range of each stack, in order to detect stack overflows and then protect the application from undefined behavior. The runtime guarantees that each stack base is aligned on a page of virtual memory.

A stack can be split into sub-stacks, allowing the destruction of continuous segments of the stack from the top by droppping the sub-stack handle. Dropping a sub-stack handle does not result into the physical memory being released back to the runtime, the explicit `trim` method should be called onto the top stack handle to deallocate the pages.

Any object of the supplied type `T` can be allocated on top of the stack to make it grow. The allocated space can only be released by destroying the sub-stack the object has been allocated onto. Sub-stacks are hierarchical by nature: sub-stacks get defined by splitting the top sub-stack, the overall stack grows object by object on the top sub-stack and the overall stack "pops" by destroying top sub-stacks. This matches 1-to-1 the lifecycle of sub-stacks as inferred by their appearance in scope and their scope end-of-definition.

### 5.3. Segments

The purpose of segments is to define a localized address space which can be addressed using relatively short opaque scalars (called "indices" here). A `segment` is a parametrized type that takes as argument the number of bits of the address space:

```spp
__segment_methods <- interface {
	// Get the short reference class. This is guaranteed to take the same space as a `unsigned(AddressBitCount)`.
	// The result `present`s the underlying referenced value,
	// making using the reference roughly equivalent to directly using the underlying value.
	// Instantiate the resulting class, passing a value stored in `this` (segment), to keep a reference of such value.
	// This reference must be stored in the same segment that produced it.
	ref <- function(ToRefType: type): class;

	// Returns whether or not a value could fit within the remaining stack
	// This must be explicitly called if the compiler cannot statically assert
	// that a value would fit within the segment, then making the assertion at
	// runtime. It is not possible to compile code that does not assert that a certain
	// value would fit within the segment.
	canAllocate(value: T): bool;

	// Allocate `value` at the end of used space within the segment,
	// and get a reference to that placed value within the stack
	<<- <- function(value: T): ref(T);
}

segment <- function(AddressBitCount: compile_time dev_u): class implements __segment_methods;
```

Contrary to `stack`, `AddressBitCount` does not indicate a potential maximum size but the full size of the `segment` at allocation. In addition to that, `segment`s are always aligned on their size. This allows trivial addressing from only an address within the segment and the index from the beginning of the segment (also knowing the size of the segment at compile-time).

`segment`s can be created in any context: on the execution stack, at compile-time, on any `stack`, ...  
In general, segments of a certain address width for a particular purpose should be allocated on a dedicated `stack` with the alignement being at least the segment's `AddressBitCount` only. Because of the alignment requirement, padding will probably be introduced in these other contexts. This padding costs memory that will not be promoted to anything else, because the user is expected to know better. Segment allocation results in checking whether the allocated segment would fit in its designated storage location, which has a slight runtime cost.

The short references that are created within segments can only be dereferenced when stored on the same referenced segment. Storing these short references anywhere else (temporarily) is permitted, but they won't be dereferenceable from there. Short references stored inside a segment are assumed to be correct, and short references from another segment cannot be stored within a segment.

Overall, segments are a safe way to produce complex memory structures with low pointer memory overhead.

## 6. Execution context

We have defined many primitives and concepts so far. They interact with each other within a vacuum in some way. We are going to define how these can form a process and interact with the Operating System.

### 6.1. Modules

S++ takes large inspiration from ECMAScript modules. By default, all objects within a .spp file (called a module) cannot be accessed outside of said module.

To expose objects, the `export` keyword is used:

```spp
// `lib.spp`

export aFunction <- function(a, b, c) {
	...
}

export bValue <- 3.14
```

To import objects from another module, the `import` keyword is used:

```spp
// `b.spp`

// Comma
import aFunction, bValue from "lib"
```

The module path must not include the `.spp` file extension, though the module file itself must have such extension. A plain string will be assumed to be relative to the module including the entry-point of the program. The module path can be made relative using the `.` operator (example: `./path/to/relative/module`), and will be assumed to be relative to the directory containing the file of the module performing the `import`.  
`*` can be imported instead of comma-separating each object to import, simply importing all exported objects within the current module.

Circular dependencies are illegal in S++. A module cannot implicitly import itself by importing dependencies. The same module can be imported as many times as required though, as long as this count is finite.

### 6.2. Entry point

To actually produce an executable and run it, the S++ file defining the module `export`ing exactly one `entry_point` must be invoked.

`entry_point` is a builtin keyword that can be used as follows to create an entry-point:

```spp
// Any compile-time `dev_u` can directly be passed to `entry_point`,
// here represented for completeness.
MainStackAddressBitCount: compile_time dev_u

// `args` can be replaced by any number of arguments,
// the single variable-length argument of type `...` is only shown as a general example.
// The arguments part can be omitted altogether, which is the same as having zero argument.
export main <- entry_point(MainStackAddressBitCount)(args: ...) {
	...
	return some_value
}
```

A scope must be passed to the entry-point, which will be run as the instantiated process. Within the scope, any object of the language or defined using the language can be used.  
`MainStackAddressBitCount` define the maximum size of the execution stack for the main thread of the process. The same precautions as required to figure out the size of a manually created `stack` apply here: see `5.2. Stack creation`.

Entry-points of S++ programs are strongly typed, as any other S++ function.  
S++ programs are expected to raise exceptions using the `throw` keyword to indicate runtime errors.  
S++ programs can return any runtime-representable value they want, which can then be used by the user invoking the program (probably storing it in a compile-time variable or chaining it as a call to another program).

In an interactive S++ shell, invoking a program defined by a file module happens by calling the loaded path to the file module, containing exactly one exported entry-point:
```spp
result <- load("/path/to/file/module")(arg0, arg1, arg2, ...)
```

As for module `import`s, the `.spp` extension must not be indicated. The `.` operator can be used to reference module files relatively to the current working directory.

In a simplified S++ shell, the same behavior can be achieved with:
```spp
result <- /path/to/file/module arg0 arg1 arg2 ...
```

The simplified S++ shell is designed to be competitive with popular Unix-like shells such as Bash or Zsh (and friends) in terms of typing speed and operator overhead, while providing comprehensive and accurate code completion on the fly. Tokens usage is inferred on the context, allowing to run a S++ program by simply typing out its module path and providing space-separated arguments. Simplified S++ is meant to be write-only, typed on the fly and takes inspiration from popular functional languages such as Haskell and Lisp. Expressions are parentheses-enclosed and here in particular, strings do not need to have double-quote characters to be written out. When a value from an enum is expected, simplified S++ will happily suggest and accept any identifier which is part of the enum in question. Types (shallowly as well as deeply) referenced by the entry-point arguments are automatically suggested and available in the context of invoking such entry-point. Scalar literals can be inputed exactly like in standard S++.

### 6.3. Exceptions

Exceptions are an easy way to propagate an error down to an error catcher. To raise an exception, use the `throw` keyword followed by the error value to be raised. Any language object qualifies, but runtime exceptions raisers/catchers must use runtime values only.

A post-scope `catch` scope must be defined to catch an exception:

```spp
{
	throw "This is a significant error!"
} catch (error) {
	// `error` is of type `string_utf8`

	std_out <<- "Exception caught: " <<- error <<- end_line
	// Rethrow exception, to propagate it further
	throw error
}
```

In the spirit of S++, exceptions are caught similarly to the way values are passed to a function. Declaring a `catch` scope in fact acquires any possible type that may be thrown inside the regular scope. A type for the caught value can be declared, that will be enriched by all possible types that can be raised. As for function arguments, a supertype of potential raised types can be explicitly used.

Exceptions in S++ are best seen as the most powerful control flow feature of the language. They may generate a lot of code for the return path back to the catch scope, so they better be used mindfully. The implementation must minimize code size over performance, ideally by sharing destructor code on a per relevant exception-scope basis.

### 6.4. Threads & concurrency

There is no way around it: S++ must provide builtin concurrency support.

S++ proposes race-condition free primitives, so that not only S++ stays memory-safe, but also keeps the user out of concurrency trouble.  
The base primitive of S++ to achieve that is the `thread`, which are created like so:

```spp
// Any compile-time `dev_u` can directly be passed to `entry_point`,
// here represented for completeness.
MainStackAddressBitCount: compile_time dev_u

threadHandle <- thread(MainStackAddressBitCount) {
	// Thread work...
	...
}

// Must explicitly `join` the thread before destroying its handle.
// This is to avoid any confusion as to the possibility of orphaned threads.
// A S++ program not joining a `thread` going out of scope cannot build.
threadHandle.join()
```

Note that because each created thread actually needs to create a new execution stack in the virtual memory space (see `6.2. Entry point`), only the main thread is allowed to create threads. This is to be able to statically infer the required memory layout at any point within the program. Aside from that, there is not much preventing us from spawning threads anywhere we want.

Threads can capture any compile-time value very much like a `function` can do. Threads can also capture runtime variables, but only the ones wrapped around a `concurrently` object:

```spp
someConstant <- 314

// `concurrently` takes a `type` as argument,
// the resulting type can then be constructed
valueToInc <- concurrently(u32)(0)

threadHandle <- thread(32) {
	valueToInc.manipulate(function(value) {
		for (i in count(someConstant))
			value + <- i
	})
}

threadHandle.join()

std_out <<- "Thread result: " <<- valueToInc.get() <<- end_line
```

Similarly to functions, threads must have `join`'d before any captured variable can be destroyed.

Threads incur a little cute exception in language to `stack`s. Stacks (including any sub-stack) cannot be split during the scope of capture by a thread. Aside from that, allocations can be made on top of a stack as usual business, be it from main thread or the spawned thread, as such concurrent access must happen `concurrently` and is then guaranteed safe.

# Next design steps

- Abstract away bit count of stacks, use weight instead and leverage compile-time layout analysis to produce a per-device optimal stack layout
- Easy-to-use low-latency inter-thread signals (effectively what `std::condition_variable` allows the user to do)
- Boost-like `io_context` thread to perform asynchronous work of asynchronously-oriented applications
- `async` feature in the spirit of ECMAScript's `Promise`s
- Primitives to handle endianness, such conversions in files, along with unique version specification
	- Probably a all-in-one package to serialize S++ runtime primitives