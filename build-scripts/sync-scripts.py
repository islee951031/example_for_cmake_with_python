import sys, os, shutil

src_path = sys.argv[1]
dest_path = sys.argv[2]

if not __name__ == "__main__" or not os.path.isdir(src_path):
    sys.exit() 

if not os.path.isdir(dest_path):
    os.mkdir(dest_path)

names = os.listdir(src_path)
try:
    names.remove("CMakeLists.txt")
except ValueError:
    pass

for name in names:
    origin_file = os.path.join(src_path, name)
    dest_file = os.path.join(dest_path, name)
    if not os.path.isfile(dest_file) or os.path.getmtime(origin_file) > os.path.getmtime(dest_file):
        print("Copy", origin_file)
        shutil.copy(origin_file, dest_file)

print("Done")