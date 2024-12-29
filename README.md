# Koby Programming Language
A dynamic programming language inspired by Kotlin, Ruby, and Lox, built in modern C++.

## Overview
Koby is an interpreted, multi-paradigm programming language that combines functional and imperative programming styles. It draws inspiration from several languages while maintaining its own unique character.

### Language Paradigms
- **Interpreted Language**:
  - Direct AST interpretation
  - Tree-walk interpreter for straightforward execution
  - Focus on developer experience over raw performance

- **Functional Programming Features**:
  - First-class functions and lambdas
  - Closures with proper variable capture
  - Functions as pure values
  - Implicit return of last expression

- **Imperative Features**:
  - Mutable variables
  - Control flow statements (if, while, for)
  - Break and continue support
  - Sequential execution
  - Side effects through native functions

### Language Influences
- Kotlin and Ruby for elegant and expressive syntax
- C++ for explicit syntax and closure behavior
- Lox's simplicity and educational foundations

## Language Features

### Values and Types
- **Numbers** (double-precision floating point)
  ```koby
  var n = 123.45;  // Numbers are always double
  ```

- **Strings**
  ```koby
  var s = "Hello, Koby!";  // UTF-8 strings
  ```

- **Booleans**
  ```koby
  var t = true;
  var f = false;
  ```

- **nil** (represents absence of value)
  ```koby
  var nothing = nil;
  ```

### Type Coercion and Operations
- Number + Number = Number (arithmetic)
- Any other combination with + will cast operands to strings and perform concatenation
```koby
1 + 2;        // 3
"a" + 1;      // "a1"
true + false;  // "truefalse"
```

### Variables
```koby
// Global variables
var globalVar = "I'm global";

// Local variables with block scope
{
    var localVar = "I'm local";
    var canShadow = "Shadows outer variables";
}

// Predicate-style naming with ?
var isEmpty? = true;
var hasValue? = false;
```

### Functions
Regular function declaration (implicit return of last expression):
```koby
fun add(a, b) {
    a + b;  // Returns this value
}

// Explicit return also supported
fun max(a, b) {
    if (a > b) return a;
    b;  // Last expression returned
}
```

Lambda expressions:
```koby
// Ruby-inspired lambda syntax with implicit return
var multiply = ->(x, y) { 
    x * y;  // Returns this value
};

// Lambda as function argument
fun twice(f, x) {
    f(f(x));
};
twice(->(x) { x + 1; }, 3);  // Returns 5
```

Functions with predicate names:
```koby 
fun isEven?(n) {
    n % 2 == 0;
}
```

### Closures and Variable Capture
Follows C++ closure behavior - captures reference to closest variable in scope:
```koby
var a = "outer";
{
    var a = "inner";
    var fn = ->() { put(a); };  // Captures inner 'a'
    fn();  // Prints "inner"
}

// More complex closure example
fun makeCounter() {
    var count = 0;
    return ->() {
        count = count + 1;
        count;
    };
}
var counter = makeCounter();
put(counter());  // 1
put(counter());  // 2
```

### Control Flow
```koby
// If statements
if (condition) {
    put("true branch");
} else {
    put("false branch");
}

// While loops
while (condition) {
    // Loop body;
}

// For loops 
for (var i = 0; i < 10; i = i + 1) {
    // Loop body;
}

// Break and continue
while (true) {
    if (done?) break;
    if (skip?) continue;
    // Do work;
}
```

### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Comparison**: `>`, `>=`, `<`, `<=`, `==`, `!=`
- **Logical**: `and`, `or`, `!` (with short-circuit evaluation)
- **String concatenation**: `+` (automatic type conversion)

Example of short-circuit evaluation:
```koby
var a = nil;
// Second condition not evaluated if first is false
if (a != nil and a.someMethod()) {
    put("not reached");
}
```

### Native Functions
- `put(value)` - Prints value to stdout with newline
- `get()` - Reads a line from stdin and returns it
- `now()` - Returns current time in seconds

More functions will be added in the future. Note that in REPL mode, `put` and `get`
is unavailable.

Examples:
```koby
put("Hello, World!");  // Output with newline
var name = get();      // Read user input
var time = now();      // Get current timestamp
```

### Syntax Notes
- Every statement must end with a semicolon, including:
  - Block bodies
  - Function declarations
  - Function calls
  - Return statements
- Function and lambda bodies implicitly return their last expression value
- Logical operators perform short-circuit evaluation
- Function/variable names can end with '?' for predicates
- Block scope follows C-style rules

## Technical Implementation

### Architecture
- Hand-written recursive descent parser
- Tree-walk interpreter
- AST-based execution
- Lexical scoping with proper closure support

### Scoping and Variables
- **Lexical Scoping**: Variables are resolved based on their location in the source code
- **Variable Shadowing**: Inner scopes can declare variables that shadow outer ones
```koby
var x = "outer";
{
    var x = "inner";  // Shadows outer x
    put(x);  // Prints "inner"
};
put(x);  // Prints "outer"
```

### Memory Management
- Based on C++ shared_ptr for automatic reference counting
- Proper cleanup of environments and closures
- No manual memory management required

### Core Components
1. **Scanner**
  - Hand-written lexer producing token stream
  - Support for multi-character tokens
  - Error recovery at token boundaries
  - Source location tracking

2. **Parser**
  - Recursive descent parser
  - Operator precedence handling
  - Error recovery with synchronization
  - AST generation

3. **AST**
  - Expression nodes
  - Statement nodes
  - Visitor pattern for traversal
  - Source location preservation

4. **Environment**
  - Scope chain management
  - Variable resolution
  - Closure support
  - Global/local separation

5. **Interpreter**
  - Tree-walk evaluation
  - Value type handling
  - Runtime error reporting
  - Native function support

### Error Handling
- Rich error messages with line numbers
- Differentiated compile-time and runtime errors
- Examples of detected errors:
  - Undefined variables
  - Invalid function calls
  - Type mismatches
  - Syntax errors

### Command-Line Interface
Koby supports three main commands:
```bash
koby help              # Show help information
koby run <filepath>    # Execute a Koby script file
koby repl             # Start interactive REPL session
```

## Building from Source
1. Build requirements:
  - Modern C++ compiler (C++17 or later)
  - CMake 3.10 or later

2. Build steps:
```bash
mkdir build && cd build
cmake ..
make
```

3. Running Koby:
```bash
# Show help
./koby help

# Run a script
./koby run script.kb

# Start REPL
./koby repl
```

## Future Enhancements

### Virtual Machine (Planned)
- Custom bytecode VM implementation
- Register-based instruction set
- JIT compilation potential
- Stack and heap management
- Optimized instruction dispatch

### Garbage Collection (Planned)
- Moving from reference counting to proper GC
- Mark-and-sweep collector
- Generational collection
- Precise GC with exact stack scanning
- Reduced pause times

### Language Features
- [ ] Classes and inheritance
- [ ] Standard library
- [ ] Module system
- [ ] More native functions
- [ ] Pattern matching
- [ ] Enhanced error handling
- [ ] Multiline string support
- [ ] Extension functions
- [ ] Better REPL with history and completion

### Performance Optimizations
- [ ] Function inlining
- [ ] Constant folding
- [ ] Dead code elimination
- [ ] Common subexpression elimination
- [ ] Tail call optimization

### Developer Tools
- [ ] Language server protocol (LSP) support
- [ ] Debugger
- [ ] Profiler
- [ ] Package manager
- [ ] Documentation generator

## License
MIT License

## Acknowledgments
- Based on Bob Nystrom's "Crafting Interpreters"
- Inspired by Kotlin, Ruby and C++