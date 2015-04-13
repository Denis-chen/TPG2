import array

def add(a,b):
	return a+b

x = add(1,2)
print x

for x in range(0, 5):
	print "loop iteration: %d" % x

a, b = 2, 3
if a > b:
	a = b
else:
	b = a


a = array.array('i',[1,2,3]);
print array

tel = {'john': 01234, 'frank': 54321}
tel['heinz'] = 878237
print tel

class MyClass:
    """A simple example class"""
    i = 12345
    def f(self):
        return 'hello world'


x = MyClass()
print x.i
print x.f()
