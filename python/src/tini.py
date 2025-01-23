import os
import ctypes
from ctypes import c_char, c_char_p, c_int, c_bool, POINTER

TINI_ROOT_SECTION = "@$ROOT$@"

_M_libtini_ffi = None
_libtini_force_path = None

def load_libtini():
    global _M_libtini_ffi

    # Check if a forced path is set
    if _libtini_force_path:
        try:
            _M_libtini_ffi = ctypes.CDLL(_libtini_force_path)
            print(f"Loaded libtini from forced path: {_libtini_force_path}")
        except OSError:
            raise RuntimeError(f"Could not load the libtini shared library from path: {_libtini_force_path}")
    else:
        # Load the shared library based on the OS
        try:
            if os.name == "nt":
                _M_libtini_ffi = ctypes.CDLL('libtini.dll')  # For Windows
            else:
                _M_libtini_ffi = ctypes.CDLL('/usr/lib/libtini.so')  # For Linux or other *nix
            print(f"Loaded libtini from default path.")
        except OSError:
            raise RuntimeError("Could not load the libtini shared library. Ensure it's compiled and in the correct path.")
        
load_libtini()

class _FILE(ctypes.Structure):
    pass

_M_libtini_ffi.fopen.argtypes = [ctypes.c_char_p, ctypes.c_char_p]  # filename, mode
_M_libtini_ffi.fopen.restype = ctypes.POINTER(_FILE)

_M_libtini_ffi.fclose.argtypes = [POINTER(_FILE)]
_M_libtini_ffi.fclose.restype = c_int

def _M_libtini_ffi_fopen(file_path, mode="wb"):
    return _M_libtini_ffi.fopen(file_path.encode('utf-8'), mode.encode('utf-8'))

def _M_libtini_ffi_fclose(file_pointer):
    return _M_libtini_ffi.fclose(file_pointer)

# Define the TiniEntry and _M_libtini_ffi_Tini structures based on C code
class _M_libtini_ffi_TiniEntry(ctypes.Structure):
    _fields_ = [
        ("section", c_char * 256),
        ("key", c_char * 256),
        ("value", c_char * 256),
    ]

class _M_libtini_ffi_Tini(ctypes.Structure):
    _fields_ = [
        ("entries", _M_libtini_ffi_TiniEntry * 128),  # Maximum number of entries in _M_libtini_ffi_Tini
        ("count", c_int),  # The count of entries in the _M_libtini_ffi_Tini structure
    ]

# Declare function prototypes and argument/return types
_M_libtini_ffi.tini_init.argtypes = [POINTER(_M_libtini_ffi_Tini)]
_M_libtini_ffi.tini_init.restype = None

_M_libtini_ffi.tini_load.argtypes = [POINTER(_M_libtini_ffi_Tini), ctypes.POINTER(_FILE)]
_M_libtini_ffi.tini_load.restype = c_bool

_M_libtini_ffi.tini_get.argtypes = [POINTER(_M_libtini_ffi_Tini), c_char_p, c_char_p]
_M_libtini_ffi.tini_get.restype = c_char_p

_M_libtini_ffi.tini_set.argtypes = [POINTER(_M_libtini_ffi_Tini), c_char_p, c_char_p, c_char_p]
_M_libtini_ffi.tini_set.restype = c_bool

_M_libtini_ffi.tini_dump.argtypes = [POINTER(_M_libtini_ffi_Tini), ctypes.POINTER(_FILE)]
_M_libtini_ffi.tini_dump.restype = c_bool

_M_libtini_ffi.tini_has.argtypes = [POINTER(_M_libtini_ffi_Tini), c_char_p, c_char_p]
_M_libtini_ffi.tini_has.restype = c_bool

_M_libtini_ffi.tini_remove.argtypes = [POINTER(_M_libtini_ffi_Tini), c_char_p, c_char_p]
_M_libtini_ffi.tini_remove.restype = c_bool


# Python wrapper class for _M_libtini_ffi_Tini structure
class Tini:
    def __init__(self):
        self.ini = _M_libtini_ffi_Tini()
        _M_libtini_ffi.tini_init(ctypes.byref(self.ini))  # Initialize the INI structure

    def load(self, file_path):
        """Load an INI file into the structure."""
        file = _M_libtini_ffi_fopen(file_path, "rb")
        if not _M_libtini_ffi.tini_load(ctypes.byref(self.ini), file):
            _M_libtini_ffi.fclose(file)
            raise RuntimeError(f"Failed to load INI file: {file_path}")
        _M_libtini_ffi.fclose(file)

    def get(self, section, key):
        """Retrieve a value for a given section and key."""
        value = _M_libtini_ffi.tini_get(ctypes.byref(self.ini), section.encode('utf-8'), key.encode('utf-8'))
        return value.decode('utf-8') if value else None

    def set(self, section, key, value):
        """Set a key-value pair in a section."""
        if not _M_libtini_ffi.tini_set(ctypes.byref(self.ini), section.encode('utf-8'), key.encode('utf-8'), value.encode('utf-8')):
            raise RuntimeError(f"Failed to set key-value pair: [{section}] {key}={value}")

    def has(self, section, key):
        """Check if a section and key exist."""
        return _M_libtini_ffi.tini_has(ctypes.byref(self.ini), section.encode('utf-8'), key.encode('utf-8'))

    def remove(self, section, key):
        """Remove a key-value pair."""
        if not _M_libtini_ffi.tini_remove(ctypes.byref(self.ini), section.encode('utf-8'), key.encode('utf-8')):
            raise RuntimeError(f"Failed to remove key: [{section}] {key}")

    def dump(self, file_path):
        """Dump the INI structure to a file."""
        file = _M_libtini_ffi_fopen(file_path, "wb")
        if not _M_libtini_ffi.tini_dump(ctypes.byref(self.ini), file):
            _M_libtini_ffi.fclose(file)
            raise RuntimeError(f"Failed to dump INI to file: {file_path}")
        _M_libtini_ffi_fclose(file)
