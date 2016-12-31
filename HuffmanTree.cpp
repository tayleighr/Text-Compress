#include <iostream> 
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include "HuffmanTree.h"
#include <memory>
#include <bitset>

using namespace std;

HuffmanTree::HuffmanTree()
{
	root.reset(new node);
	//root = nullptr;
	asciiArray.reset(new asciiNode[256]);
}

HuffmanTree::~HuffmanTree() {}

/*
void HuffmanTree::traverseDelete(shared_ptr<node> r)
{
if (r == nullptr)
return;
traverseDelete(r->left);
traverseDelete(r->right);
delete r;
return;
}
*/

/*
The following functions are for encoding and 'sending' encoded files.
*/
bool HuffmanTree::makeCompressedFile(string inputFile, string outputFile)
{
	if (!readFile(inputFile))
		return false;
	buildTree();
	encode(root, "");

	if (!writeEncoded2File(inputFile, outputFile))
		return false;

	return true;
}

bool HuffmanTree::readFile(string filename)
{
	char read;
	ifstream inFile(filename.c_str());


	if (!inFile.is_open())
	{
		cout << "ERROR opening file" << endl;
		return false;
	}

	while (inFile >> noskipws >> read)
		asciiArray[read].freq++;

	if (inFile.fail())
	{
		cout << "ERROR reading file" << endl;
		return false;
	}
	inFile.close();

	if (inFile.fail())
	{
		cout << "ERROR closing file" << endl;
		return false;
	}

	return true;
}

void HuffmanTree::buildTree()
{
	priority_queue<shared_ptr<HuffmanTree::node>, vector<shared_ptr<HuffmanTree::node>>, HuffmanTree::compareFreq> pq;
	shared_ptr<node> tempNode;
	shared_ptr<node> temp1;
	shared_ptr<node> temp2;
	shared_ptr<node> temp3
		;
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
		temp2=pq.top();

		temp3 = make_shared<node>();

		if (temp2 != nullptr)
		{
			pq.pop();
			temp3->left = temp1;
			temp3->right = temp2;
			temp3->freq = temp1->freq + temp2->freq;
			pq.push(tempNode);
		}
		else
		{
			temp3 = temp1;
		}
		root = temp3;
	}
}

void HuffmanTree::encode(shared_ptr<node> nd, string huffCode)
{
	if (nd->ch != '/0')
	{
		asciiArray[nd->ch].encoding = huffCode;
		return;
	}

	encode(nd->left, huffCode + "0");
	encode(nd->right, huffCode + "1");

	return;
}

bool HuffmanTree::writeEncoded2File(string inputFile, string outputFile)
{
	char read;
	const int WRITE_SIZE = 8;

	string stream = "";

	ifstream infile(inputFile.c_str());

	if (!infile.is_open())
	{
		cout << "ERROR opening input file!" << endl;
		return false;
	}


	while (infile >> noskipws >> read)
	{
		stream += asciiArray[read].encoding;
	}

	infile.close();
	if (infile.fail())
	{
		cout << "ERROR closing input file!" << endl;
		return false;
	}

	ofstream outfile(outputFile.c_str(), ios::binary);

	if (!outfile.is_open())
	{
		cout << "ERROR opening output file!" << endl;
		return false;
	}

	while ((int)stream.length() > WRITE_SIZE)
	{
		string byte = stream.substr(0, WRITE_SIZE);
		bitset<8> segment(byte);
		const char toWrite = (unsigned char)((unsigned int)segment.to_ulong());
		outfile.write(&toWrite, sizeof(char));
		stream = stream.substr(WRITE_SIZE, stream.length() - WRITE_SIZE);
	}

	outfile.close();
	if (outfile.fail())
	{
		cout << "ERROR closing output file!" << endl;
		return false;
	}

	return true;
}


/*
The following functions are for recieving and decoding encoded files.
*/
bool HuffmanTree::decompressFile(string inputFile, string outputFile)
{
	string bitStream = "";

	readEncodedFile(inputFile, bitStream);

	if (!writeDecoded2File(bitStream, outputFile))
		return false;

	return true;
}

bool HuffmanTree::readEncodedFile(string filename, string& bitStream)
{
	ifstream encodedfile(filename.c_str(), ios::binary);

	if (!encodedfile.is_open())
	{
		cout << "ERROR opening encoded file!" << endl;
		return false;
	}

	encodedfile >> std::noskipws;

	while (!encodedfile.eof())
	{
		unsigned char readMe;
		encodedfile >> readMe;
		bitset<8> set((unsigned long)readMe);
		bitStream += set.to_string();
	}


	encodedfile.close();

	if (encodedfile.fail())
	{
		cout << "ERRORS reading from encoded file!" << endl;
		return false;
	}

	return true;
}

bool HuffmanTree::writeDecoded2File(string bits, string outputFile)
{
	ofstream oFile(outputFile.c_str());

	if (!oFile.is_open())
	{
		cout << "ERROR opening printing file!" << endl;
		return false;
	}

	shared_ptr<node> walk;

	for (int i = 0; i < (int)bits.length(); i++)
	{
		if (bits[i] == 0)
			walk = walk->left;
		else
			walk = walk->right;

		if (walk->ch != '\0')
		{
			oFile << walk->ch;
			walk = root;
		}
	}

	if (oFile.fail())
	{
		cout << "ERROR printing to output file!" << endl;
		return false;
	}

	oFile.close();

	if (oFile.fail())
	{
		cout << "ERROR closing output file!" << endl;
	}
	return true;
}