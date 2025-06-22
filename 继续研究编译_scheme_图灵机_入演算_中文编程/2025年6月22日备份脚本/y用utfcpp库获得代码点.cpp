/* 若要不依赖, 可以加静态编译参数 `-static` . */

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <bitset>
#include "utf8.h"  // 包含 UTF-CPP 头文件

// 处理 UTF-8 编码和解码
class UTF8处理器 {
public:
    // 把 UTF-8 字符串翻译为 Unicode 代码点
    static std::vector<uint32_t> 翻译为代码点(const std::string& utf8字符串) {
        std::vector<uint32_t> 代码点列表;
        auto 迭代器 = utf8::iterator(utf8字符串.begin(), utf8字符串.begin(), utf8字符串.end());
        auto 结束 = utf8::iterator(utf8字符串.end(), utf8字符串.begin(), utf8字符串.end());

        for (; 迭代器 != 结束; ++迭代器) {
            代码点列表.push_back(*迭代器);
        }

        return 代码点列表;
    }

    // 把 Unicode 代码点翻译为 UTF-8 编码
    static std::string 翻译为UTF8(const std::vector<uint32_t>& 代码点列表) {
        std::string utf8字符串;
        for (uint32_t 代码点 : 代码点列表) {
            utf8::append(代码点, std::back_inserter(utf8字符串));
        }
        return utf8字符串;
    }
};

// 处理用户输入
class 输入处理器 {
public:
    // 获取用户输入的 UTF-8 字符串
    static std::string 获取UTF8字符串() {
        std::string utf8字符串;
        std::cout << "请输入 UTF-8 字符串: ";
        std::getline(std::cin, utf8字符串);
        return utf8字符串;
    }

    // 获取用户输入的 Unicode 代码点
    static std::vector<uint32_t> 获取代码点() {
        std::vector<uint32_t> 代码点列表;
        std::string 输入;
        std::cout << "请输入 Unicode 代码点 (支持格式: 0x4F60 十六进制, 12345 十进制, 0b1010 二进制): ";
        std::getline(std::cin, 输入);

        // Lambda 表达式: 解析代码点字符串
        auto 解析代码点 = [](const std::string& 代码点字符串) -> uint32_t {
            if (代码点字符串.empty()) {
                throw std::invalid_argument("输入为空");
            }

            // 判断输入格式
            if (代码点字符串.size() >= 2 && 代码点字符串[0] == '0') {
                if (代码点字符串[1] == 'x' || 代码点字符串[1] == 'X') {
                    // 十六进制格式: 0x4F60
                    return std::stoul(代码点字符串.substr(2), nullptr, 16);
                } else if (代码点字符串[1] == 'b' || 代码点字符串[1] == 'B') {
                    // 二进制格式: 0b1010
                    return std::stoul(代码点字符串.substr(2), nullptr, 2);
                }
            }

            // 默认十进制格式: 12345
            return std::stoul(代码点字符串, nullptr, 10);
        };

        size_t 位置 = 0;
        while (位置 < 输入.length()) {
            size_t 下一个位置 = 输入.find(' ', 位置);
            if (下一个位置 == std::string::npos) {
                下一个位置 = 输入.length();
            }
            std::string 代码点字符串 = 输入.substr(位置, 下一个位置 - 位置);

            try {
                uint32_t 代码点 = 解析代码点(代码点字符串);
                代码点列表.push_back(代码点);
            } catch (const std::exception& 错误) {
                std::cerr << "错误: 无法解析代码点 '" << 代码点字符串 << "': " << 错误.what() << "\n";
            }

            位置 = 下一个位置 + 1;
        }

        return 代码点列表;
    }
};

// 处理输出
class 输出处理器 {
public:
    // 输出 Unicode 代码点
    static void 输出代码点(const std::vector<uint32_t>& 代码点列表) {
        std::cout << "Unicode 代码点:\n";
        for (uint32_t 代码点 : 代码点列表) {
            std::cout << "--------------------" << "\n";
            std::cout << "U+" << std::hex << std::uppercase << 代码点 << "\n";
            std::cout << std::dec << 代码点 << "\n";
            std::cout << std::bitset<8>(代码点) << "\n";
            std::cout << "--------------------" << "\n";
        }
    }

    // 输出 UTF-8 编码
    static void 输出UTF8(const std::string& utf8字符串) {
        std::cout << "--------------------" << "\n";
        std::cout << "UTF-8 编码: " << utf8字符串 << "\n";
        std::cout << "--------------------" << "\n";
    }
};

// 主应用程序
class 应用程序 {
public:
    void 运行() {
        // 示例 1: 输入 UTF-8 字符串，输出 Unicode 代码点
        std::string utf8字符串 = 输入处理器::获取UTF8字符串();
        std::vector<uint32_t> 代码点列表 = UTF8处理器::翻译为代码点(utf8字符串);
        输出处理器::输出代码点(代码点列表);

        // 示例 2: 输入 Unicode 代码点，输出 UTF-8 编码
        std::vector<uint32_t> 输入的代码点 = 输入处理器::获取代码点();
        std::string 输出的UTF8字符串 = UTF8处理器::翻译为UTF8(输入的代码点);
        输出处理器::输出UTF8(输出的UTF8字符串);
    }
};

void 读求值印环(){
    std::string 输入;
    std::cout << "输入'退出' 'q' '出口'关闭! " << std::endl;
    for (;;){
        应用程序 应用;
        应用.运行();
        std::cout << ">>> " << std::flush;
        std::getline(std::cin, 输入);
        if (输入.compare("退出") == 0 ||
            输入.compare("q") == 0 ||
            输入.compare("出口") == 0){
            break;
        }
    }
    std::cout << "=> 环终." << std::endl;
}

int main() {
    读求值印环();
    return 0;
}