#include <iostream>
#include <cmath>
#include <queue>
//#include <ql/quantlib.hpp>
using namespace std;

template <typename T>
class Node {
public:
    long timestamp;
    T value;
    Node(long a, T b) {timestamp = a, value = b;};
    Node() = default;
    Node operator+(const Node& rhs){
        return Node((timestamp<rhs.timestamp)?rhs.timestamp:timestamp, value+rhs.value);
    }
    virtual pair<long, T> evaluate() {
        return make_pair(timestamp, value);
    }
    friend std::ostream& operator<< (std::ostream &strm, const Node &node) {
        return strm << "Node(" << node.timestamp << "," << node.value << ")";
    }
};

template <typename T>
class OneSidedNode : public Node<T> {
public:
    Node<T> &rhs;
    OneSidedNode() = default;
    OneSidedNode(Node<T> &rhs) : rhs(rhs) {};
};

template <typename T>
class TwoSidedNode : public Node<T> {
public:
    Node<T> &lhs, &rhs;
    TwoSidedNode() = default;
    TwoSidedNode(Node<T> &lhs, Node<T> &rhs) : lhs(lhs), rhs(rhs) {};
};

template <typename T>
class NodeClipLower : public OneSidedNode<T> {
public:
    using OneSidedNode<T>::rhs;
    using OneSidedNode<T>::timestamp;
    using OneSidedNode<T>::value;
    float lower;
    NodeClipLower(Node<T> &rhs, float lower) : OneSidedNode<T>(rhs), lower(lower) {};
    pair<long, T> evaluate() {
        pair<long, T> eval = rhs.evaluate();
        timestamp = eval.first;
        value = eval.second<lower?lower:eval.second;
        return make_pair(timestamp, value);
    };
    friend std::ostream& operator<< (std::ostream &strm, const NodeClipLower<T> &node) {
        return strm << "NodeClipLower(" << node.timestamp << "," << node.value << ",lower=" << node.lower << ")";
    };
};

template <typename T>
class NodeClipUpper : public OneSidedNode<T> {
public:
    using OneSidedNode<T>::rhs;
    using OneSidedNode<T>::timestamp;
    using OneSidedNode<T>::value;
    float upper;
    NodeClipUpper(Node<T> &rhs, T upper) : OneSidedNode<T>(rhs), upper(upper) {};
    pair<long, T> evaluate() {
        pair<long, T> eval = rhs.evaluate();
        timestamp = eval.first;
        value = eval.second>upper?upper:eval.second;
        return make_pair(timestamp, value);
    };
    friend std::ostream& operator<< (std::ostream &strm, const NodeClipUpper<T> &node) {
        return strm << "NodeClipUpper(" << node.timestamp << "," << node.value << ",upper=" << node.upper << ")";
    };
};

template <typename T>
class NodeClip : public OneSidedNode<T> {
public:
    using OneSidedNode<T>::rhs;
    using OneSidedNode<T>::timestamp;
    using OneSidedNode<T>::value;
    float upper, lower;
    NodeClip(Node<T> &rhs, T lower, T upper) : OneSidedNode<T>(rhs), lower(lower), upper(upper) {};
    pair<long, T> evaluate() {
        pair<long, float> eval = rhs.evaluate();
        timestamp = eval.first;
        value = eval.second<lower?lower:eval.second;
        value = value>upper?upper:value;
        return make_pair(timestamp, value);
    };
    friend std::ostream& operator<< (std::ostream &strm, const NodeClip &node) {
        return strm << "NodeClip(" << node.timestamp << "," << node.value << ",lower=" << node.lower << ",upper=" << node.upper << ")";
    };
};

template <typename T>
class NodeAbs : public OneSidedNode<T> {
public:
    using OneSidedNode<T>::rhs;
    using OneSidedNode<T>::timestamp;
    using OneSidedNode<T>::value;
    NodeAbs(Node<T> &rhs) : OneSidedNode<T>(rhs) {};
    pair<long, T> evaluate() {
        pair<long, T> eval = rhs.evaluate();
        timestamp = eval.first;
        value = std::abs (eval.second);
        return make_pair(timestamp, value);
    }
    friend std::ostream& operator<< (std::ostream &strm, const NodeAbs &node) {
        return strm << "NodeAbs(" << node.timestamp << "," << node.value << ")";
    };
};

template <typename T>
class MaxQueue: public queue<T> {
public:
    int maxlen;
    MaxQueue(int a) {maxlen = a;};
    void push(T a) {
        while (queue<T>::size() + 1 > maxlen) {
            queue<T>::pop();
        }
        queue<T>::push(a);
    }
};

