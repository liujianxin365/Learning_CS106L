<p align="center">
  <img src="docs/logo.jpeg" alt="Treebook 标志，这是一个虚构的斯坦福社交媒体创业项目" style="width: 300px; height: auto;" />
</p>

# Assignment 5：Treebook 中文说明

截止时间：5 月 15 日（星期五）晚上 11:59

## 作业概述

本作业要完善 `User` 类，使 Treebook 用户资料支持：

- 使用 `operator<<` 输出；
- 正确析构动态好友数组；
- 深拷贝构造与深拷贝赋值；
- 禁止移动构造和移动赋值；
- 使用 `operator+=` 双向添加好友；
- 使用 `operator<` 按用户名排序。

需要修改的文件：

- `user.h`：声明运算符和特殊成员函数；
- `user.cpp`：实现这些函数。

配套知识讲义见：`../../../lecture/2026Spring-11-12-中文讲义.md`。

## 编译与运行

在 `assignment5/` 目录运行：

```sh
g++ -std=c++20 main.cpp user.cpp -o main
./main
```

Windows 可能需要：

```sh
g++ -static-libstdc++ -std=c++20 main.cpp user.cpp -o main
./main.exe
```

建议每完成一部分就重新编译并运行评分器。

## Part 1：输出用户资料

将 `operator<<` 声明为 `User` 的友元非成员函数，并在 `user.cpp` 中实现。

例如，Alice 的好友是 Bob 和 Charlie 时，输出必须精确为：

```text
User(name=Alice, friends=[Bob, Charlie])
```

要求：

- 需要访问并遍历私有 `_friends` 数组；
- 好友之间用逗号和一个空格分隔；
- 函数内部不要输出换行符；
- 返回传入的 `std::ostream&`，以支持链式输出。

## Part 2：正确管理复制与销毁

`_friends` 是类拥有的原始指针。编译器默认复制只会复制指针地址，导致两个对象共享同一数组，随后可能出现相互影响和重复释放。

需要完成：

1. 析构函数 `~User()`；
2. 复制构造函数 `User(const User& user)`；
3. 复制赋值运算符 `User& operator=(const User& user)`；
4. 使用 `= delete` 禁止移动构造；
5. 使用 `= delete` 禁止移动赋值。

复制构造和复制赋值都必须进行深拷贝：

- 复制 `_name`、`_size` 和 `_capacity`；
- 为新对象分配自己的好友数组；
- 逐个复制有效好友；
- 复制赋值还要释放目标对象原来的数组，并处理自赋值。

## Part 3：自定义常用操作

### `operator+=`

签名：

```cpp
User& operator+=(User& rhs);
```

该操作必须双向添加好友：

```cpp
User alice("Alice");
User charlie("Charlie");

alice += charlie;
```

执行后：

```text
User(name=Alice, friends=[Charlie])
User(name=Charlie, friends=[Alice])
```

函数返回 `*this`。

### `operator<`

签名：

```cpp
bool operator<(const User& rhs) const;
```

按用户名的字典序比较。该运算符让 `User` 能用于依赖排序关系的代码，例如 `std::set<User>`。

## 测试重点

- 零个、一个和多个好友的输出格式；
- 复制构造后修改副本，原对象保持不变；
- 复制赋值能覆盖一个已经有好友的对象；
- `a = a` 自赋值不会破坏对象；
- 析构和赋值没有内存泄漏或重复释放；
- 类型可复制，但不可移动；
- `+=` 同时更新左右两边；
- `<` 对 A、B、C 的排序结果一致。

## 提交

通过全部测试后：

1. 填写[课程反馈表](https://forms.gle/tfLJSKnuUbUx9Xdi6)；
2. 在 [Paperless](https://paperless.stanford.edu) 提交。

提交文件：

- `user.h`
- `user.cpp`

截止前可以多次重新提交。
