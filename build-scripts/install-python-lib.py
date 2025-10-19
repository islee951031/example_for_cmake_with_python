import sys, os, shutil, subprocess

# This function is for finding python**.dll and, returning its absolute path.
def find_python_dll(root_dir):
    found_file = ""
    counter = 0
    for dir_path, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if (filename.find("python") == 0) and \
                (filename.find(".dll") > len("python*")) and \
                (filename[len("python"):filename.find(".dll")-1].isdecimal()):
                found_file = root_dir + "/" + filename
                counter = counter + 1
    if (counter == 0):
        return "0"
    elif (counter > 1):
        return "2"
    return found_file

# This function is for copying python**.dll from source path to distrbution path.
def install_python_dll(python_path:str, build_path:str):
    print("On installing python**.dll,")
    src_dllfile = find_python_dll(python_path)
    dst_dllfile = find_python_dll(build_path)

    # To find source python dll file.
    if   src_dllfile == "0":
        print("Error: It cannot find a src python**.dll.")
        return -1
    elif src_dllfile == "2":
        print("Error: src python**.dll cannot be specified. It's not singular.")
        return -1
    else:
        print(" >", os.path.basename(src_dllfile), "is found.")
        print(" > PATH:", src_dllfile)

    # To copy the source python dll file to distribution path
    if   dst_dllfile == "0": # <<-- happy case
        shutil.copy(src_dllfile, build_path)
        print(" >", os.path.basename(src_dllfile), "is copied.")
    elif dst_dllfile == "2":
        print("Error: There are already mutilple python dlls")
        return -1
    else:
        if os.path.basename(src_dllfile) == os.path.basename(dst_dllfile): # <<-- happy case
            print(" >", os.path.basename(dst_dllfile), "already exists at dst. Copying is skipped.")
        else:
            print("Error: It is different between versions of src and dst python dll.")
            print(" > src ver. :", os.path.basename(src_dllfile))
            print(" > dst ver. :", os.path.basename(dst_dllfile))
            return -1
    return 0

# This function is for copying directory, which is portion installed python.
def install_target_dir(python_path:str, build_path:str, target_name:str):
    print("On installing " + target_name + ",")
    src_path = python_path + "/" + target_name
    dst_path = build_path + "/python/" + target_name

    # To check if target python directory exists at source path.
    if not os.path.isdir(src_path):
        print("Error: Cannot find python/" + target_name + " directory at the source.")
        return -1
    
    # To check if target python directory exists at distribution path.
    if not os.path.isdir(build_path + "/python"):
        os.mkdir(build_path + "/python")
    if os.path.isdir(dst_path):
        print(" > There is already " + target_name + " directory at distribution path. Copying is skipped.")
        return 0
    os.mkdir(dst_path)

    # To exclude unnecessary directories in target python directory.
    names = os.listdir(src_path)
    if "site-packages" in names:
        names.remove("site-packages")
    if "__pycache__" in names:
        names.remove("__pycache__")

    # To copy target python directory from source path to distribution path.
    size = len(names)
    for i in range(size):
        print(f" > On copying[{i+1}/{size}]", names[i] + ",", flush=True)
        target = src_path + "/" + names[i]
        if (os.path.isdir(target)):
            shutil.copytree(target, dst_path + "/" + names[i])
        else:
            shutil.copy2(target, dst_path)
    return 0

# This function is for installing python modules.
def install_site_packages(build_path:str, requirements:str):
    print("On installing site-packages,")
    dst_path = build_path + "/python/Lib/site-packages"

    # To check "requirements.txt".
    if not os.path.isfile(requirements):
        print("Error: There is no", requirements)
        return -1

    # To check if "python/Lib/site-packages" directory exists.
    if not os.path.isdir(build_path + "/python"):
        os.mkdir(build_path + "/python")
    if not os.path.isdir(build_path + "/python/Lib"):
        os.mkdir(build_path + "/python/Lib")
    if not os.path.isdir(build_path + "/python/Lib/site-packages"):
        os.mkdir(build_path + "/python/Lib/site-packages")
    
    # To install required packages except already installed.
    with open(requirements, 'r') as f:
        required_pkgs = [line.strip().split('==')[0].split('<')[0].split('>')[0] for line in f if line.strip() and not line.startswith('#')]
    with open(requirements, 'r') as f:
        required_pkgs_with_ver = [line.strip() for line in f if line.strip() and not line.startswith('#')]
    num_of_required_pkgs = len(required_pkgs)
    installed_pkg_files = os.listdir(dst_path) 
    num_of_installed_pkg_files = len(installed_pkg_files)
    for i in range(num_of_required_pkgs):
        counter = 0
        for j in range(num_of_installed_pkg_files):
            if required_pkgs[i] in installed_pkg_files[j]:
                counter = counter + 1
        if counter == 0:
            print(f" > On installing[{i+1}/{num_of_required_pkgs}]", required_pkgs_with_ver[i] + ",", flush=True)
            command = [
                sys.executable, 
                "-m", 
                "pip", 
                "install", 
                required_pkgs_with_ver[i], 
                "--target", dst_path
            ]
            subprocess.check_call(command)
        else:
            print(" >", required_pkgs[i], "already exist.")
    return 0


if __name__ == "__main__":
    python_path = sys.argv[1]
    build_path  = sys.argv[2]

    err_counter = 0
    if install_python_dll(python_path, build_path):           err_counter = err_counter + 1
    if install_target_dir(python_path, build_path, "DLLs"):   err_counter = err_counter + 1
    if install_target_dir(python_path, build_path, "Lib"):    err_counter = err_counter + 1
    if install_site_packages(build_path, "requirements.txt"): err_counter = err_counter + 1
    if err_counter:
        print("Python Installation is failed.")
    else:
        print("Python Installation is successed.")