template <typename T>
class MaxDeque: public deque<T> {
public:
    int maxlen;
    MaxDeque(int a) {maxlen = a;};
    void push_back(T a) {
        while (deque<T>::size() + 1 > maxlen) {
            deque<T>::pop_front();
        }
        deque<T>::push_back(a);
    }
    void push_front(T a) {
        while (deque<T>::size() + 1  > maxlen) {
            deque<T>::pop_back();
        }
        deque<T>::push_front(a);
    }
    using deque<T>::size;
};

template <typename T>
class NodeDeque : public OneSidedNode<T> {
public:
    using OneSidedNode<T>::rhs;
    using OneSidedNode<T>::timestamp;
    int maxlen;
    float (*op)(MaxDeque<float>);
    MaxDeque<T> maxdeque = MaxDeque<T>(maxlen);
    NodeDeque() = default;
    NodeDeque(Node<T> &rhs, int maxlen, float (*op)(MaxDeque<float>)) : OneSidedNode<T>(rhs), maxlen(maxlen), op(op) {};
    pair<long, float> evaluate() {
        pair<long, float> eval = rhs.evaluate();
        if (eval.first != timestamp) {
            timestamp = eval.first;
            maxdeque.push_back(eval.second);
        }
        return make_pair(this->timestamp, op(maxdeque));
    }
};

template <typename T>
float deque_diff(MaxDeque<T> maxdeque) {
    return maxdeque.back() - maxdeque.front();
}

template <typename T>
float deque_pdiff(MaxDeque<T> maxdeque) {
    return (maxdeque.back() / maxdeque.front()) - T(1.0);
}

template <typename T>
float deque_shift(MaxDeque<T> maxdeque) {
    return maxdeque.front();
}

template <typename T>
class NodeDiff : public NodeDeque<T> {
public:
    NodeDiff(Node<T> &rhs, int maxlen) : NodeDeque<T>(rhs, maxlen, deque_diff<T>) {};
};

template <typename T>
class NodePDiff : public NodeDeque<T> {
public:
    NodePDiff(Node<T> &rhs, int maxlen) : NodeDeque<T>(rhs, maxlen, deque_pdiff<T>) {};
};

template <typename T>
class NodeShift : public NodeDeque<T> {
public:
    NodeShift(Node<T> &rhs, int maxlen) : NodeDeque<T>(rhs, maxlen, deque_shift<T>) {};
};

template <typename T>
class NodeOuterProduct : public TwoSidedNode<T> {
public:
    using TwoSidedNode<T>::lhs;
    using TwoSidedNode<T>::rhs;
    using TwoSidedNode<T>::timestamp;
    using TwoSidedNode<T>::value;
    NodeOuterProduct(Node<T> &lhs, Node<T> &rhs) : TwoSidedNode<T>(lhs, rhs) {};
    pair<long, T> evaluate() {
        pair<long, T> lhs_eval = lhs.evaluate();
        pair<long, T> rhs_eval = rhs.evaluate();
        long timestamp = lhs_eval.first > rhs_eval.first ? lhs_eval.first : rhs_eval.first;
//      This needs to be expanded to deal with outer product of vectors
        T value = lhs_eval.second * rhs_eval.second;
        return make_pair(timestamp, value);
    }
};

template <typename T>
class NodeSmooth : public OneSidedNode<T> {
private:
public:
    int halfLife, meanSeedPeriod, meanSeedOverride;
    int counter = 0;
    float filter = 0;
    T cumSum = 0.0;
    T value = 0, timestamp = 0, gamma;
    using OneSidedNode<T>::rhs;
    NodeSmooth(Node<T> &rhs, int halfLife, int meanSeedPeriod, int meanSeedOverride) : OneSidedNode<T>(rhs), halfLife(halfLife), meanSeedPeriod(meanSeedPeriod), meanSeedOverride(meanSeedOverride), gamma(pow(0.5, 1.0 / float(halfLife))) {};
    pair<long, T> evaluate() {
        pair<long, T> rhsEval = rhs.evaluate();
        if (rhsEval.first != timestamp) {
            timestamp = rhsEval.first;
            if (filter == 0) {
                cumSum += rhsEval.second;
                counter += 1;
                if (counter == meanSeedPeriod) {
                    filter = cumSum / float(meanSeedPeriod);
                }
            } else {
                filter = gamma * filter + (1.0 - gamma) * rhsEval.second;
                value = filter;
            }
        }
        return make_pair(timestamp, value);
    }
};

template <typename T>
class MockDataSource : public Node<T> {
public:
    vector<long> timestamps;
    vector<T> values;
    int count = 0;
    MockDataSource(vector<long> _timestamps, vector<T> _values) {timestamps = _timestamps, values = _values;};
    pair<long, T> evaluate() {
        return make_pair(timestamps[count], values[count]);
    }
    void next() {
        if (count == sizeof(timestamps)) {
            cout << "Already reached the end of the MockDataSource";
        }
        else {
            count++;
        }
    }
};
