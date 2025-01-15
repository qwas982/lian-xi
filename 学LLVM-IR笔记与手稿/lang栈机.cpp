/* 用大模型翻译了一个简单栈机来用, 
但是在调用的时候会产生 `段错误` , 
不晓得咋回事, 其它的功能待测试.
*/

/* clang++ lang.cpp -o lang -static */
/* 使用了 utfcpp 三方库支持Unicode的输入输出, https://github.com/nemtrif/utfcpp , */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <iterator>
#include <memory>

#include "utf8.h"

using namespace std;

// 类型别名
using 指针 = size_t;
using 标号表 = map<string, 指针>;
using 过程表 = map<string, pair<指针, 指针>>;

// 栈框结构
struct 栈框 {
    指针 栈偏移;
    指针 指令指针;
};

// 栈类
class 栈 {
private:
    vector<int64_t> 数据;

public:
    void 推(int64_t 值) {
        数据.push_back(值);
    }

    int64_t 弹() {
        if (数据.empty()) {
            throw runtime_error("试图从空栈中弹出元素");
        }
        int64_t 值 = 数据.back();
        数据.pop_back();
        return 值;
    }

    int64_t 窥() const {
        if (数据.empty()) {
            throw runtime_error("试图查看空栈的栈顶");
        }
        return 数据.back();
    }

    int64_t& 窥可变() {
        if (数据.empty()) {
            throw runtime_error("试图查看空栈的栈顶");
        }
        return 数据.back();
    }

    int64_t 获取(size_t 索引) const {
        if (索引 >= 数据.size()) {
            throw runtime_error("访问了不存在的栈索引");
        }
        return 数据[索引];
    }

    int64_t& 获取可变(size_t 索引) {
        if (索引 >= 数据.size()) {
            throw runtime_error("可变访问了不存在的栈索引");
        }
        return 数据[索引];
    }

    size_t 大小() const {
        return 数据.size();
    }

    void 印() const {
        for (const auto& 值 : 数据) {
            cout << 值 << " ";
        }
        cout << endl;
    }
};

// 指令枚举
enum class 指令 {
    推,
    弹,
    加,
    减,
    乘,
    除,
    递增,
    递减,
    跳,
    等跳,
    不等跳,
    大于跳,
    小于跳,
    大等跳,
    小等跳,
    获取,
    设置,
    获取参数,
    设置参数,
    空操作,
    印,
    印字符,
    印栈,
    叫,
    返
};

// 虚机类
class 虚机 {
private:
    vector<pair<指令, int64_t>> 程序;
    栈 栈;
    指针 指令指针;
    vector<栈框> 叫栈;

public:
    虚机(const vector<pair<指令, int64_t>>& 程序) : 程序(程序), 指令指针(0) {}

