# The Test For IAT Hook

## The Summary of the test

_This is just a code for my test of `IAT HOOK`, it is a x86 program._

_the function of the program:_

_1. find the address of `MessageBoxW` after loading `user32.dll`, by use API `LoadLibrary`._

_2.define the new function `MyMessageBox`, and get its address._

_3.develop a function `SetIATHook` to change the address of `MessageBoxW` to the address of `MyMessageBox`._

_4.call the `MessageBoxw` to test if the `SecUATHook` works._

_5.repair the address changed by me in `IAT`._

