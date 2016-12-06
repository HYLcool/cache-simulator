# cache simulator

- Cache模拟器 by 黄一伦，叶根，邢新宇

# 用法:
```shell
$ make
$ ./sim <options> [arguments] <filename>
```
## 选项options:

- -a: 用于测试test a中的第一个测试项目，无[arguments]项
- -b: 用于测试test a中的第二个测试项目，无[arguments]项
- -c: 用于测试test a中的第三个测试项目，有[arguments]，包含两项：<back&through>为设置write back还是write through，前者为0,后者为1；<allocate>为设置no write allocate或write allocate，前者为0,后者为1
- -u: 用于打印使用帮助信息

## 文件名<filename>:

- 为`.trace`类文件

# 补充说明

- -a选项中，默认相连度associativity为1,cache size变化范围为32kB到32MB，Block size变化范围为8到1024bytes
- -b选项中，默认Block size为128bytes，cache size变化范围为32kB到32MB，associativity变化范围为1到32
- -c选项中，默认cache size为32kB，相连度为8,set num为32,然后默认no write allocate必须和write through搭配
