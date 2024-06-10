from invoke import task
import pathlib
import cffi


@task
def build_sdk(c):
    ffi = cffi.FFI()
    this_dir = pathlib.Path().absolute()
    h_file_name = this_dir / "GetGudSdk_C.h"
    with open(h_file_name) as h_file:
        ffi.cdef(h_file.read())

    ffi.set_source(
        "getgudsdk",
        '#include "GetGudSdk_C.h"',
        # libraries=[":libGetGudSdk.so", "stdc++", "crypto", "ssl", ":libz.so"], # Linux
        libraries=[":libGetGudSdk.so"], # Linux
        # libraries=["GetGudSdk"], # Windows
        library_dirs=[this_dir.as_posix()],
        extra_link_args=["-Wl,-rpath,."],
    )

    ffi.compile(verbose=True)