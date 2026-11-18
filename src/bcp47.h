/**
 * Copyright 2026 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <string_view>
#include <vector>

namespace Firebolt::Bcp47
{
inline bool isAlpha(char value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
}

inline bool isDigit(char value)
{
    return value >= '0' && value <= '9';
}

inline bool isAlphanumeric(char value)
{
    return isAlpha(value) || isDigit(value);
}

inline bool consistsOf(std::string_view value, bool (*predicate)(char))
{
    for (const char character : value)
    {
        if (!predicate(character))
        {
            return false;
        }
    }
    return true;
}

inline bool equalsIgnoreCase(std::string_view left, std::string_view right)
{
    if (left.size() != right.size())
    {
        return false;
    }

    for (size_t i = 0; i < left.size(); ++i)
    {
        char leftCharacter = left[i];
        char rightCharacter = right[i];
        if (leftCharacter >= 'A' && leftCharacter <= 'Z')
        {
            leftCharacter = static_cast<char>(leftCharacter - 'A' + 'a');
        }
        if (rightCharacter >= 'A' && rightCharacter <= 'Z')
        {
            rightCharacter = static_cast<char>(rightCharacter - 'A' + 'a');
        }
        if (leftCharacter != rightCharacter)
        {
            return false;
        }
    }
    return true;
}

inline bool isValid(std::string_view language)
{
    static constexpr std::array<std::string_view, 26> grandfatheredTags =
        {"art-lojban", "cel-gaulish", "en-gb-oed", "i-ami",      "i-bnn",     "i-default", "i-enochian",
         "i-hak",      "i-klingon",   "i-lux",     "i-mingo",    "i-navajo",  "i-pwn",     "i-tao",
         "i-tay",      "i-tsu",       "no-bok",    "no-nyn",     "sgn-be-fr", "sgn-be-nl", "sgn-ch-de",
         "zh-guoyu",   "zh-hakka",    "zh-min",    "zh-min-nan", "zh-xiang"};

    std::vector<std::string_view> subtags;
    size_t start = 0;
    while (start <= language.size())
    {
        const size_t separator = language.find('-', start);
        const size_t end = separator == std::string_view::npos ? language.size() : separator;
        const auto subtag = language.substr(start, end - start);
        if (subtag.empty())
        {
            return false;
        }
        subtags.push_back(subtag);
        if (separator == std::string_view::npos)
        {
            break;
        }
        start = separator + 1;
    }

    for (const auto tag : grandfatheredTags)
    {
        if (equalsIgnoreCase(language, tag))
        {
            return true;
        }
    }

    const auto isPrivateSubtag = [](std::string_view value)
    { return !value.empty() && value.size() <= 8 && consistsOf(value, isAlphanumeric); };

    if (equalsIgnoreCase(subtags.front(), "x"))
    {
        if (subtags.size() < 2)
        {
            return false;
        }
        for (size_t i = 1; i < subtags.size(); ++i)
        {
            if (!isPrivateSubtag(subtags[i]))
            {
                return false;
            }
        }
        return true;
    }

    size_t index = 0;
    const auto languageSubtag = subtags[index++];
    if (!consistsOf(languageSubtag, isAlpha))
    {
        return false;
    }

    if (languageSubtag.size() >= 2 && languageSubtag.size() <= 3)
    {
        for (size_t extlang = 0;
             extlang < 3 && index < subtags.size() && subtags[index].size() == 3 && consistsOf(subtags[index], isAlpha);
             ++extlang)
        {
            ++index;
        }
    }
    else if (languageSubtag.size() != 4 && (languageSubtag.size() < 5 || languageSubtag.size() > 8))
    {
        return false;
    }

    if (index < subtags.size() && subtags[index].size() == 4 && consistsOf(subtags[index], isAlpha))
    {
        ++index;
    }
    if (index < subtags.size() && ((subtags[index].size() == 2 && consistsOf(subtags[index], isAlpha)) ||
                                   (subtags[index].size() == 3 && consistsOf(subtags[index], isDigit))))
    {
        ++index;
    }

    while (index < subtags.size() &&
           ((subtags[index].size() >= 5 && subtags[index].size() <= 8 && consistsOf(subtags[index], isAlphanumeric)) ||
            (subtags[index].size() == 4 && isDigit(subtags[index].front()) && consistsOf(subtags[index], isAlphanumeric))))
    {
        ++index;
    }

    std::array<bool, 36> seenExtensionSingletons{};
    while (index < subtags.size() && subtags[index].size() == 1 && isAlphanumeric(subtags[index].front()) &&
           !equalsIgnoreCase(subtags[index], "x"))
    {
        char singleton = subtags[index++].front();
        if (singleton >= 'A' && singleton <= 'Z')
        {
            singleton = static_cast<char>(singleton - 'A' + 'a');
        }
        const size_t singletonIndex = isDigit(singleton) ? static_cast<size_t>(singleton - '0')
                                                         : static_cast<size_t>(singleton - 'a' + 10);
        if (seenExtensionSingletons[singletonIndex])
        {
            return false;
        }
        seenExtensionSingletons[singletonIndex] = true;

        const size_t extensionStart = index;
        while (index < subtags.size() && subtags[index].size() >= 2 && subtags[index].size() <= 8 &&
               consistsOf(subtags[index], isAlphanumeric))
        {
            ++index;
        }
        if (index == extensionStart)
        {
            return false;
        }
    }

    if (index < subtags.size() && equalsIgnoreCase(subtags[index], "x"))
    {
        ++index;
        const size_t privateStart = index;
        while (index < subtags.size() && isPrivateSubtag(subtags[index]))
        {
            ++index;
        }
        if (index == privateStart)
        {
            return false;
        }
    }

    return index == subtags.size();
}
} // namespace Firebolt::Bcp47
