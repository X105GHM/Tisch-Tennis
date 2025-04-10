import subprocess
from SCons.Script import DefaultEnvironment # type: ignore

env = DefaultEnvironment()

def after_build_action(target, source, env):
    print("\n============== Automatically uploading FS image ==============")
    ret = subprocess.call("pio run -t uploadfs", shell=True)
    if ret:
        print("Error during uploading FS image.")
    else:
        print("File system image uploaded successfully.")

env.AddPostAction("buildprog", after_build_action)
