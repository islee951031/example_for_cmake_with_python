import sys, os, shutil, functools

def find_python_dll(root_dir):
    found_file = ""
    for dir_path, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if (filename.find("python") == 0) and \
                (filename.find(".dll") > len("python*")) and \
                (filename[len("python"):filename.find(".dll")-1].isdecimal()):
                found_file = os.path.join(root_dir, filename)
    return found_file

def install_logger(name):
    def real_logger(func):
        @functools.wraps(func)
        def warrper(*args, **kwargs):
            print("Install", name, flush=True)
            func(*args, **kwargs)
            print("Done", flush=True)
        return warrper
    return real_logger

@install_logger("DLLs")
def install_DLLS(python_path:str, build_path:str):
    src = os.path.join(python_path, "DLLs")
    dist = os.path.join(build_path, "python/DLLs")
    if (not os.path.isdir(dist)) and os.path.isdir(src):
        shutil.copytree(src, dist)

@install_logger("Lib")
def install_Lib(python_path:str, build_path:str):
    src = os.path.join(python_path, "Lib")
    dist = os.path.join(build_path, "python/Lib")
    if os.path.isdir(dist):
        return
    
    if not os.path.isdir(src):
        print("Error: cannot find the paths", flush=True)
        return
        
    os.mkdir(dist)
    os.mkdir(os.path.join(dist, "site-packages"))
    names = os.listdir(src)
    try:
        names.remove("site-packages")
        names.remove("__pycache__")
    except ValueError:
        pass
    
    size = len(names)
    for i in range(size):
        target = os.path.join(src, names[i])
        if os.path.isfile(target):
            shutil.copy(target, dist)
            
        elif os.path.isdir(target):
            shutil.copytree(target, os.path.join(dist, names[i]))
        
        print(f"Copy[{i+1}/{size}]", names[i], flush=True)

if __name__ == "__main__":
    python_path = sys.argv[1]
    build_path = sys.argv[2]

    install_DLLS(python_path, build_path)
    install_Lib(python_path, build_path)
    dllfile = find_python_dll(python_path)

    if os.path.isfile(dllfile):
        print("\"python**.dll\" is found.")
        print(dllfile)
        shutil.copy(dllfile, build_path)
    else:
        print("\"python**.dll\" is not found.")