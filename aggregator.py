import glob
import math
import re


if __name__ == "__main__":
    # timelimit = 300
    files = list(glob.glob(f'results/*/*.sol'))
    print(len(files))
    output_path = 'results.csv'

    # clear the file first
    with open(output_path, "w") as f:
        f.write("")

    for filepath in files:
        filepath_data = filepath.split('/')
        instance_name = filepath_data[-1].split(".")[0]
        solver = filepath_data[-2]
        print(instance_name, solver)
        m, n, p = map(int, instance_name.split("_"))
        
        
        with open(filepath, "r") as f:
            try:
                if "scip" in solver:
                    upper_bound = float(f.readline())
                    lower_bound = float(f.readline())
                    time = float(f.readline())
                elif "dp" in solver:
                    first_line = f.readline()
                    upper_bound = float(first_line) if "No" not in first_line else "-"
                    time = float(f.readline())
                    lower_bound = "-"
            except:
                continue
            
            with open(output_path, "a") as output_file:
                line = f"{m},{n},{p},{instance_name},{solver},{upper_bound},{lower_bound},{time}\n"
                print(line)
                output_file.write(line)
            