#include <iostream>
#include <queue>
#include "nodes.h"

using namespace std;

int main() {
    Node<float> node1(2, 3.0);
    Node<float> node2(3, 4.0);
    cout << node1 << "\n";
    cout << node2 << "\n";
    cout << node1 + node2 << "\n";
    NodeClipLower<float> node_clip_lower(node1, 5.0);
    cout << node_clip_lower << "\n";
    node_clip_lower.evaluate();
    cout << node_clip_lower << "\n";
    cout << NodeClipUpper<float>(node1, 2.0) << "\n";
    cout << NodeClip<float>(node1, 2.0, 4.0) << "\n";
    Node<float> neg_node(2, -float(1.0));
    cout << NodeAbs<float>(neg_node) << "\n";
    pair<long, float> mypair;
    mypair = node1.evaluate();
    cout << mypair.first << "," << mypair.second << " evaluated" << "\n";
    int maxlen = 5;
    std::deque<int> mydeque;
    for (int i = 0; i <= 10; i++) {
        mydeque.push_back(i);
        while (mydeque.size() > maxlen) {
            mydeque.pop_front();
        };
        for (int j = 0; j < mydeque.size(); j++) {
            cout << mydeque.at(j) << " ";
        }
        cout << "\n";
    }
    cout << "MaxQueue\n";
    MaxQueue<float> maxqueue(5);
    for (int i = 0; i <= 10; i++) {
        maxqueue.push(float(i)/float(10.0));
    }
    cout << "maxqueue.size()= " << maxqueue.size() << "\n";
    long n = maxqueue.size();
    for (int z = 0; z < n; z++) {
        cout << z << "  " << maxqueue.front() << "\n";
        maxqueue.pop();
    }
    cout << "\n";
    cout << "MaxDeque\n";
    MaxDeque<float> maxdeque(5);
    for (int i = 0; i <= 10; i++) {
        maxdeque.push_front(float(i)/float(10.0));
    }
    cout << "maxdeque.size()= " << maxdeque.size() << "\n";
    long m = maxqueue.size();
    for (int z = 0; z < m; z++) {
        cout << z << "  " << maxqueue.front() << "\n";
        maxqueue.pop();
    }
    cout << "\n";
    vector<long> timestamps;
    vector<float> values;
    for (long i=0; i <=10; i++) {
        timestamps.push_back(i);
        values.push_back(float(i)*float(10));
    }
    MockDataSource<float> mockdatasource(timestamps, values);
    cout << &mockdatasource << "\n";
//    NodeAbs<float> node_abs(mockdatasource);
//    NodeClipUpper<float> node_clip_upper(node_abs, float(50));
//    for (int i=0; i<=5; i++) {
//        pair<long, float> eval = mockdatasource.evaluate();
//        pair<long, float> eval_abs = node_abs.evaluate();
//        pair<long, float> eval_clip = node_clip_upper.evaluate();
//        cout << eval.first << "," << eval.second << " " << eval_abs.first << "," << eval_abs.second << " " << eval_clip.first << "," << eval_clip.second << "\n";
//        mockdatasource.next();
//    }
//    NodeOuterProduct<float> node_outer_product(mockdatasource, mockdatasource);
//    pair<long, float> node_outer_eval = node_outer_product.evaluate();
//    cout << node_outer_eval.first << "," << node_outer_eval.second << "\n";
    cout << "\n";
    MockDataSource<float> mockdatasource2(timestamps, values);
    NodeDiff<float> nodediff(mockdatasource2, 5);
    NodePDiff<float> nodepdiff(mockdatasource2, 5);
    NodeShift<float> nodeshift(mockdatasource2, 5);
    NodeSmooth<float> node_smooth(mockdatasource2, 3, 3, 2);
    for (int i=0; i<=10; i++) {
        pair<long, float> eval = mockdatasource2.evaluate();
        mockdatasource2.next();
        cout << "mock2 i=" << i << " " << eval.first << "," << eval.second << "\n";
        pair<long, float> eval_diff = nodediff.evaluate();
        cout << "diff i=" << i << " " << eval_diff.first << "," << eval_diff.second << "\n";
        pair<long, float> eval_pdiff = nodepdiff.evaluate();
        cout << "pdiff i=" << i << " " << eval_pdiff.first << "," << eval_pdiff.second << "\n";
        pair<long, float> eval_shift = nodeshift.evaluate();
        cout << "shift i=" << i << " " << eval_shift.first << "," << eval_shift.second << "\n";
        pair<long, float> eval_smooth = node_smooth.evaluate();
        cout << "smooth i=" << i << " " << eval_smooth.first << "," << eval_smooth.second << " counter = " << node_smooth.counter << "\n";
    }
    float (*foo)(MaxDeque<float>) = &deque_diff;
    cout << foo(maxdeque) << '\n';

}