import os, sys, shutil

def copy_target_gcc_dll(msys2_mingw_path:str, build_path:str):
  libgcc_s_seh_path  = os.path.join(msys2_mingw_path, "libgcc_s_seh-1.dll")
  libstdc_path       = os.path.join(msys2_mingw_path, "libstdc++-6.dll")
  libwinpthread_path = os.path.join(msys2_mingw_path, "libwinpthread-1.dll")

  num_of_err = 0
  if os.path.isfile(libgcc_s_seh_path):
    print("libgcc_s_seh_path :", libgcc_s_seh_path)
  else:
    num_of_err = num_of_err + 1
    print("Cannot find libgcc_s_seh-1.dll")
  if os.path.isfile(libstdc_path):
    print("libstdc_path :", libstdc_path)
  else:
    num_of_err = num_of_err + 1
    print("Cannot find libstdc++-6.dll")
  if os.path.isfile(libwinpthread_path):
    print("libwinpthread_path :", libwinpthread_path)
  else:
    num_of_err = num_of_err + 1
    print("Cannot find libwinpthread-1.dll")

  if num_of_err == 0:
    shutil.copy2(libgcc_s_seh_path, build_path)
    shutil.copy2(libstdc_path, build_path)
    shutil.copy2(libwinpthread_path, build_path)
    return True
  else:
    return False


if __name__ == "__main__":
  msys2_mingw_path = sys.argv[1]
  build_path = sys.argv[2]
  result = copy_target_gcc_dll(msys2_mingw_path, build_path)
  if result == True:
    print("SUCCESS: It is successed to copy the target gcc dll.")
  else:
    print("FAILURE: It is failed to copy the target gcc dll.")