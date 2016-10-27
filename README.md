# Cquate
An interpreted language for numeric computing

## Background
Cquate attemps to be a high-level language for numeric computing, while roughly following syntax norms of most modern languages. It started out as a hobby project many years ago, and after a few years I decided to just "finish" it. After much effort, here it is very much unfinished but delightfully functional (with limitations). It remains a work in progress.

Developing a functional syntax, parser, and interpreter is a major undertaking. The project could use some serious attention--and I hope to recruit some help from the universe. If you are interested in contributing, shoot me an email.

## Acknowledgements
Cquate uses the <a href="http://arma.sourceforge.net/">Armadillo library</a> for many matrix calculations.

## Building
Jump in to <code>/cq_container/cq_container</code> and run <code>make</code>, then jump into <code>/cquate</code> and again run <code>make</code>. The build has been tested with gcc 4.6.3. Please report build errors if you encounter them!

Building the container creates a matrix library <code>libcontainer.a</code>, which must exist in order for <code>cquate</code> to build.

I am aware there are several build warnings at this time.

## Quick start
Inline computing
```
cquate 'sin(pi/3)'
=
       0.866
```

Random numbers
```
cquate 'rand(1,4)'
=
       0.338 ,      0.331 ,       0.274 ,       0.746
```
Gaussian (normal) distribution
```
cquate 'grand(1,4)'
=
       -0.224 ,      -0.076 ,       0.606 ,       0.337
```
Variable assignment, matrix computations
```
cquate 'a=rand(2); inverse(a)'
=
       -0.231 ,      1.086
       1.309 ,      -0.467
```
Complex numbers
```
cquate 'exp(i*pi/2)'
=
       0.000 + i1.000
```
