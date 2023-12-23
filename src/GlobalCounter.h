#pragma once
#include <iostream>

class GlobalCounter {
private:
    static GlobalCounter* instance;
    int value;

    // 私有构造函数确保不会从外部创建多个实例
    GlobalCounter() : value(0) {}

public:
    // 禁止拷贝构造和赋值
    GlobalCounter(const GlobalCounter&) = delete;
    void operator=(const GlobalCounter&) = delete;

    // 获取这个类的单实例对象
    static GlobalCounter& GetInstance() {
        if (instance == nullptr) {
            instance = new GlobalCounter();
        }
        return *instance;
    }

    // 获取当前计数值
    int GetCurrent() const {
        return value;
    }

    // 获取下一个计数值（获取后当前值加一）
    int GetNext() {
        return ++value;
    }
};

// 获取全局用到了第几个临时数字变量，转成字符串
std::string GetNext();
std::string GetCurrent();