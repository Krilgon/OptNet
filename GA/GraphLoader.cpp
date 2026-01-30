#include "GraphLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <iostream>
#include <algorithm>

// -----------------------------
// Small JSON helpers (targeted)
// -----------------------------
static void skipWs(const std::string& s, size_t& i) {
    while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
}

static bool consume(const std::string& s, size_t& i, char c) {
    skipWs(s, i);
    if (i < s.size() && s[i] == c) { ++i; return true; }
    return false;
}

static void expect(const std::string& s, size_t& i, char c, const char* msg) {
    if (!consume(s, i, c)) throw std::runtime_error(msg);
}

static std::string parseString(const std::string& s, size_t& i) {
    skipWs(s, i);
    if (i >= s.size() || s[i] != '"') throw std::runtime_error("JSON: expected string");
    ++i;
    std::string out;
    while (i < s.size()) {
        char ch = s[i++];
        if (ch == '"') break;
        if (ch == '\\') {
            if (i >= s.size()) throw std::runtime_error("JSON: bad escape");
            char esc = s[i++];
            switch (esc) {
            case '"': out.push_back('"'); break;
            case '\\': out.push_back('\\'); break;
            case '/': out.push_back('/'); break;
            case 'b': out.push_back('\b'); break;
            case 'f': out.push_back('\f'); break;
            case 'n': out.push_back('\n'); break;
            case 'r': out.push_back('\r'); break;
            case 't': out.push_back('\t'); break;
            default: out.push_back(esc); break;
            }
        }
        else {
            out.push_back(ch);
        }
    }
    return out;
}

static double parseNumber(const std::string& s, size_t& i) {
    skipWs(s, i);
    size_t start = i;
    if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
    bool dot = false;
    while (i < s.size()) {
        char c = s[i];
        if (std::isdigit((unsigned char)c)) { ++i; continue; }
        if (c == '.' && !dot) { dot = true; ++i; continue; }
        if ((c == 'e' || c == 'E')) {
            ++i;
            if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
            while (i < s.size() && std::isdigit((unsigned char)s[i])) ++i;
            break;
        }
        break;
    }
    if (start == i) throw std::runtime_error("JSON: expected number");
    return std::stod(s.substr(start, i - start));
}

static void skipValue(const std::string& s, size_t& i);

static void skipObject(const std::string& s, size_t& i) {
    expect(s, i, '{', "JSON: expected {");
    skipWs(s, i);
    if (consume(s, i, '}')) return;
    while (true) {
        (void)parseString(s, i);
        expect(s, i, ':', "JSON: expected :");
        skipValue(s, i);
        skipWs(s, i);
        if (consume(s, i, '}')) break;
        expect(s, i, ',', "JSON: expected ,");
    }
}

static void skipArray(const std::string& s, size_t& i) {
    expect(s, i, '[', "JSON: expected [");
    skipWs(s, i);
    if (consume(s, i, ']')) return;
    while (true) {
        skipValue(s, i);
        skipWs(s, i);
        if (consume(s, i, ']')) break;
        expect(s, i, ',', "JSON: expected ,");
    }
}

static void skipValue(const std::string& s, size_t& i) {
    skipWs(s, i);
    if (i >= s.size()) throw std::runtime_error("JSON: unexpected end");
    char c = s[i];
    if (c == '"') { (void)parseString(s, i); return; }
    if (c == '{') { skipObject(s, i); return; }
    if (c == '[') { skipArray(s, i); return; }
    if (std::isdigit((unsigned char)c) || c == '-' || c == '+') { (void)parseNumber(s, i); return; }
    if (s.compare(i, 4, "true") == 0) { i += 4; return; }
    if (s.compare(i, 5, "false") == 0) { i += 5; return; }
    if (s.compare(i, 4, "null") == 0) { i += 4; return; }
    throw std::runtime_error("JSON: bad value");
}

static NodeType typeFromString(const std::string& t) {
    if (t == "PC") return NodeType::PC;
    if (t == "COMPUTE") return NodeType::COMPUTE;
    if (t == "SERVER") return NodeType::SERVER;
    if (t == "STORAGE") return NodeType::STORAGE;
    if (t == "GATEWAY") return NodeType::GATEWAY;
    return NodeType::UNKNOWN;
}

static bool isKnownKey(const std::string& key) {
    return key == "nodes" || key == "edges" || key == "start_node" || key == "end_node";
}

