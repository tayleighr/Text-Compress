#ifndef HUFF_COMPRESS_H
#define HUFF_COMPRESS_H

#include <iostream> 
#include <string>
#include <memory>
#include <array>


using namespace std;

class HuffmanTree
{

public:

	HuffmanTree();
	~HuffmanTree();
	
	/*
	Compressing methods
	*/
	bool makeCompressedFile(string inputFile, string outputFile);

	/*
	Decompressing methods
	*/
	bool decompressFile(string inputFile, string outputFile);


private:

	struct node
	{
	
		node() :freq(0), ch('\0'), left(nullptr), right(nullptr) {}
	
		int freq;
		unsigned char ch;

		shared_ptr<node> left;
		shared_ptr<node> right;
	};

	struct compareFreq
	{
		bool operator()(const shared_ptr<node> lhs, const shared_ptr<node> rhs) const
		{
			return lhs->freq > rhs->freq;
		}
	};

	struct asciiNode
	{
		asciiNode() : freq(0), encoding("") {}

		int freq;
		string encoding;
	};

	shared_ptr<node> root;
	unique_ptr<asciiNode[]> asciiArray;
	const int asciiSize = 128;
	int encodedFilePadding;

	/*
	Compressing methods
	*/
	bool readFile(string filename);

	void buildTree();

	bool writeEncoded2File(string inputFile, string outputFile);

	bool isLeaf(shared_ptr<node> const& nd) const;

	void encode(shared_ptr<node> const& nd, string huffCode);

	/*
	Decompressing methods
	*/

	bool readEncodedFile(string filename, string& bitStream);

	bool writeDecoded2File(string bits, string outputFile);
};

#endif