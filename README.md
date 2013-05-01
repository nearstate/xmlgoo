##libxml2 &harr; node.js

This library exposes functionality implemented in libxml2 as functions and objects in node.js

####As simple as possible

##### A very small subset
Only the most basic functionality has been implemented. For example, there is no support for namespaces, nor is there support for manipulating CDATA sections.

##### Dumb C++
The C++ has been kept as simple as we possibly can. It looks more like a C program than a C++ program. In certain cases, simpler implementation has been preferred over the most performant algorithm. e.g. We used a linked-list of nodes instead of a balanced binary tree.

##### Well functionally tested
There is a suite of over 200 scripted tests which have been designed to help protect the code from faulting. 

##### Memory leak tested
One of the drivers for this library is that available adapters for libxml2 leak quite badly under moderate load. This code is being run in a production system which undergoes load tests to detect memory leaks.