// -----------------------------
// Parsing specific structures
// -----------------------------
static void parseNodesArray(const std::string& s, size_t& i, Graph& g) {
    expect(s, i, '[', "JSON: nodes should be array");
    skipWs(s, i);
    if (consume(s, i, ']')) return;

    while (true) {
        expect(s, i, '{', "JSON: node should be object");
        Node n;

        skipWs(s, i);
        if (!consume(s, i, '}')) {
            while (true) {
                std::string key = parseString(s, i);
                expect(s, i, ':', "JSON: expected : in node");

                if (key == "id") n.id = (int)parseNumber(s, i);
                else if (key == "performance") n.performance = (int)parseNumber(s, i);
                else if (key == "type") n.type = typeFromString(parseString(s, i));
                else skipValue(s, i);

                skipWs(s, i);
                if (consume(s, i, '}')) break;
                expect(s, i, ',', "JSON: expected , in node");
            }
        }

        g.nodes[n.id] = n;

        skipWs(s, i);
        if (consume(s, i, ']')) break;
        expect(s, i, ',', "JSON: expected , after node");
    }
}

static void parseEdgesArray(const std::string& s, size_t& i, Graph& g) {
    expect(s, i, '[', "JSON: edges should be array");
    skipWs(s, i);
    if (consume(s, i, ']')) return;

    while (true) {
        expect(s, i, '{', "JSON: edge should be object");
        Edge e;

        skipWs(s, i);
        if (!consume(s, i, '}')) {
            while (true) {
                std::string key = parseString(s, i);
                expect(s, i, ':', "JSON: expected : in edge");

                if (key == "node_a") e.node_a = (int)parseNumber(s, i);
                else if (key == "node_b") e.node_b = (int)parseNumber(s, i);
                else if (key == "latency") e.latency = parseNumber(s, i);
                else if (key == "bandwidth") e.bandwidth = parseNumber(s, i);
                else skipValue(s, i);

                skipWs(s, i);
                if (consume(s, i, '}')) break;
                expect(s, i, ',', "JSON: expected , in edge");
            }
        }

        g.edges.push_back(e);

        skipWs(s, i);
        if (consume(s, i, ']')) break;
        expect(s, i, ',', "JSON: expected , after edge");
    }
}

Graph GraphLoader::loadFromFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("GraphLoader: cannot open file: " + path);

    std::ostringstream ss;
    ss << in.rdbuf();
    std::string s = ss.str();

    Graph g;

    size_t i = 0;
    expect(s, i, '{', "JSON: root must be object");

    skipWs(s, i);
    if (consume(s, i, '}')) throw std::runtime_error("GraphLoader: empty JSON");

    while (true) {
        std::string key = parseString(s, i);
        expect(s, i, ':', "JSON: expected : in root");

        if (key == "nodes") parseNodesArray(s, i, g);
        else if (key == "edges") parseEdgesArray(s, i, g);
        else if (key == "start_node") g.start_node = (int)parseNumber(s, i);
        else if (key == "end_node") g.end_node = (int)parseNumber(s, i);
        else skipValue(s, i);

        skipWs(s, i);
        if (consume(s, i, '}')) break;
        expect(s, i, ',', "JSON: expected , in root");
    }

    if (g.nodes.empty()) throw std::runtime_error("GraphLoader: nodes is empty");
    if (g.edges.empty()) throw std::runtime_error("GraphLoader: edges is empty");

    // If generator didn't write start/end, choose sane defaults
    if (!g.nodes.count(g.start_node)) {
        // prefer a PC if exists, else smallest id
        int best = g.nodes.begin()->first;
        for (auto& kv : g.nodes) best = std::min(best, kv.first);
        for (auto& kv : g.nodes) {
            if (kv.second.type == NodeType::PC) { best = kv.first; break; }
        }
        g.start_node = best;
    }
    if (!g.nodes.count(g.end_node)) {
        // prefer a SERVER if exists, else largest id
        int best = g.nodes.begin()->first;
        for (auto& kv : g.nodes) best = std::max(best, kv.first);
        for (auto& kv : g.nodes) {
            if (kv.second.type == NodeType::SERVER) { best = kv.first; break; }
        }
        g.end_node = best;
    }

    std::cout << "[GraphLoader] Loaded graph. Nodes=" << g.nodes.size()
        << " Edges=" << g.edges.size()
        << " Start=" << g.start_node
        << " End=" << g.end_node << "\n";

    return g;
}
