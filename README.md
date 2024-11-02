# adlang

### overview
adlang is a weakly type programming language suited for simple, fast written applications. It offeres classes, closures, builtins and bootstrapping. Currently a REPL for adlang is implemented in C++, Python and Java with plans for developing a VM for compiling and executing ad sources. The VM is currently developed in Python and C++.

### examples
#### backtracking
```
n = 3
a = list(n, 0)

def check(k) {
    for (i = 0; i < k; i++) if (a[i] == a[k]) return false
    return true
}

def back(k) {
    if (k == n) {
        println(a)
    } else {
        a[k] = 0
        while (a[k] < n) {
            if (check(k)) back(k+1)
            a[k]++
        }
    }
}

back(0)
```

#### reading contents of a file
```
file = File("../../README.md", "r")
content = file.read()
println(content)
```

#### simple class
```
class Flag {
    def constructor(colors) {
        this.colors = colors
    }
}

class Country {
    flag = Flag("red", "white", "purple")
    def constructor(name) {
        this.name = name
    }
}

country = Country("Chocolate")
println(country.name)
println(country.flag.colors)
```
