// Huffman.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>
#include <numeric>
#include <memory>
#include <queue>

std::vector<char> read_file(std::string fn) {
	std::vector<char> res;

	if (std::ifstream is{ fn, std::ios::binary | std::ios::ate }) {
		auto size = is.tellg();
		res.resize(size);

		is.seekg(0);
		is.read(&res[0], size);
	}
	return res;
}

struct Node {
	int freq = 0;
	char ch = 0;
	
	Node* left;
	Node* right;

	bool is_leaf() {
		return !(left && right);
	}

	bool operator< (const Node &other) const {
		return freq < other.freq;
	}

	bool operator> (const Node &other) const {
		return freq > other.freq;
	}
};

void print_node(Node* n, std::string prefix = "") {

	if (n->is_leaf()) {
		return;
	}

	if (n->left->is_leaf()) {
		std::cout << (int)(n->left->ch) << " " << prefix << "0" << std::endl;
	} else {
		print_node(n->left, prefix + "0");
	}
	
	if (n->right->is_leaf()) {
		std::cout << (int)(n->right->ch) << " " << prefix << "1" << std::endl;
	} else {
		print_node(n->right, prefix + "1");
	}
}

void serialize_node(Node* n, std::ofstream& fs, std::string prefix = "") {

	if (n->is_leaf()) {
		return;
	}

	if (n->left->is_leaf()) {
		fs << (int)(n->left->ch) << " " << prefix << "0" << std::endl;
	}
	else {
		serialize_node(n->left, fs, prefix + "0");
	}

	if (n->right->is_leaf()) {
		fs << (int)(n->right->ch) << " " << prefix << "1" << std::endl;
	}
	else {
		serialize_node(n->right, fs, prefix + "1");
	}
}

void serialize_to_file(Node* n, std::string file_name) {
	std::ofstream fs;
	fs.open(file_name);
	
	serialize_node(n, fs);

	fs.close();
}


int main()
{
	// Read source material to be compressed
	std::vector<char> content = read_file("source.txt");


	// Init char frequencies
	std::array<int, 256> freq{ 0 };

	for (int i = 0; i < content.size(); ++i) {
		char current_char = content[i];
		freq[current_char]++;
	}

	// Record chars to unique_ptr Nodes
	std::vector<std::unique_ptr<Node>> node_storage;

	for (int i = 0; i < freq.size(); ++i) {
		if (freq[i] > 0) {
			node_storage.push_back(std::make_unique<Node>());
			node_storage.back()->freq = freq[i];
			node_storage.back()->ch = i;
		}
	}

	// Declaring priority queue to construct heap tree from nodes' unique pointers 
	auto cmp = [](const Node* lhs, const Node* rhs) { return *lhs > *rhs; };
	std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

	// Filling original leaves' pointers to pq
	for (auto& node : node_storage) {
		pq.push(node.get());
	}

	auto x = pq.top();

	// Constructing inner nodes and assigning relationships
	while(pq.size() > 1) {
		std::unique_ptr<Node> n = std::make_unique<Node>();

		n->left = pq.top();
		pq.pop();

		n->right = pq.top();
		pq.pop();

		n->freq = n->left->freq + n->right->freq;

		node_storage.push_back(std::move(n));
		pq.push(node_storage.back().get());
	}

	// Root of the tree
	auto root = pq.top();

	// Print out Huffman tree
	print_node(root);

	// Save to file
	serialize_to_file(root, "output_tree.txt");

	std::string endc;
	std::cin >> endc;

	return 0;
}