    void 跑() {
        while (指令指针 < 程序.size()) {
            auto [指令, 操作数] = 程序[指令指针++];

            switch (指令) {
                case 指令::空操作:
                    break;

                case 指令::推:
                    栈.推(操作数);
                    break;

                case 指令::弹:
                    栈.弹();
                    break;

                case 指令::加: {
                    int64_t 值1 = 栈.弹();
                    int64_t 值2 = 栈.弹();
                    栈.推(值1 + 值2);
                    break;
                }

                case 指令::减: {
                    int64_t 值1 = 栈.弹();
                    int64_t 值2 = 栈.弹();
                    栈.推(值2 - 值1);
                    break;
                }

                case 指令::乘: {
                    int64_t 值1 = 栈.弹();
                    int64_t 值2 = 栈.弹();
                    栈.推(值1 * 值2);
                    break;
                }

                case 指令::除: {
                    int64_t 值1 = 栈.弹();
                    int64_t 值2 = 栈.弹();
                    栈.推(值2 / 值1);
                    break;
                }

                case 指令::递增:
                    栈.窥可变() += 1;
                    break;

                case 指令::递减:
                    栈.窥可变() -= 1;
                    break;

                case 指令::跳:
                    指令指针 = 操作数;
                    break;

                case 指令::等跳:
                    if (栈.窥() == 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::不等跳:
                    if (栈.窥() != 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::大于跳:
                    if (栈.窥() > 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::小于跳:
                    if (栈.窥() < 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::大等跳:
                    if (栈.窥() >= 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::小等跳:
                    if (栈.窥() <= 0) {
                        栈.弹();
                        指令指针 = 操作数;
                    }
                    break;

                case 指令::获取: {
                    size_t 偏移 = 叫栈.empty() ? 0 : 叫栈.back().栈偏移;
                    if (操作数 + 偏移 >= 栈.大小()) {
                        throw runtime_error("获取指令: 索引越界");
                    }
                    栈.推(栈.获取(操作数 + 偏移));
                    break;
                }

                case 指令::设置: {
                    size_t 偏移 = 叫栈.empty() ? 0 : 叫栈.back().栈偏移;
                    if (操作数 + 偏移 >= 栈.大小()) {
                        throw runtime_error("设置指令: 索引越界");
                    }
                    栈.获取可变(操作数 + 偏移) = 栈.窥();
                    break;
                }

                case 指令::获取参数: {
                    if (叫栈.empty()) {
                        throw runtime_error("获取参数指令: 没有调用过程，无法获取参数");
                    }
                    size_t 偏移 = 叫栈.back().栈偏移 - 1 - 操作数;
                    cout << "调试: 获取参数, 偏移 = " << 偏移 << ", 栈大小 = " << 栈.大小() << endl;
                    if (偏移 >= 栈.大小()) {
                        throw runtime_error("获取参数指令: 索引越界");
                    }
                    栈.推(栈.获取(偏移));
                    break;
                }

                case 指令::设置参数: {
                    if (叫栈.empty()) {
                        throw runtime_error("设置参数指令: 没有调用过程，无法设置参数");
                    }
                    size_t 偏移 = 叫栈.back().栈偏移 - 1 - 操作数;
                    cout << "调试: 设置参数, 偏移 = " << 偏移 << ", 栈大小 = " << 栈.大小() << endl;
                    if (偏移 >= 栈.大小()) {
                        throw runtime_error("设置参数指令: 索引越界");
                    }
                    栈.获取可变(偏移) = 栈.窥();
                    break;
                }

                case 指令::印:
                    cout << 栈.窥() << '\n';
                    break;

                case 指令::印字符: {
                    int64_t codepoint = 栈.窥();
                    if (codepoint >= 0 && codepoint <= 0x10FFFF) {
                        std::string utf8_char;
                        utf8::append(static_cast<uint32_t>(codepoint), std::back_inserter(utf8_char));
                        std::cout << utf8_char << '\n';
                    } else {
                        throw std::runtime_error("无效的Unicode码点: " + std::to_string(codepoint));
                    }
                    break;
                }
                case 指令::印栈:
                    栈.印();
                    break;

                case 指令::叫:
                    叫栈.push_back({栈.大小(), 指令指针});
                    指令指针 = 操作数;
                    break;

                case 指令::返:
                    if (叫栈.empty()) {
                        throw runtime_error("返指令: 调用栈为空");
                    }
                    指令指针 = 叫栈.back().指令指针;
                    叫栈.pop_back();
                    break;
            }
        }
    }
};

// 辅助函数
pair<指令, int64_t> 解析指令(const vector<string>& 牌, const 标号表& 标号, const 过程表& 过程) {
    if (牌.empty()) {
        throw runtime_error("空指令");
    }

    string 操作 = 牌[0];

    if (操作 == "推") {
        return {指令::推, stoll(牌[1])};
    } else if (操作 == "弹") {
        return {指令::弹, 0};
    } else if (操作 == "加") {
        return {指令::加, 0};
    } else if (操作 == "减") {
        return {指令::减, 0};
    } else if (操作 == "乘") {
        return {指令::乘, 0};
    } else if (操作 == "除") {
        return {指令::除, 0};
    } else if (操作 == "递增") {
        return {指令::递增, 0};
    } else if (操作 == "递减") {
        return {指令::递减, 0};
    } else if (操作 == "跳") {
        return {指令::跳, 标号.at(牌[1])};
    } else if (操作 == "等跳") {
        return {指令::等跳, 标号.at(牌[1])};
    } else if (操作 == "不等跳") {
        return {指令::不等跳, 标号.at(牌[1])};
    } else if (操作 == "大于跳") {
        return {指令::大于跳, 标号.at(牌[1])};
    } else if (操作 == "小于跳") {
        return {指令::小于跳, 标号.at(牌[1])};
    } else if (操作 == "大等跳") {
        return {指令::大等跳, 标号.at(牌[1])};
    } else if (操作 == "小等跳") {
        return {指令::小等跳, 标号.at(牌[1])};
    } else if (操作 == "获取") {
        return {指令::获取, stoull(牌[1])};
    } else if (操作 == "设置") {
        return {指令::设置, stoull(牌[1])};
    } else if (操作 == "获取参数") {
        return {指令::获取参数, stoull(牌[1])};
    } else if (操作 == "设置参数") {
        return {指令::设置参数, stoull(牌[1])};
    } else if (操作 == "印") {
        return {指令::印, 0};
    } else if (操作 == "印字符") {
        return {指令::印字符, 0};
    } else if (操作 == "印栈") {
        return {指令::印栈, 0};
    } else if (操作 == "叫") {
        return {指令::叫, 过程.at(牌[1]).first + 1};
    } else if (操作 == "返") {
        return {指令::返, 0};
    } else if (操作 == "标号" || 操作 == "终" || 操作 == "过程") {
        return {指令::空操作, 0}; // 处理标号、终和过程指令
    
    } else if (操作 == "空操作") {
        return {指令::空操作, 0};

    } else {
        throw runtime_error("无效指令: " + 操作);
    }
}

标号表 查找标号(const vector<vector<string>>& 行) {
    标号表 标号;
    for (size_t i = 0; i < 行.size(); ++i) {
        if (行[i].size() == 2 && 行[i][0] == "标号") {
            标号[行[i][1]] = i;
        }
    }
    return 标号;
}

过程表 查找过程(const vector<vector<string>>& 行) {
    过程表 过程;
    size_t 指令指针 = 0;

    while (指令指针 < 行.size()) {
        if (行[指令指针].size() == 2 && 行[指令指针][0] == "过程") {
            string 过程名 = 行[指令指针][1];
            size_t 开始指令指针 = 指令指针;
            while (行[指令指针] != vector<string>{"终"}) {
                ++指令指针;
            }
            过程[过程名] = {开始指令指针, 指令指针 + 1};
        } else {
            ++指令指针;
        }
    }

    return 过程;
}

vector<pair<指令, int64_t>> 解析程序(const vector<vector<string>>& 行, const 标号表& 标号, const 过程表& 过程) {
    vector<pair<指令, int64_t>> 指令集;
    for (const auto& 行 : 行) {
        指令集.push_back(解析指令(行, 标号, 过程));
    }
    return 指令集;
}

int main(int argc, char* argv[]) {    
    if (argc < 2) {
        cerr << "用法: " << argv[0] << " <文件名>" << endl;
        return 1;
    }

    unique_ptr<ifstream> 文件 = make_unique<ifstream>(argv[1]);
    if (!*文件) {
        cerr << "无法打开文件: " << argv[1] << endl;
        return 1;
    }

    stringstream 缓冲区;
    缓冲区 << 文件->rdbuf();
    文件.reset();

    vector<vector<string>> 行;
    string 行内容;
    while (getline(缓冲区, 行内容)) {
        if (行内容.empty() || 行内容.substr(0, 2) == "--") {
            continue;
        }
        istringstream iss(行内容);
        vector<string> 牌{istream_iterator<string>{iss}, istream_iterator<string>{}};
        行.push_back(牌);
    }

    标号表 标号 = 查找标号(行);
    过程表 过程 = 查找过程(行);
    vector<pair<指令, int64_t>> 指令集 = 解析程序(行, 标号, 过程);

    虚机 虚机实例(指令集);
    虚机实例.跑();

    return 0;
}


/*

程序:

推 5
推 3
乘
推 2
加
推 4
减
印


推 65
印字符
推 97
印字符

推 19968
印字符
推 29968
印字符
推 19969
印字符
推 23334
印字符
推 31000
印字符

印
印栈

; --------------------------------------

输出:

# ./lang.exe program.txt
13
A
a
一
甐
丁
嬦
礘
31000
13 65 97 19968 29968 19969 23334 31000

*/