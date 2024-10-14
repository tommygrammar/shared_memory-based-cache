import ctypes
import time


def cache_read():
    # Load the shared library compiled by C
    lib = (ctypes.CDLL('./sm.so'))

    # Define the return type of the read function
    lib.read_from_shared_memory.restype = ctypes.c_char_p

    lib.read_from_shared_memory()


cache_read()
