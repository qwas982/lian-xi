#include "MAL.h"
#include "Types.h"

#include <regex>

typedef std::regex              Regex;

static const Regex intRegex("^[-+]?\\d+$");
static const Regex closeRegex("[\\)\\]}]");

static const Regex whitespaceRegex("[\\s,]+|;.*");
static const Regex tokenRegexes[] = {
    Regex("~@"),
    Regex("[\\[\\]{}()'`~^@]"),
    Regex("\"(?:\\\\.|[^\\\\\"])*\""),
    Regex("[^\\s\\[\\]{}('\"`,;)]+"),
};

class 词牌化_大写
{
public:
    词牌化_大写(const 字符串_大写& input);

    字符串_大写 peek() const {
        ASSERT(!eof(), "词牌化在窥视中读取经过的EOF \n");
        return m_token;
    }

    字符串_大写 next() {
        ASSERT(!eof(), "词牌化在下一个中读取经过的EOF\n");
        字符串_大写 ret = peek();
        nextToken();
        return ret;
    }

    bool eof() const {
        return m_iter == m_end;
    }

private:
    void skipWhitespace();
    void nextToken();

    bool matchRegex(const Regex& regex);

    typedef 字符串_大写::const_iterator StringIter;

    字符串_大写      m_token;
    StringIter  m_iter;
    StringIter  m_end;
};

词牌化_大写::词牌化_大写(const 字符串_大写& input)
:   m_iter(input.begin())
,   m_end(input.end())
{
    nextToken();
}

bool 词牌化_大写::matchRegex(const Regex& regex)
{
    if (eof()) {
        return false;
    }

    std::smatch match;
    auto flags = std::regex_constants::match_continuous;
    if (!std::regex_search(m_iter, m_end, match, regex, flags)) {
        return false;
    }

    ASSERT(match.size() == 1, "应该只有一个子匹配项，而不是 %lu\n",
                              match.size());
    ASSERT(match.position(0) == 0, "需要匹配第一个字符\n");
    ASSERT(match.length(0) > 0, "需要匹配非空字符串\n");

    // Don't advance  m_iter now, do it after we've consumed the token in
    // next().  If we do it now, we hit eof() when there's still one token left.
    m_token = match.str(0);

    return true;
}

void 词牌化_大写::nextToken()
{
    m_iter += m_token.size();

    skipWhitespace();
    if (eof()) {
        return;
    }

    for (auto &it : tokenRegexes) {
        if (matchRegex(it)) {
            return;
        }
    }

    字符串_大写 mismatch(m_iter, m_end);
    if (mismatch[0] == '"') {
        MAL_CHECK(false, "期望 '\"', 得到EOF");
    }
    else {
        MAL_CHECK(false, "不期望 '%s'", mismatch.c_str());
    }
}

void 词牌化_大写::skipWhitespace()
{
    while (matchRegex(whitespaceRegex)) {
        m_iter += m_token.size();
    }
}

static 制作树语言值指针_小写 readAtom(词牌化_大写& tokeniser);
static 制作树语言值指针_小写 readForm(词牌化_大写& tokeniser);
static void readList(词牌化_大写& tokeniser, 制作树语言值向量_小写* items,
                      const 字符串_大写& end);
static 制作树语言值指针_小写 processMacro(词牌化_大写& tokeniser, const 字符串_大写& symbol);

制作树语言值指针_小写 readStr(const 字符串_大写& input)
{
    词牌化_大写 tokeniser(input);
    if (tokeniser.eof()) {
        throw 制作树语言空输入异常_小写();
    }
    return readForm(tokeniser);
}

static 制作树语言值指针_小写 readForm(词牌化_大写& tokeniser)
{
    MAL_CHECK(!tokeniser.eof(), "期望形式, 得到EOF");
    字符串_大写 token = tokeniser.peek();

    MAL_CHECK(!std::regex_match(token, closeRegex),
            "不期望 '%s'", token.c_str());

    if (token == "(") {
        tokeniser.next();
        std::unique_ptr<制作树语言值向量_小写> items(new 制作树语言值向量_小写);
        readList(tokeniser, items.get(), ")");
        return 制作树语言_小写::list(items.release());
    }
    if (token == "[") {
        tokeniser.next();
        std::unique_ptr<制作树语言值向量_小写> items(new 制作树语言值向量_小写);
        readList(tokeniser, items.get(), "]");
        return 制作树语言_小写::vector(items.release());
    }
    if (token == "{") {
        tokeniser.next();
        制作树语言值向量_小写 items;
        readList(tokeniser, &items, "}");
        return 制作树语言_小写::hash(items.begin(), items.end(), false);
    }
    return readAtom(tokeniser);
}

static 制作树语言值指针_小写 readAtom(词牌化_大写& tokeniser)
{
    struct ReaderMacro {
        const char* token;
        const char* symbol;
    };
    ReaderMacro macroTable[] = {
        { "@",   "解引用" },
        { "`",   "准引号" },
        { "'",   "引号" },
        { "~@",  "拼接-反引号" },
        { "~",   "反引号" },
    };

    struct Constant {
        const char* token;
        制作树语言值指针_小写 value;
    };
    Constant constantTable[] = {
        { "假",  制作树语言_小写::falseValue()  },
        { "无值",    制作树语言_小写::nilValue()          },
        { "真",   制作树语言_小写::trueValue()   },
    };

    字符串_大写 token = tokeniser.next();
    if (token[0] == '"') {
        return 制作树语言_小写::string(unescape(token));
    }
    if (token[0] == ':') {
        return 制作树语言_小写::keyword(token);
    }
    if (token == "^") {
        制作树语言值指针_小写 meta = readForm(tokeniser);
        制作树语言值指针_小写 value = readForm(tokeniser);
        // Note that meta and value switch places
        return 制作树语言_小写::list(制作树语言_小写::symbol("带有-元"), value, meta);
    }
    for (auto &constant : constantTable) {
        if (token == constant.token) {
            return constant.value;
        }
    }
    for (auto &macro : macroTable) {
        if (token == macro.token) {
            return processMacro(tokeniser, macro.symbol);
        }
    }
    if (std::regex_match(token, intRegex)) {
        return 制作树语言_小写::integer(token);
    }
    return 制作树语言_小写::symbol(token);
}

static void readList(词牌化_大写& tokeniser, 制作树语言值向量_小写* items,
                      const 字符串_大写& end)
{
    while (1) {
        MAL_CHECK(!tokeniser.eof(), "期望 '%s', 得到EOF", end.c_str());
        if (tokeniser.peek() == end) {
            tokeniser.next();
            return;
        }
        items->push_back(readForm(tokeniser));
    }
}

static 制作树语言值指针_小写 processMacro(词牌化_大写& tokeniser, const 字符串_大写& symbol)
{
    return 制作树语言_小写::list(制作树语言_小写::symbol(symbol), readForm(tokeniser));
}
