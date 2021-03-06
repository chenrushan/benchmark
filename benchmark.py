#!/usr/bin/python2
# coding: utf-8

# ============================================================
#
# input:
#    json benchmark config
# output:
#    stderr 包含文本结果
#    图片包含曲线结果，图片文件名为 json config 的 desc 字段
# USAGE:
#    ./run_benchmark.py config.json
# 
# ------------------------------------------------------------
#
# json config example:
# 
# [
#     {
#         "desc": "performance when load is low",
#         "cmd": "./test2",
#         "times": 50,
#         "args": [
#             "1 100",
#             "2 100",
#             "3 100",
#             "4 100",
#             "8 100",
#             "100 100"
#         ]
#     },
#     {
#         "desc": "performance when load is high",
#         "cmd": "./test2",
#         "times": 3,
#         "args": [
#             "1 50000",
#             "2 50000",
#             "3 50000",
#             "4 50000",
#             "8 50000",
#             "100 50000",
#             "1000 50000",
#             "10000 50000"
#         ]
#     }
# ]
# 
# 这个 config 中包含两组实验
#  1. "desc" 表示某祖实验的 description
#  2. "cmd" 指向需要调用的命令的路径，要求 cmd 的输出必须包含 "[time]: " 字段
#  3. "args" 为命令参数，与输出结果一一对应，如果 cmd 不需要 argument，那随便写
#     一个就行
#  4. "times" 表示表示每个 arg 跑多少次，每个 arg 对应的时间结果为跑这么多次后
#     取平均的结果
# 
# ============================================================

import subprocess
import json
import sys
import os
import matplotlib.pyplot as plt
import numpy as np

def get_time(fn):
    f = open(fn)
    for line in f:
        if line.startswith("[time]: "):
            return float(line[8:])
    f.close()

def plot(desc, xs, ys, outfn):
    fig = plt.figure()
    fig.suptitle(desc)

    x = np.array([i for i in range(len(xs))])
    y = np.array(ys)
    plt.xticks(x, xs)
    plt.plot(x, y, 'bo')
    plt.plot(x, y)
    fig.savefig(outfn)

def main():
    jsn = json.load(open(sys.argv[1]))

    perfs = {}
    result_file = "__benchmark_result__"
    for run in jsn:
        if not os.path.exists(run["cmd"]):
            sys.stderr.write("%s not found\n" % run["cmd"])
            sys.exit(1)

        print json.dumps(run)
        args = run["args"]
        sys.stderr.write("[%s]\n" % run["desc"])
        xs = []
        ys = []
        for a in args:
            cmd = run["cmd"] + " " + a + " 2> " + result_file
            run_times = int(run["times"])
            total_time = 0
            for i in xrange(run_times):
                subprocess.call(cmd, shell=True)
                total_time += get_time(result_file)
            total_time /= run_times;

            xs.append(a)
            ys.append(total_time)
            sys.stderr.write("[%s]: %f\n" % (a, total_time))

        plot(run["desc"], xs, ys, run["desc"] + ".png")
        sys.stderr.write("\n")

    os.remove(result_file)

if __name__ == "__main__":
    main()

