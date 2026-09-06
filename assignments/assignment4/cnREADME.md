<p align="center">
  <img src="docs/header.png" alt="黑色背景上的标题图片，以代码字体显示 [~]$ ispell" />
</p>


# 作业 4：Ispell

截止时间：5 月 8 日（星期五）晚上 11:59

## 概述

我们已经学习了 STL（标准模板库）的核心组成部分——容器、迭代器、函数对象和算法——以及驱动这一切的关键要素——模板。现在，让我们把这些知识综合起来！
在本次作业中，你将为 [Ispell](https://en.wikipedia.org/wiki/Ispell) 编写核心逻辑。Ispell 是一个经典 Unix 风格的拼写检查器，可以完成简单的拼写检查。为此，你需要编写使用 `<algorithm>` 头文件和新版 C++ ranges（范围）库的代码。

你编写的所有代码都将放在 `spellcheck.cpp` 中。完成后，你会得到一个如下所示的拼写检查器：

<p align="center">
  <img src="docs/spellcheck.png" alt="在终端中运行拼写检查程序的示例" />
</p>

> [!IMPORTANT]
> 这份作业说明看起来可能很长，但本次作业实际需要编写的代码并不多！我们加入了许多额外细节，希望让实现过程更直接。如果有任何内容让你感到困惑，请告诉我们（欢迎通过 Ed、课堂或答疑时间联系）！我们还会在星期二（05/05）的课堂上讲解 `tokenize`，帮助大家开始本次作业！

如需下载本作业的初始代码，请查看课程作业仓库中的 [**入门说明（Getting Started）**](../assignment-setup/README.md)。

## 运行代码

运行代码前，需要先进行编译。打开终端（如果你使用 VS Code，请按 <kbd>Ctrl+\`</kbd>，或选择顶部菜单中的 **Terminal > New Terminal**）。然后确认当前位于 `assignment4/` 目录，并运行：

```sh
g++ -std=c++20 main.cpp spellcheck.cpp -o main
```

假设代码编译成功且没有任何编译器错误，接下来可以运行：

```sh
./main
```

这条命令会实际运行 `main.cpp` 中的 `main` 函数。

在按照下方说明完成作业的过程中，我们建议你不时进行编译、测试，并使用自动评分器检查，从而确认自己的实现方向是否正确！

> [!NOTE]
>
> ### Windows 用户注意事项
>
> 在 Windows 上，为了正常看到输出，你可能需要使用以下命令编译代码：
>
> ```sh
> g++ -static-libstdc++ -std=c++20 main.cpp spellcheck.cpp -o main
> ```
>
> 此外，生成的可执行文件可能名为 `main.exe`。在这种情况下，请使用以下命令运行代码：
>
> ```sh
> ./main.exe
> ```

## 构建 Ispell

经典 Unix 程序 Ispell 的工作方式如下。首先，将一份包含常用英语单词的词典加载到内存中。如果某个单词无法在词典中找到，它就被视为拼写错误。程序使用 [Damerau–Levenshtein 编辑距离](https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance)算法寻找每个错词的修改建议。这个算法大致告诉你：要把一个单词变成另一个单词，需要进行多少次编辑——增加、删除或替换一个字母，或者交换两个相邻字母。如果错词与某个词典单词之间的 Damerau–Levenshtein 编辑距离恰好为 `1`，就把该词典单词加入建议列表。这里的想法是：人们拼错单词时，通常只差一个很小的改动，例如 `"mispelled"` 与 `"misspelled"`。

> [!NOTE]
> 上面描述的是标准 Damerau–Levenshtein 编辑距离；不过，本仓库 `utils.cpp` 中提供的 `levenshtein` 实现实际只处理增加、删除和替换，不会把相邻字符交换计为一次编辑。作业中应直接使用这个已提供的函数，无须自行重写。

本次作业已经提供了构建这个拼写检查器所需的基础设施，其中包括 Damerau–Levenshtein 函数的实现。你的任务是实现检查单词拼写的核心算法。具体来说，你需要编写一个把输入字符串拆分为一组词元的算法 `tokenize`，以及另一个根据分词后的输入字符串和词典真正识别错词的算法 `spellcheck`。为了增加一点挑战，同时对应上一周的课程内容，这里有一项限制：你的代码中不能使用任何 `for` 或 `while` 循环。你必须完全使用 STL 来实现这些任务：`tokenize` 使用传统 STL 算法，`spellcheck` 使用全新的 ranges 库。在此过程中，你会接触如何利用算法和 lambda 函数操作现代 C++ 数据结构。

这听起来可能很多，但别担心！下面的说明会详细引导你完成每个算法。

### `tokenize`

```cpp
struct Token { std::string content; size_t src_offset; };
using Corpus = std::set<Token>;
Corpus tokenize(std::string& input);
```

`tokenize` 函数接收一个输入字符串，并把它拆分为一组 `Token` 对象。请查看我们在 `spellcheck.h` 中定义的 `Token` 结构体。`Token` 表示较大文件中的一段内容：从概念上说，它就是一篇文本中出现的一个单词；在代码中，它是文件内索引 `src_offset` 处出现的一个 `std::string`。我们的目标是把输入文件拆分为一组 `Token`，并将这组词元称为 `Corpus`（语料集）；`Corpus` 只是 `std::set<Token>` 的类型别名。

本题有一个关键条件：每个词元的两侧是空白字符和／或输入文件的边界。例如，短字符串 `"history will absolve me"` 包含四个词元：

* `{ content: "history", src_offset: 0 }`
* `{ content: "will", src_offset: 8 }`
* `{ content: "absolve", src_offset: 13 }`
* `{ content: "me", src_offset: 21 }`

为了实现 `tokenize`，我们将使用 `std::transform` 等传统 STL 方法，并且不使用任何 `for` 或 `while` 循环。整体思路如下：

1. 找出所有指向空白字符的迭代器。
2. 在每对相邻空白字符之间生成词元。
3. 删除空词元。

你可以按照下面的步骤完成实现：

1. **第一步：找出所有指向空白字符的迭代器**
    如果能找出字符串中所有指向空白字符的迭代器，那么大体上就可以把任意两个空白字符之间的内容视为一个词元。我们似乎需要多次调用 `find_if`，收集所有指向空白字符的迭代器。幸运的是，我们已经提供了一个正好能完成这项工作的函数：`find_all`。

    > 📄 [**`find_all`**](./utils.cpp)
    > ```cpp
    > template <typename Iterator, typename UnaryPred>
    > std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);
    > ```
    >
    > 返回一个向量，其中包含 `[begin, end)` 范围内所有满足一元谓词 `pred` 的元素所对应的迭代器。**这个向量还会包含边界迭代器 `begin` 和 `end`**。换句话说，如果 `it` 是返回向量中的一个迭代器，那么 `pred(*it)`、`it == begin` 和 `it == end` 三者中至少有一个成立。向量中的迭代器一定按顺序排列。

    我们可以对 `source` 字符串调用 `find_all`，并传入一个用于判断字符是否为空白字符的一元谓词，从而得到所有指向空白字符的迭代器。C++ 已经内置了这样一个函数，它叫作 `isspace`。

    > 📄 [**`std::isspace`**](https://en.cppreference.com/w/c/string/byte)
    >
    > 注意：`std::isspace` 存在重载，直接把它传给函数模板时，编译器可能无法推断谓词类型[^1]。在本仓库中可以直接传入 `::isspace`；更稳妥的写法是传入一个参数类型明确的 lambda，并在其中调用 `std::isspace`。
    >
    > ```cpp
    > int std::isspace(int ch);
    > ```

[^1]: 使用 `std::isspace` 时，实际上存在不止一个同名函数版本。

      ```cpp
      int isspace(int ch);                          // 在头文件 <cctype> 和 <ctype.h> 中定义

      template <class CharT>
      bool isspace(CharT ch, const locale& loc);    // 在头文件 <locale> 中定义
      ```

      严格来说，第一个版本既[定义在 `namespace std` 中](https://en.cppreference.com/w/cpp/header/cctype)，也作为[从 C 继承而来的全局函数](https://en.cppreference.com/w/c/string/byte)存在，不属于任何特定命名空间。第二个版本属于 `std`，定义在 `<locale>` 头文件中。因此，`std::isspace` 表示一个重载集合，编译器可能无法据此推断模板参数 `UnaryPred` 的具体类型。

      写成 `::isspace` 会明确告诉 C++ 在*全局命名空间*中查找 C 版本的 `isspace`，从而消除重载歧义。

2. **第二步：在相邻空白字符之间生成词元**
    现在我们已经获得了所有指向空白字符的迭代器，可以把任意两个相邻空白字符迭代器之间的字符范围看作一个词元。为了理解原因，请看下面的示意图：

    ```
    "history will absolve me"
     ▲      ▲    ▲       ▲  ▲
     ├──────┼────┼───────┼──┤
     │  t1  │ t2 │   t3  │t4│
    ```

    箭头表示 `find_all` 返回的迭代器。可以看到，每两个箭头之间的字符就是一个词元。不必担心某个迭代器是否确实指向空白字符，也不需要亲自“修剪”词元；`Token` 提供了一个接收一对迭代器的构造函数，会自动删除词元两端的空白和标点符号。

    > 📄 [**`Token`**](./spellcheck.h)
    > ```cpp
    > template <typename It>
    > Token(std::string& source, It begin, It end);
    > ```
    >
    > 给定字符串 `source`，以及一对用于标明 `source` 中某个词元范围的迭代器 `begin` 和 `end`，构造一个 `Token`。构造函数会自动删除词元两端多余的空白字符和标点符号。

    我们需要为每一对相邻迭代器调用这个构造函数。为此，将使用 [`std::transform` 的重载版本 (3)](https://en.cppreference.com/w/cpp/algorithm/transform)。

    > 📄 [**`std::transform`**](https://en.cppreference.com/w/cpp/algorithm/transform)
    > ```cpp
    > template <class InputIt1, class InputIt2, class OutputIt, class BinaryOp>
    > OutputIt std::transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
    >                         OutputIt d_first, BinaryOp binary_op);
    > ```
    >
    > 给定两个长度相同的范围，第一个范围从 `first1` 开始，第二个范围从 `first2` 开始，而第一个范围的尾后迭代器为 `last1`。此函数把二元函数 `binary_op` 依次应用到两个范围中相应位置的元素上，例如 `binary_op(first1, first2)`、`binary_op(first1 + 1, first2 + 1)` 等，并将结果存入从 `d_first` 开始、长度相同的输出范围。

    对于 `binary_op`，可以提供一个 lambda 函数。它接收两个 `std::string::iterator`，即 `it1` 和 `it2`（也可以像课堂上讲过的那样，把 lambda 参数类型写成 `auto`），然后使用前面提到的 `Token { source, it1, it2 }` 构造函数创建 `Token`。请注意，我们必须把 `source` 传给这个构造函数，因此你创建的 lambda 需要捕获 `source`！**必须按引用捕获 `source`，否则代码将无法正常运行！**

    > **‼️⚠️📢🚨 警告 🚨📢⚠️‼️**
    > 由于过去有不少同学在这里遇到问题，我们再重复一次：为了让 `Token` 构造函数正常工作，**必须在 lambda 函数中按引用捕获 `source`**。如果忘记了 lambda 的捕获语法，请回顾相关课程幻灯片。

    对于输出范围 `d_first`，首先创建一个 `std::set<Token>`，用来保存找到的词元。假设这个集合叫作 `tokens`，那么可以创建 [`std::inserter(tokens, tokens.end())`](https://en.cppreference.com/w/cpp/iterator/inserter)，将产生的词元保存进去。

    > 📄 [**`std::inserter`**](https://en.cppreference.com/w/cpp/iterator/inserter)
    > ```cpp
    > template <class Container>
    > std::insert_iterator<Container> inserter(Container& c, typename Container::iterator i);
    > ```
    >
    > 这是一个输出迭代器，会把写入它的值插入容器 `c` 中的 `i` 位置；`i` 的类型是该容器的迭代器类型。返回值是一个 [`std::insert_iterator<Container>`](https://en.cppreference.com/w/cpp/iterator/insert_iterator)，可以作为输出范围传给其他 STL 算法，例如 `std::transform`。
    >
    > 请注意，`std::inserter` 返回的迭代器与之前见过的一些迭代器类型略有不同，但它仍然是输出迭代器！其他算法可以解引用并向它写入值，而它会在内部把这些元素插入底层容器。

    对于输入范围 `first1`、`last1` 和 `first2`，需要巧妙地选择迭代器。假设把 `find_all` 返回的迭代器向量叫作 `boundaries`，我们必须让二元操作第一次接收到 `boundaries[0]` 与 `boundaries[1]`，第二次接收到 `boundaries[1]` 与 `boundaries[2]`，依此类推。应如何设置这两个输入范围，才能让二元操作作用于一对对相邻的空白字符迭代器？**提示：从 `first1` 和 `first2` 开始的两个输入范围完全可以重叠！**

3. **第三步：删除空词元**
    到目前为止生成的某些词元会是空的，例如字符串中存在多个连续空白字符时。我们需要删除这些空词元。幸运的是，[`std::erase_if` 函数](https://en.cppreference.com/w/cpp/container/set/erase_if)可以删除 `std::set` 中所有满足某项条件的元素。

    > 📄 [**`std::erase_if`**](https://en.cppreference.com/w/cpp/container/set/erase_if)
    > ```cpp
    > template <class Key, class Compare, class Alloc, class Pred>
    > std::set<Key, Compare, Alloc>::size_type erase_if (std::set<Key, Compare, Alloc>& c, Pred pred);
    > ```

    对于 `pred`，可以传入一个检查词元是否为空的 lambda 函数。例如，可以检查 `token.content.empty()`。

    最后，返回 `tokens`；它包含输入字符串中的所有有效词元。

完成这一步后，拼写检查器应该就会开始报告词元数量。编译代码后，可以运行：

```sh
./main "hello wrld"
```

这会检查字符串 `"hello wrld"` 的拼写，并应当报告：

```
Loading dictionary... loaded 464811 unique words.
Tokenizing input... got 2 tokens.
```

你的 `tokenize` 函数已经飞快地对包含约五十万个单词的英语词典以及输入字符串 `"hello wrld"` 完成了分词。不过，它还没有真正进行拼写检查：`"wrld"` 会被报告为拼写正确。要解决这个问题，还需要实现 `spellcheck` 函数。

### `spellcheck`

```cpp
struct Misspelling { Token token; std::set<std::string> suggestions; };
using Dictionary = std::unordered_set<std::string>;
std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary);
```

`spellcheck` 函数接收分词后的 `Corpus`（即 `tokenize` 的输出）和一个 `Dictionary`；`Dictionary` 只是 `std::unordered_set<std::string>` 的类型别名，表示所有拼写正确的英语单词。该函数返回一组 `Misspelling` 结构体。每个 `Misspelling` 都会标明一个拼写错误的 `token`，以及一组可用来替换该 `token`、从而得到正确拼写的建议单词。

为了找出这些 `Misspelling`，我们将执行下面的算法。这一次，你会练习使用位于 `std::ranges::views` 命名空间中的新版 ranges/views（范围／视图）库：

1. 跳过已经拼写正确的单词。
2. 对其余单词，利用 Damerau–Levenshtein 编辑距离在词典中寻找只差一次编辑的单词。
3. 丢弃没有任何建议的错词。

下面是实现该算法的分步指南：

1. **第一步：跳过已经拼写正确的单词**
    如果一个单词出现在 `dictionary` 中，就说明它拼写正确。例如，`dictionary.contains("world")` 返回 `true`，而 `dictionary.contains("wrld")` 返回 `false`。第一步是跳过 `source` 中已经拼写正确的单词。为此，可以使用 `std::ranges::views::filter` 视图。

    > 📄 [**`std::ranges::views::filter`**](https://en.cppreference.com/w/cpp/ranges/filter_view)
    > ```cpp
    > template <ranges::viewable_range R, class Pred>
    > constexpr ranges::view auto filter(R&& r, Pred&& pred);
    >
    > template <class Pred>
    > constexpr /* range adaptor closure */ filter(Pred&& pred);
    > ```
    >
    > `filter(r, pred)` 会生成一个视图，对底层范围 `r` 进行适配；遍历所得视图时，只包含满足 `pred` 的元素。`filter(pred)` 则会创建一个*范围适配器*，可以像下方示例一样，通过 `operator|` 与某个范围串联起来。

    构建 `std::ranges::views` 管道时，我们会把多个范围操作逐步串联起来。每一步都对前一步进行*适配*，并通过 lambda 函数惰性地应用某项操作，例如过滤元素或转换元素。观察上面的 `std::ranges::views::filter` 定义，可以发现有两种写法：

    ```cpp
    auto view = std::ranges::views::filter(source, /* 一个 lambda 谓词函数 */);

    /* ……等价于…… */

    auto view = source | std::ranges::views::filter(/* 一个 lambda 谓词函数 */);
    ```

    第二种写法可以说更简洁，因为它允许我们用 `operator|` 在同一条管道中串联多个步骤，而不必为每一步创建单独的变量。`std::ranges::views::filter` 写起来比较繁琐，所以人们经常创建一个*命名空间别名*来缩短它：

    ```cpp
    namespace rv = std::ranges::views;
    auto view = source | rv::filter(/* 一个 lambda 谓词函数 */);
    ```

    自动评分器会接受任意一种写法：既可以通过命名空间别名使用 `rv::filter`，也可以直接使用 `std::ranges::views::filter`。

    这一步的任务，是把 `/* 一个 lambda 谓词函数 */` 替换成一个 lambda：它接收一个 `Token`，并在该词元内容**拼写错误**时返回 `true`，因为我们只关心拼错的单词。为此，需要在 lambda 中使用 `dictionary`，因此必须捕获它。你应该按引用捕获，还是按值捕获呢？

2. **第二步：利用 Damerau–Levenshtein 编辑距离在词典中寻找只差一次编辑的单词**
    此时，`view` 表示一个视图，其中包含 `source` 中所有*拼写错误*的词元。接下来，我们使用 `std::ranges::views::transform` 视图，把每个错词转换为相应的 `Misspelling` 对象，同时生成修改建议。

    > 📄 [**`std::ranges::views::transform`**](https://en.cppreference.com/w/cpp/ranges/transform_view)
    > ```cpp
    > template <ranges::viewable_range R, class F>
    > constexpr ranges::view auto transform(R&& r, F&& func);
    >
    > template <class F>
    > constexpr /*range adaptor closure*/ transform(F&& func);
    > ```
    >
    > `transform(r, func)` 会生成一个视图，对底层范围 `r` 进行适配；遍历所得视图时，`r` 中的每个元素 `e` 都会通过 `func(e)` 转换为一个新元素。`transform(func)` 则会创建一个*范围适配器*，可以通过 `operator|` 与某个范围串联起来。

    如果把这一步和上一步结合起来，代码大致如下：

    ```cpp
    namespace rv = std::ranges::views;
    auto view = source
        | rv::filter(/* 一个 lambda 谓词函数 */)
        | rv::transform(/* 一个接收 Token 并返回 Misspelling 的 lambda 函数 */);
    ```
    <sup>注意：这只是一种实现方式。如果你选择使用 `transform(r, func)` 重载，或者不创建 `namespace rv` 别名，你的解法看起来可能不同。</sup>

    那么，`/* 一个接收 Token 并返回 Misspelling 的 lambda 函数 */` 应该替换成什么？我们需要一个 lambda 函数：它接收一个 `Token` 对象，并生成一个 `Misspelling` 对象，其中包含该 `token` 的所有候选正确拼写。为了找出这些建议，需要遍历 `dictionary`，找到所有与 `token.content` 的 Damerau–Levenshtein 编辑距离恰好为 `1` 的单词。你可以使用已经提供的 `levenshtein` 函数计算这个编辑距离。

    > 📄 [**`levenshtein`**](./spellcheck.h)
    > ```cpp
    > size_t levenshtein(const std::string& a, const std::string& b);
    > ```
    >
    > 返回 `a` 与 `b` 之间的 Damerau–Levenshtein 编辑距离。粗略来说，它表示把 `a` 变为 `b` 所需的修改次数。实际上，这个函数实现的是经过高度优化的 Damerau–Levenshtein 编辑距离算法；如果计算过程中确定距离会大于 `1`，它便会提前退出。

    请注意，必须为*每一个*拼写错误的单词遍历 `dictionary` 并寻找建议。**这意味着，你需要在 `/* 一个接收 Token 并返回 Misspelling 的 lambda 函数 */` 内部再嵌套一次 `std::ranges::views::filter` 调用。**为了构造保存建议的 `std::set`，需要使用 [`std::set` 构造函数的重载版本 (4)](https://en.cppreference.com/w/cpp/container/set/set)，把嵌套的建议单词视图实体化为一个集合，从而触发惰性求值。

    > 📄 [**`std::set`**](https://en.cppreference.com/w/cpp/container/set/set)
    > ```cpp
    > template <class InputIt>
    > set(InputIt first, InputIt last, const Compare& comp = Compare(), const Allocator& alloc = Allocator());
    > ```
    >
    > 根据迭代器 `first` 与 `last` 之间的元素范围创建一个 `set`。

    例如，可以使用下面的代码把一个视图实体化为集合：

    ```cpp
    auto view = dictionary | rv::filter(/* 一个 lambda 谓词函数 */);
    std::set<std::string> suggestions(view.begin(), view.end());
    ```

    最后，可以使用统一初始化，根据 `token` 和建议集合 `suggestions` 创建一个 `Misspelling` 对象：

    ```cpp
    Misspelling { token, suggestions }
    ```

    这应该作为上面代码中 `/* 一个接收 Token 并返回 Misspelling 的 lambda 函数 */` 的返回值。

3. **第三步：丢弃没有任何建议的错词**
    此时，`view` 包含所有错词及其修改建议；换句话说，它是一个由 `Misspelling` 对象组成的集合视图。不过，其中一些 `Misspelling` 不会包含任何建议。例如，乱码单词 `"adskadnfknfs"` 显然拼写错误，但英语词典中没有任何单词与它只相差一次编辑。返回结果之前，我们希望从视图中删除这些没有建议的 `Misspelling`。

    我们可以再次对 `view` 应用 `std::ranges::views::filter`。到这里，你应该已经掌握完成这一步所需的全部信息！过滤掉空的 `Misspelling` 后，需要把 `view` 实体化为 `std::set<Misspelling>` 并返回。可以采用与第二步中构造 `suggestions` 类似的方法！

    > ⚠️ [**`std::ranges::to`**](https://en.cppreference.com/w/cpp/ranges/to)
    > 你可能还记得，课堂上曾使用 `std::ranges::to`，把一个 `char` 视图实体化为 `std::string`：
    > ```cpp
    > auto v = s | rv::filter(isalpha)
    >            | /* 其他步骤 */
    >            | std::ranges::to<std::string>();
    > ```
    > 你可能会想在这里使用 `std::ranges::to<std::set<Misspelling>>()` 完成类似的操作。这是个好想法！不过，`std::ranges::to` 直到 C++23 才加入标准。根据你使用的编译器版本，这段代码可能能够编译，也可能不能。为确保代码安全地通过编译，并能在我们的自动评分环境中正常编译，请使用接收迭代器的 `std::set<Misspelling>` 构造函数。**一般来说，本次作业请只使用不晚于 C++20 的语言特性。**

如果此前的所有内容都实现正确，你现在应该已经拥有一个功能完整的拼写检查器！重新编译后，尝试运行：

```sh
./main "This string is mispelled"
```

你应该会看到类似下面的输出：

<p align="center">
  <img src="docs/mispelled.png" alt="在终端中运行拼写检查程序的示例" />
</p>

你也可以检查某个给定示例的拼写：

```sh
./main --stdin < "examples/(marquez).txt"
```

> [!NOTE]
> **PowerShell 用户：**
> 如果你使用 Microsoft PowerShell（Windows），检查示例文件时的命令语法会略有不同：
> ```sh
> Get-Content "examples/(marquez).txt" | ./main --stdin
> ```

> [!NOTE]
> 我们鼓励你多试用这个拼写检查程序，看看能发现哪些有趣的行为。下面是可以尝试的全部选项：
>
> ```
> ./main [--dict dict_path] [--stdin] [--unstyled] [--profile] text
>
> --dict dict_path  设置词典的位置；默认值为 words.txt
> --stdin           从标准输入读取；可以用它通过管道传入文件内容
> --unstyled        不为输出添加任何颜色
> --profile         分析程序性能，输出分词和拼写检查所用的时间
> text              不使用标准输入时，要进行拼写检查的文本
> ```
>
> 如果想接受额外挑战，可以尝试带上 `--profile` 选项运行代码。我们的拼写检查算法虽然采用了简单的暴力方法，会搜索包含约五十万个单词的整个词典，但运行速度仍然相当快！欢迎探索如何在保证输出正确的同时进一步提升算法性能。这完全是可选内容，但我们很期待看到你的成果。


## 🚀 提交说明

要完整测试拼写检查器，请重新编译并运行自动评分器：

```sh
./main
```

如果通过所有测试，就可以提交了！提交本次作业时：

1. 请通过[此链接](https://forms.gle/AMq7kvVKprKmBafKA)填写反馈表。
2. 请在 [Paperless](https://paperless.stanford.edu) 上提交作业！

你需要提交以下文件：

* `spellcheck.cpp`

在截止时间之前，你可以根据需要多次重新提交。
