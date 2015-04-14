def test1():
    a = 5 # this is a comment
    b = 3
    print "a+b= %d" % (a+b)

def test2(a=5, b=1):
    print "a+b= 0x%04x, a*b = 0x%04x" % (a+b, a*b)

def test3():
    print "range(4)",range(4)
    print "range(1, 8)",range(1, 8)
    print "range(8, 1, -1)",range(8, 1, -1)

def test4():
    l = ["parrot","cat","cow","dog"]
    for item in l:
        print "animal: %s" % item
    for item in sorted(l):
        print "  animal: %s" % item

def test5():
    d = {"a":34, "b":98, "c":17}
    print d
    print d.keys(), d.values()
    for k in d.keys():
        print "%s: %s" % (k, d[k])

class cls_A(object):
    def __init__(self,p1):
        print "cls_A::__init__(%s)" % p1
        self.p1 = p1
    def __repr__(self):
        return "cls_A, p1=%s" % self.p1
    def doit(self):
        "the famous doit method"
        print "cls_A::doit()"

def test6(p1=43):
    obj = cls_A(p1)
    print obj
    obj.doit()
    print dir(obj)
    help(obj.doit)

def test7():
    print [a*2 for a in range(8)]

def test8():
    if 7 > 8:
        print "Strange"
    else:
        print "7 > 8"

import textwrap
def test9():
    print "String methods:"
    print dir("a string")
    help("".endswith)

    print "Dictionary methods:"
    print dir({})

    print "\nList methods:"
    print textwrap.fill(", ".join(dir([])),80)

def test10():
    l = [chr(a) for a in range(ord("a"),ord("m"))]
    s = "".join(l)
    print l,s
    print "s[1]: %s" % s[1]
    print "s[-1]: %s" % s[-1]
    print "s[1:4]: %s" % s[1:4]
    print "s[5:1:-1]: %s" % s[5:1:-1]
    print "s[2::-1]: %s" % s[2::-1]
