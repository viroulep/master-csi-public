def cut(from_file, to_file):
    skip_line = False
    with open(from_file, "r") as from_:
      with open(to_file, "w") as to_:
        for line in from_.readlines():
          if "CUT" in line and "BEGIN" in line:
            skip_line = True

          if not skip_line:
            to_.write(line)

          if "CUT" in line and "END" in line:
            skip_line = False 
