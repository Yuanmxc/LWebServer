#!/bin/zsh

# 统计代码行数
find . \( -name "*.cc" -o -name "*.h" \) -exec wc -l {} +

# 格式化代码
find . \( -name "*.cc" -o -name "*.h" \) -exec clang-format -i {} +
echo "已完成上面的文件的代码格式化"
