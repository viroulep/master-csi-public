import os
from pathlib import Path
import sys
import tarfile
import cut

src_dir = sys.argv[1]
bin_dir = sys.argv[2]
output = sys.argv[3]
courses = sys.argv[4:]

temp = "./temp_file"
subdir = ""
prefix = "tp-obfu"

with tarfile.open(output, "w:gz") as package:
    package.add(os.path.join(src_dir, "docker", "Dockerfile"), os.path.join(prefix, "docker", "Dockerfile"))
    package.add(os.path.join(src_dir, "docker", "install-essentials.sh"), os.path.join(prefix, "docker", "install-essentials.sh"))
    package.add(os.path.join(src_dir, "docker", "build-install-llvm.sh"), os.path.join(prefix, "docker", "build-install-llvm.sh"))

    cut.cut(os.path.join(src_dir, "CMakeLists.txt"), temp)
    package.add(temp, os.path.join(prefix, subdir, "CMakeLists.txt"))
    package.add(os.path.join(src_dir, "README.md"), os.path.join(prefix, "README.md"))
    package.add(os.path.join(src_dir, "Vagrantfile"), os.path.join(prefix, "Vagrantfile"))
    package.add(os.path.join(src_dir, "activate_llvm.sh"), os.path.join(prefix, "activate_llvm.sh"))

    # Add the package target
    package.add(os.path.join(src_dir, "package", "CMakeLists.txt"), os.path.join(prefix, subdir, "package", "CMakeLists.txt"))
    package.add(os.path.join(src_dir, "package", "package.py"), os.path.join(prefix, subdir, "package", "package.py"))
    package.add(os.path.join(src_dir, "package", "cut.py"), os.path.join(prefix, subdir, "package", "cut.py"))

    # Add the cmake helpers
    package.add(os.path.join(src_dir, "cmake", "FindZ3.cmake"), os.path.join(prefix, subdir, "cmake", "FindZ3.cmake"))

    # Add the courses
    for course in courses:
        # FIXME: add the course's instructions
        # slides = os.path.join(bin_dir, course, course + ".pdf")

        # package.add(slides, os.path.join(subdir, course, course + ".pdf"))

        cut.cut(os.path.join(src_dir, course, "CMakeLists.txt"), temp)
        package.add(temp, os.path.join(prefix, subdir, course, "CMakeLists.txt"))

        root = os.path.join(src_dir, course, "ex")
        for pattern in ["*.c", "*.cpp", "*.h", "CMakeLists.txt", "*.py", "*.test", "*.in"]:
            for src in Path(root).rglob(pattern):
                src_name = src.relative_to(root)
                cut.cut(src, temp)
                package.add(temp, os.path.join(prefix, subdir, course, "ex", src_name))
