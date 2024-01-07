import os
ch = ['A', 'B', 'C', 'D','E','F','G', 'H','I','J','K','L', 'M', 'N', 'O']
# 创建12个文本文件，每个文件代表一个作业
for job_id in range(12):
    file_path = f"job_{job_id}.txt"

    # 每个文件的大小为256 * 10字节，用于存储10个页面的标识信息
    file_size = 256 * 10


    with open(file_path, 'w') as file:
        for i in range(15):
            # 在文件的第i*256字节处写入<作业号，页面号>
            data = f"<{job_id},{i}>"
            file.seek(i * 256)
            file.write(data)

            # 填充剩余部分，可以使用空格或其他字符
            remaining_space = 256 - len(data)
            file.write(f"{ch[i]}" * remaining_space)

# 显示创建的文件
file_list = os.listdir()
print("Created Files:")
for file_name in file_list:
    if file_name.endswith(".txt"):
        print(file_name)
