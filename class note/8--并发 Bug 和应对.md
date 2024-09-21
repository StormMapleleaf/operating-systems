## Bug 多的根本原因：编程语言的缺陷

> 软件是需求 (规约) 在计算机数字世界的投影。

只管 “翻译” 代码，不管和实际需求 (规约) 是否匹配

 

## 更实在的方法：防御性编程

> 把程序需要满足的条件用 assert 表达出来。//断言assert

------

## 死锁 (Deadlock)

> A deadlock is a state in which each member of a group is waiting for another member, including itself, to take action.

出现线程 “互相等待” 的情况

## 避免死锁

死锁产生的四个必要条件 ([Edward G. Coffman](https://en.wikipedia.org/wiki/Edward_G._Coffman,_Jr.), 1971):

- 互斥：一个资源每次只能被一个进程使用
- 请求与保持：一个进程请求资阻塞时，不释放已获得的资源
- 不剥夺：进程已获得的资源不能强行剥夺
- 循环等待：若干进程之间形成头尾相接的循环等待资源关系

------

## 数据竞争

> 不同的线程同时访问同一段内存，且至少有一个是写。

- 两个内存访问在 “赛跑”，“跑赢” 的操作先执行

## 数据竞争 (cont'd)

Peterson 算法告诉大家：

- 你们写不对无锁的并发程序
- 所以事情反而简单了
- 用互斥锁保护好共享数据
- 消灭一切数据竞争

------

# 应对并发 Bug 的方法

## Lockdep: 运行时的死锁检查

Lockdep 规约 (Specification)

- 为每一个锁确定唯一的 “allocation site”
  - [lock-site.c](https://jyywiki.cn/pages/OS/2022/demos/lock-site.c)
  - assert: 同一个 allocation site 的锁存在全局唯一的上锁顺序

检查方法：printf

- 记录所有观察到的上锁顺序，例如[x,y,z]⇒x→y,x→z,y→z[*x*,*y*,*z*]⇒*x*→*y*,*x*→*z*,*y*→*z*
- 检查是否存在 x⇝y∧y⇝x*x*⇝*y*∧*y*⇝*x*

Lockdep 的实现](https://jyywiki.cn/OS/OS_Lockdep.html)

- Since Linux Kernel 2.6.17, also in [OpenHarmony](https://gitee.com/openharmony)!

## ThreadSanitizer: 运行时的数据竞争检查

为所有事件建立 happens-before 关系图

- Program-order + release-acquire
- 对于发生在不同线程且至少有一个是写的



## 更多的检查：动态程序分析

在事件发生时记录

- Lockdep: lock/unlock
- ThreadSanitizer: 内存访问 + lock/unlock

解析记录检查问题

- Lockdep: x⇝y∧y⇝x*x*⇝*y*∧*y*⇝*x*
- ThreadSanitizer: x⊀y∧y⊀x*x*≺*y*∧*y*≺*x*

付出的代价和权衡

- 程序执行变慢
- 但更容易找到 bug (因此在测试环境中常用)

## 动态分析工具：Sanitizers

没用过 lint/sanitizers？

- AddressSanitizer

   

  (asan);

   

  (paper)

  : 非法内存访问

  - Buffer (heap/stack/global) overflow, use-after-free, use-after-return, double-free, ...
  - Demo: [uaf.c](https://jyywiki.cn/pages/OS/2022/demos/uaf.c); [kasan](https://www.kernel.org/doc/html/latest/dev-tools/kasan.html)

- ThreadSanitizer

   

  (tsan): 数据竞争

  - Demo: [fish.c](https://jyywiki.cn/pages/OS/2022/demos/fish.c), [sum.c](https://jyywiki.cn/pages/OS/2022/demos/sum.c), [peterson-barrier.c](https://jyywiki.cn/pages/OS/2022/demos/peterson-barrier.c); [ktsan](https://github.com/google/ktsan)

- [MemorySanitizer](https://clang.llvm.org/docs/MemorySanitizer.html) (msan): 未初始化的读取

- UBSanitizer

   

  (ubsan): undefined behavior

  - Misaligned pointer, signed integer overflow, ...
  - Kernel 会带着 `-fwrapv` 编译

------

## 我们也可以！Buffer Overrun 检查

Canary (金丝雀) 对一氧化碳非常敏感

- 用生命预警矿井下的瓦斯泄露 (since 1911)

![img](https://jyywiki.cn/pages/OS/img/canary_with_miner.jpg)

计算机系统中的 canary

- “牺牲” 一些内存单元，来预警 memory error 的发生
  - (程序运行时没有动物受到实质的伤害)

## Canary 的例子：保护栈空间 (M2/L2)

//stack首位增加缓冲，标记一个值定期做检查

```c
#define MAGIC 0x55555555
#define BOTTOM (STK_SZ / sizeof(u32) - 1)
struct stack { char data[STK_SZ]; };

void canary_init(struct stack *s) {
  u32 *ptr = (u32 *)s;
  for (int i = 0; i < CANARY_SZ; i++)
    ptr[BOTTOM - i] = ptr[i] = MAGIC;
}

void canary_check(struct stack *s) {
  u32 *ptr = (u32 *)s;
  for (int i = 0; i < CANARY_SZ; i++) {
    panic_on(ptr[BOTTOM - i] != MAGIC, "underflow");
    panic_on(ptr[i] != MAGIC, "overflow");
  }
}
```



# 总结

本次课回答的问题

- **Q**: 如何拯救人类不擅长的并发编程？

Take-away message

- 常见的并发 bug
  - 死锁、数据竞争、原子性/顺序违反
- 不要盲目相信自己：检查、检查、检查
  - 防御性编程：检查
  - 动态分析：打印 + 检查