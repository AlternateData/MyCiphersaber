My Ciphersaber Implementation!

WARNING: DO NOT USE THIS IMPLEMENTATION FOR ANYTHING SECURITY RELATED. THIS IMPLEMENTATION MIGHT BE UNSAFE. USE AT YOUR OWN RISK!

This is still a work in progress ... but it works!

TODOS:
- proper error handling if that is possible
    - replace occurences of fprintf(stderr, "[ERROR]: ....") with a macro
    - a lot of internal functions can fail and return null without any error message!
- Errors/Bugs
    - eleminate all valgrind errors
- Code style
    - choose code conventions and apply them.
    - mark internals as such better naming? using static?
