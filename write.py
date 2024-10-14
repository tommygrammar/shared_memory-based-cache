import ctypes

# Load the shared library
lib = ctypes.CDLL('./write.so')

# Define the function signature
lib.write_and_signal.argtypes = [ctypes.c_char_p]

# Example usage
data = '{"name": "baby", "value": "i will win"}'
lib.write_and_signal(data.encode('utf-8'))
