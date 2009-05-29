// Copyright (c) 2009 - Decho Corp.

#include "http.h"

#include <cassert>

static std::string quote(const std::string& str)
{
    if (str.empty())
        return "\"\"";

    if (str.find_first_of("!#$%&'*+-./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz|~") == std::string::npos) {
        return str;
    }

    std::string result;
    result.reserve(str.length() + 2);
    result.append(1, '"');

    size_t lastEscape = 0;
    size_t nextEscape = std::min(str.find('\\'), str.find('"'));
    while (nextEscape != std::string::npos) {
        result.append(str.substr(lastEscape, nextEscape - lastEscape));
        result.append(1, '\\');
        result.append(1, str[nextEscape]);
        lastEscape = nextEscape + 1;
        nextEscape = std::min(str.find('\\', lastEscape), str.find('"', lastEscape));
    }
    result.append(str.substr(lastEscape));
    result.append(1, '"');
    return result;
}

static std::ostream& serialize(std::ostream& os, const HTTP::StringSet& set)
{
    for (HTTP::StringSet::const_iterator it(set.begin());
        it != set.end();
        ++it) {
        if (it != set.begin())
            os << ", ";
        os << *it;
    }
    return os;
}

static const char *methods[] = {
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE"
};

std::ostream& operator<<(std::ostream& os, HTTP::Method m)
{
    assert(m >= HTTP::GET && m <= HTTP::TRACE);
    return os << methods[(size_t)m];
}

std::ostream& operator<<(std::ostream& os, HTTP::Version v)
{
    assert(v.major != ~0 && v.minor != ~0);
    return os << "HTTP/" << v.major << "." << v.minor;    
}

std::ostream& operator<<(std::ostream& os, const HTTP::RequestLine& r)
{
    return os << r.method << " " << r.uri << " " << r.ver;
}

std::ostream& operator<<(std::ostream& os, const HTTP::StatusLine& s)
{
    assert(!s.reason.empty());
    return os << s.ver << " " << (int)s.status << " " << s.reason;
}

std::ostream& operator<<(std::ostream& os, const HTTP::GeneralHeaders& g)
{
    if (!g.connection.empty())
        os << "Connection: " << serialize(os, g.connection) << "\r\n";
    if (!g.transferEncoding.empty()) {
        os << "Transfer-Encoding: ";
        for (HTTP::ParameterizedList::const_iterator it(g.transferEncoding.begin());
            it != g.transferEncoding.end();
            ++it) {
            if (it != g.transferEncoding.begin())
                os << ", ";
            os << it->value;
            for (HTTP::StringMap::const_iterator it2(it->parameters.begin());
                it2 != it->parameters.end();
                ++it2) {
                os << ":" << it2->first << "=" << quote(it2->second);
            }
        }
        os << "\r\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const HTTP::RequestHeaders& r)
{
    if (!r.host.empty())
        os << "Host: " << r.host << "\r\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const HTTP::ResponseHeaders& r)
{
    if (r.location.isDefined())
        os << "Location: " << r.location << "\r\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const HTTP::EntityHeaders& e)
{
    if (e.contentLength != ~0)
        os << "Content-Length: " << e.contentLength << "\r\n";
    for (HTTP::StringMap::const_iterator it(e.extension.begin());
        it != e.extension.end();
        ++it) {
        os << it->first << ": " << it->second << "\r\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const HTTP::Request& r)
{
    return os << r.requestLine << "\r\n"
        << r.general
        << r.request
        << r.entity << "\r\n";
}

std::ostream& operator<<(std::ostream& os, const HTTP::Response& r)
{
    return os << r.status << "\r\n"
        << r.general
        << r.response
        << r.entity << "\r\n";
}