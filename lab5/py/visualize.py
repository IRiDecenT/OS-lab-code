import matplotlib.pyplot as plt
from matplotlib.ticker import FixedFormatter, FixedLocator, MaxNLocator
import numpy as np
import fileinput

# for example
# :RR|10|2.71|4
# A:0|0-1|1-3
# B:2|3-4|5-7|14-17
# C:4|4-5|9-11|17-18
# D:6|7-8|11-13|18-20
# E:8|8-9|13-14
# \n

class package:
    # algo_name = ''
    # processName = []
    # time = []
    # avg_time = 0.0
    # avg_time_with_weight = 0.0
    def __init__(self, name, t1, t2):
        self.algo_name = name
        self.avg_time = t1
        self.avg_time_with_weight = t2
        self.processInfo = []
        self.run_time = []
        self.wait_time = []
        self.processName = []

    def appendProcessInfo(self, proInfo):
        proInfo = proInfo.split(':')
        self.processInfo.append((proInfo[0], int(proInfo[1])))

    def appendTimePeriod(self, period):
        self.run_time.append(period)

    def processWaitInfo(self):
        for i in range(len(self.processInfo)):
            self.processName.append(self.processInfo[i][0])
            self.wait_time.append([])
            preEnd = self.processInfo[i][1]
            for j in range(len(self.run_time[i])):
                self.wait_time[i].append((preEnd, self.run_time[i][j][0] - preEnd))
                preEnd = self.run_time[i][j][0] + self.run_time[i][j][1]


def schedulingPlot(ax, pack):
    ax.invert_yaxis()
    # ax.grid()
# plt.rcParams["font.family"]=["PingFang"] #设置字体
# plt.rcParams["axes.unicode_minus"]=False #正常显示负号

    ax.set_title(pack.algo_name + " scheduling")

    yticks = []
    for i in range(0, len(pack.run_time)):
        ax.broken_barh(pack.run_time[i], ((i+1) * 10, 5), facecolors='royalblue')
        ax.broken_barh(pack.wait_time[i], ((i+1) * 10, 5), facecolors='lightgrey')
        yticks.append((i+1)*10 + 2.5)

    ax.yaxis.set_major_locator(FixedLocator(yticks))
    ax.yaxis.set_major_formatter(FixedFormatter(pack.processName))
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.set_yticklabels(pack.processName)
    ax.set_yticks(yticks)
    ax.set_xlabel("avg turnaround time:" + str(pack.avg_time) + '\navg weighted turnaround time:' + str(pack.avg_time_with_weight))

packageList = []

curPackage = -1

for line in fileinput.input():
    # print(line)
    s = line.strip().split('|')
    if s[0][0] == ':':
        curPackage = curPackage + 1
        packageList.append(package(s[0].removeprefix(':'), float(s[1]), float(s[2])))
    else:
        proInfo = s[0]
        packageList[curPackage].appendProcessInfo(proInfo)
        periods = []
        for i in range(1, len(s)):
            period = s[i].split('-')
            periods.append((int(period[0]), int(period[1]) - int(period[0])))
        packageList[curPackage].appendTimePeriod(periods)

for i in range(len(packageList)):
    packageList[i].processWaitInfo()

for pack in packageList:

    print(pack.algo_name)
    # print(id(pack.processName))
    print(pack.processInfo)
    # print(id(pack.time))
    print(pack.run_time)
    print(pack.wait_time)

fig, ((ax1, ax2, ax3), (ax4, ax5, ax6)) = plt.subplots(nrows=2, ncols=3, figsize = (10,7))

ax6.clear()

axList = [ax1, ax2, ax3, ax4, ax5]

for i in range(0, len(packageList)):
    schedulingPlot(axList[i], packageList[i])

fig.legend(['run', 'wait'])
plt.tight_layout()
plt.show()
