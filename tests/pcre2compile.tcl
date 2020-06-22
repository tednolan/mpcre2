# sum.test
 
package require tcltest
namespace import ::tcltest::*
 
# Software under test
source mpcre2.tcl
 
test pcre2compile {
    Test: Compiling regular expressions
} -body {
    mexec pcre2compile
} -result 0
 
cleanupTests
