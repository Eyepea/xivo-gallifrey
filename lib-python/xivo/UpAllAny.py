# any() and all() are available in Python 2.5, we target Python 2.4


try:
    temp = all
    all = temp
except NameError:
    def all(iterable):
        for element in iterable:
            if not element:
                return False
        return True
    __builtins__['all'] = all


try:
    temp = any  
    any = temp
except NameError:
    def any(iterable):
        for element in iterable:
            if element:
                return True
        return False
    __builtins__['any'] = any


__all__ = ['all', 'any']
