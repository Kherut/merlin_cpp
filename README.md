# Merlin C++ Library

## The library is not ready yet, do not use it! 
The problem is that *curlpp* lib misses something, but we don't know what yet. We're going to use *curl* alone (then it should work), so stay tuned.

An open-source C++ library for Merlin.

Firstly put the header file in the include path:
```
    # cp merlin.hpp /usr/include/
```

To compile the library into a shared object (.so), execute:
```
    $ g++ -fPIC -c -Wall merlin.cpp -lcurl -lcurlpp
    $ g++ -shared -fPIC -o libmerlin.so merlin.o
```

Now comes the *sudo* part:
```
    # mv libmerlin.so /usr/lib/
    # ldconfig

    // You may want to check if everything ran smoothly
    # ldconfig -p | grep libmerlin
```

And there you have it!
