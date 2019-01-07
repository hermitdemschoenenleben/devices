import asyncio as aio
from time import sleep
from termcolor import colored
from traceback import print_exc
from multiprocessing import Process, Pipe

ERROR = 'ERROR'

class CallableProperties:
    def __getattr__(self, name):
        class Callable:
            def __init__(this, name):
                this.name = name

            def __call__(this, *args, **kwargs):
                return self.call(this.name, *args)
        return Callable(name)


class AsyncDeviceException(Exception):
    pass


class AsyncDevice(CallableProperties):
    """
    This class runs a class that has synchronously blocking functions
    in a background process and provides asynchronous access to it, enabling
    non-blocking method calls.

    Usage:

        class BlockingDevice:
            def __init__(self, some_parameter):
                pass

            def slow_method(self, parameter):
                sleep(10)
                return parameter

        async def do_something_with_the_device():
            some_parameter = 123

            def constructor():
                return BlockingDevice(some_parameter)

            unblocking_device = AsyncDevice(constructor)

            data = unblocking_device.call('slow_method', 'parameter')

            # do something else in the meantime

            print(await data)

            unblocking_device.close()
    """

    def __init__(self, device_constructor, loop=None):
        self.device_constructor = device_constructor
        self.lock = aio.Lock(loop=loop)
        self.pipe, child_pipe = Pipe()
        self.process = Process(target=_asyncify, args=(child_pipe, device_constructor))
        self.process.start()
        self.pipe.recv()

    async def _wait_for_result(self):
        while not self.pipe.poll():
            await aio.sleep(0.1)
        return self.pipe.recv()

    async def call(self, fct, *args):
        with await self.lock:
            self.pipe.send((fct, args))
            result = await self._wait_for_result()

            if result == ERROR:
                raise AsyncDeviceException()

            return result

    def close(self):
        self.process.join()


class SeperateProcess(CallableProperties):
    """
    This class runs a class in a seperate process, thus it is similar to
    AsyncDevice, with the exception that function calls are executed in a
    blocking way.
    This may seem useless, but is necessary e.g. for talking to two different
    devices if doing so in a single thread would cause interference
    (e.g.: Meerstetter TEC using the supplied software package)
    """
    process = None

    def __init__(self, device_constructor):
        self.pipe, child_pipe = Pipe()
        self.process = Process(target=_asyncify, args=(child_pipe, device_constructor))
        self.process.start()
        self.pipe.recv()

    def call(self, fct, *args):
        self.pipe.send((fct, args))
        result = self.pipe.recv()

        if result == ERROR:
            raise AsyncDeviceException()
        return result


    def close(self):
        self.process.terminate()
        self.process.join()



SPECIAL_PYTHON_METHODS = [
    '__abs__', '__add__', '__and__', '__call__', '__cmp__', '__coerce__',
    '__contains__', '__delitem__', '__delslice__', '__div__', '__divmod__',
    '__eq__', '__float__', '__floordiv__', '__ge__', '__getitem__',
    '__getslice__', '__gt__', '__hash__', '__hex__', '__iadd__', '__iand__',
    '__idiv__', '__idivmod__', '__ifloordiv__', '__ilshift__', '__imod__',
    '__imul__', '__int__', '__invert__', '__ior__', '__ipow__', '__irshift__',
    '__isub__', '__iter__', '__itruediv__', '__ixor__', '__le__', '__len__',
    '__long__', '__lshift__', '__lt__', '__mod__', '__mul__', '__ne__',
    '__neg__', '__oct__', '__or__', '__pos__', '__pow__', '__radd__',
    '__rand__', '__rdiv__', '__rdivmod__', '__reduce__', '__reduce_ex__',
    '__repr__', '__reversed__', '__rfloorfiv__', '__rlshift__', '__rmod__',
    '__rmul__', '__ror__', '__rpow__', '__rrshift__', '__rshift__', '__rsub__',
    '__rtruediv__', '__rxor__', '__setitem__', '__setslice__', '__sub__',
    '__truediv__', '__xor__', 'next',
]

for name in SPECIAL_PYTHON_METHODS:
    setattr(
        AsyncDevice, name,
        lambda self, *args, _attribute_name=name, **kwargs: self.call(_attribute_name, *args, **kwargs)
    )
    setattr(
        SeperateProcess, name,
        lambda self, *args, _attribute_name=name, **kwargs: self.call(_attribute_name, *args, **kwargs)
    )


def execute_on_object(obj, fct_name, args, kwargs={}):
    for part in fct_name.split('.'):
        obj = getattr(obj, part)

    return obj(*args, **kwargs)


def _asyncify(pipe, device_constructor):
    try:
        device = device_constructor()
        pipe.send(True)
        while True:
            try:
                fct_name, args = pipe.recv()

                result = execute_on_object(device, fct_name, args)

                pipe.send(result)
            except KeyboardInterrupt:
                raise
            except Exception as e:
                print(colored(
                    'Exception at async device %s' % \
                        str(device or device_constructor),
                    'red'
                ))
                print_exc()
                pipe.send(ERROR)

    except KeyboardInterrupt:
        print(colored('KeyboardInterrupt!', 'red'))