/*
Filename: HuffmanTree.h
Author: Taylor Roozen

Purpose: This implementation takes a text document, scans the document to determine character frequency, builds a
huffman tree based on those frequencies, and then encodes each character in the text via the search path taken to
find each character (leaf) as the document is re-scanned. Moving to a left child in the tree adds a 0 to the character 
code, searching right adds a 1. 

Once the entire document is represented as a binary encoded string, the string is separated into bytes, and printed into
an enoded text file (strings of characters not divisible by 8 are padded with zeros).

In order to decompress, the reciever needs the original huffman tree structure, and the amount of zero padding added to
the document. It then recovers the binary representation of the encoded characters, re-constructs the bit string, and 
traverses the tree according to the binary pattern (0->left, 1->right); every time a leaf is encountered in the tree, 
the character represented at that leaf is printed to the final, recovered text. The last n padded digits are ignored.

The next step in this implementation would be to combine it with a TCP/IP protocol to actually send the compressed
document.
*/

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

	void printToEncoded(string& bitStream, ostream& outfile);

	bool isLeaf(shared_ptr<node> const& nd) const;

	void encode(shared_ptr<node> const& nd, string huffCode);

	/*
	Decompressing methods
	*/

	bool decodeToFile(string eFile, string outputFile);
	void walkTree(shared_ptr<node>& walk, string bits, ostream& outputFile);
};

#endif