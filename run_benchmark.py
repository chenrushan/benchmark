#!/usr/bin/python2

import subprocess
import json
import sys

def get_time(fn):
    f = open(fn)
    for line in f:
        if line.startswith("[time]: "):
            return float(line[8:])
    f.close()

def main():
    jsn = json.load(open("runs.json"))

    perfs = {}
    result_file = "__benchmark_result__"
    for run in jsn:
        print json.dumps(run)
        args = run["args"]
        sys.stderr.write("[%s]\n" % run["desc"])
        for a in args:
            cmd = run["cmd"] + " " + a + " 2> " + result_file
            run_times = int(run["times"])
            total_time = 0
            for i in xrange(run_times):
                subprocess.call(cmd, shell=True)
                total_time += get_time(result_file)
            total_time /= run_times;

            sys.stderr.write("[%s]: %f\n" % (a, total_time))
        sys.stderr.write("\n")

if __name__ == "__main__":
    main()

