"""True immutable symbolic enumeration with qualified value access.

From http://code.activestate.com/recipes/413486/ 

Proformatique changes:
2008-02-20      A __call__ method has been added in EnumClass that just does
                a getattr on its single parameter. This is handy for using an
                EnumClass instance (that is, an enumeration) as a factory that
                returns one of its EnumValue instance, given the string repr of
                the EnumValue. It is used in descriptor structures where a
                callable is needed to act has a mathematical function.
                Also added some PyDoc.
"""

__version__ = "$Revision$ $Date$"

def Enum(*names):
    """
    Returns an enumeration type, which contains enumerated values in its
    attributes having an associated value that goes from 0 to len(names)-1.

    >>> colors = Enum('red', 'black', 'blue')
    >>> colors
    Enum('red', 'black', 'blue')
    >>> colors = Enum('red', 'black', 'blue')
    >>> colors
    Enum('red', 'black', 'blue')
    >>> someColors = set([colors.red, colors('blue')])
    >>> someColors
    set([red, blue])
    >>> otherColors = set([colors[1], colors.red])
    >>> otherColors
    set([red, black])
    >>> fruits = Enum('apple', 'banana', 'orange', 'grape')
    >>> fruits
    Enum('apple', 'banana', 'orange', 'grape')
    >>> colors.red == colors.black
    False
    >>> colors.red == colors.red
    True
    >>> colors.red == fruits.apple
    Traceback (most recent call last):
      File "<stdin>", line 1, in ?
      File "/home/xilun/CA/xivo/Enum.py", line 34, in __cmp__
        assert self.EnumType is other.EnumType, "Only values from the same enum are comparable"
    AssertionError: Only values from the same enum are comparable
    >>> colors.blue in someColors
    True
    """
    ##assert names, "Empty enums are not supported"
    class EnumClass(object):
        __slots__ = names
        def __iter__(self):        return iter(constants)
        def __len__(self):         return len(constants)
        def __getitem__(self, i):  return constants[i]
        def __repr__(self):        return 'Enum' + str(names)
        def __str__(self):         return 'enum ' + str(constants)
        def __call__(self, x):     return getattr(self, x)
    class EnumValue(object):
        __slots__ = ('__value')
        def __init__(self, value): self.__value = value
        Value = property(lambda self: self.__value)
        EnumType = property(lambda self: EnumType)
        def __hash__(self):        return hash(self.__value)
        def __cmp__(self, other):
            assert self.EnumType is other.EnumType, "Only values from the same enum are comparable"
            return cmp(self.__value, other.__value)
        def __invert__(self):      return constants[maximum - self.__value]
        def __nonzero__(self):     return bool(self.__value)
        def __repr__(self):        return str(names[self.__value])
    maximum = len(names) - 1
    constants = [None] * len(names)
    for i, each in enumerate(names):
        val = EnumValue(i)
        setattr(EnumClass, each, val)
        constants[i] = val
    constants = tuple(constants)
    EnumType = EnumClass()
    return EnumType
