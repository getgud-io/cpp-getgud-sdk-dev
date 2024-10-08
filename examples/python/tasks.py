from invoke import task
import pathlib
import cffi


@task
def build_sdk(c):
    ffi = cffi.FFI()
    this_dir = pathlib.Path().absolute()
    h_file_name = this_dir / "GetgudSDK_C.h"

    def preprocess_header(file_content):
        lines = file_content.splitlines()
        processed_lines = []
        skip_block = False

        for line in lines:
            if line.startswith("#ifdef __cplusplus"):
                # Start skipping the extern "C" block
                skip_block = True
            elif line.startswith("#endif") and skip_block:
                # End of extern "C" block
                skip_block = False
            elif line.startswith("#") and not skip_block:
                # Skip other preprocessor directives not related to __cplusplus
                continue
            elif not skip_block:
                # Remove GETGUDSDK_API from the line
                processed_lines.append(line.replace("GETGUDSDK_API", "").strip())

        return "\n".join(processed_lines)

    with open(h_file_name) as h_file:
        header_content = h_file.read()
        processed_header_content = preprocess_header(header_content)
        ffi.cdef(processed_header_content)

    # we need to link psl otherwise python getgudsdk doesn't see it for some reason
    ffi.set_source(
        "getgudsdk",
        '#include "GetgudSDK_C.h"',
        libraries=["GetgudSDK", "psl"],
        library_dirs=[this_dir.as_posix()],
        extra_link_args=["-Wl,-rpath,."],
    )

    ffi.compile(verbose=True)