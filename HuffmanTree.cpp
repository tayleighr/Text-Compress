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

#include <iostream> 
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include "HuffmanTree.h"
#include <memory>
#include <bitset>

using namespace std;

HuffmanTree::HuffmanTree():encodedFilePadding{0}
{
	root.reset(new node);
	asciiArray.reset(new asciiNode[128]);
}

HuffmanTree::~HuffmanTree() {}

bool HuffmanTree::makeCompressedFile(string inputFile, string outputFile)
{
	if (!readFile(inputFile))
		return false;

	buildTree();

	encode(root, "");

	if (!writeEncoded2File(inputFile, outputFile))
	{
		cout << "couldn't write to encoded file" << endl;
		return false;
	}
	
	return true;
}

bool HuffmanTree::readFile(string filename)
{
	unsigned char read;
	ifstream infile(filename.c_str());
	
	if (!infile.is_open())
	{
		cout << "ERROR opening input file!" << endl;
		return false;
	}
	 
	while (infile >> noskipws >> read)
	{
		asciiArray[read].freq++;
	}
	
	infile.close();

	return true;
}

void HuffmanTree::buildTree()
{
	priority_queue<shared_ptr<HuffmanTree::node>, vector<shared_ptr<HuffmanTree::node>>, HuffmanTree::compareFreq> pq;
	shared_ptr<node> tempNode;
	shared_ptr<node> temp1;
	shared_ptr<node> temp2;
	shared_ptr<node> temp3;

	for (int i = 0; i < asciiSize; i++)
	{
		if (asciiArray[i].freq > 0)
		{
			tempNode = make_shared<node>();
			tempNode->ch = i;
			tempNode->freq = asciiArray[i].freq;
			pq.push(tempNode);
		}
	}

	while (!pq.empty())
	{
		temp1=pq.top();
		pq.pop();

		if (!pq.empty())
		{
			temp3 = make_shared<node>();
			temp2=pq.top();
			pq.pop();
			temp3->left = temp1;
			temp3->right = temp2;
			temp3->freq = temp1->freq + temp2->freq;
			pq.push(temp3);
		}
		else
		{
			root = temp1;
		}
	}
}

bool HuffmanTree::isLeaf(shared_ptr<node> const& nd) const
{
	if (nd->left == nullptr && nd->right == nullptr)
		return true;
	return false;
}

void HuffmanTree::encode(shared_ptr<node> const& nd, string huffCode)
{
	if (isLeaf(nd))
	{
		asciiArray[nd->ch].encoding = huffCode;

		//cout << nd->ch << ": " << huffCode << endl;
		return;
	}

	encode(nd->left, huffCode + "0");
	encode(nd->right, huffCode + "1");

	return;
}

bool HuffmanTree::writeEncoded2File(string inputFile, string outputFile)
{
	unsigned char read;
	const int WRITE_SIZE = 8;

	string bitStream = "";

	ifstream infile(inputFile.c_str());

	if (!infile.is_open())
	{
		cout << "ERROR opening input file!" << endl;
		return false;
	}

	ofstream outfile(outputFile.c_str(), ios::binary);

	if (!outfile.is_open())
	{
		cout << "ERROR opening output file!" << endl;
		return false;
	}

	while (infile >> noskipws >> read)
	{
		bitStream += asciiArray[read].encoding;
		printToEncoded(bitStream, outfile);
	}

	while ((int)bitStream.length() % WRITE_SIZE > 0) //pad the last byte of encrypted file with 0's
	{
		bitStream += '0';
		++encodedFilePadding;
	}

	printToEncoded(bitStream, outfile);

	outfile.close();

	return true;
}

void HuffmanTree::printToEncoded(string& bitStream, ostream& outfile)
{
	const int WRITE_SIZE = 8;

	if ((int)bitStream.length() >= WRITE_SIZE)
	{
		string byte = bitStream.substr(0, WRITE_SIZE);
		bitset<WRITE_SIZE> segment(byte);
		const char toWrite = (char)((int)segment.to_ulong());
		outfile.write(&toWrite, sizeof(char));
		bitStream = bitStream.substr(WRITE_SIZE, bitStream.length() - WRITE_SIZE);
	}
}

/*
The following functions are for recieving and decoding encoded files.
*/
bool HuffmanTree::decompressFile(string encodedFile, string outputFile)
{
	if (!decodeToFile(encodedFile, outputFile))
		return false;

	cout << "Text Compression Successful." << endl;

	return true;
}


/*
This is for the next implementation using less storage.
*/
bool HuffmanTree::decodeToFile(string eFile, string outputFile)
{
	ifstream encodedFile(eFile.c_str(), ios::binary);
	ofstream oFile(outputFile.c_str());

	if (!encodedFile.is_open())
	{
		cout << "ERROR opening encoded file!" << endl;
		return false;
	}

	if (!oFile.is_open())
	{
		cout << "ERROR opening printing file!" << endl;
		return false;
	}

	string newBits, delayBits = "";
	unsigned char readMe;
	shared_ptr<node> walk = root;

	while (encodedFile >> noskipws >> readMe)
	{
		bitset<8> set((unsigned long)readMe);
		newBits = set.to_string();

		if (delayBits != "")
		{
			walkTree(walk, delayBits, oFile);
		}
		delayBits = newBits;
	}
	
	string finalBits = delayBits.substr(0, (int)delayBits.length() - encodedFilePadding);
	walkTree(walk, finalBits, oFile);
	encodedFile.close();
	oFile.close();

	return true;
}

void HuffmanTree::walkTree(shared_ptr<node>& walk, string bits, ostream& outputFile)
{
	for (int i = 0; i < (int)bits.length(); i++)
	{
		if (bits[i] == '0')
		{
			walk = walk->left;
		}
		else
		{
			walk = walk->right;
		}

		if (isLeaf(walk))
		{
			outputFile << walk->ch;
			walk = root;
		}
	}
}
