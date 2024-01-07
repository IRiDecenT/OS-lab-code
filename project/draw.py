import matplotlib.pyplot as plt
import numpy as np

# 生成逻辑地址概率的函数
def probability(i):
    return 1 / np.sqrt(i + 1)

# 生成i的值从0到63的范围
i_values = np.arange(0, 64)

# 计算每个i对应的概率值
probabilities = [probability(i) for i in i_values]

# 绘制图形
plt.plot(i_values, probabilities, marker='o', linestyle='-', color='b')
plt.title('Probability of Accessing Page i')
plt.xlabel('Page Number (i)')
plt.ylabel('Probability')
plt.grid(True)

# 保存图片
plt.savefig('probability_plot.png')

# 显示图形
plt.show()
