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
	//root = nullptr;
	asciiArray.reset(new asciiNode[128]);
	//messageCharCount = 0;
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
	{
		cout << "couldn't write to encoded file" << endl;
		return false;
	}
	

	return true;
}

bool HuffmanTree::readFile(string filename)
{
	char read;
	ifstream infile(filename.c_str());
	

	if (!infile.is_open())
	{
		cout << "ERROR opening file!" << endl;
		return false;
	}
	 
	while (infile >> noskipws >> read)
	{
		asciiArray[read].freq++;
	}
	
	/*
	if (infile.fail())
	{
		cout << "ERROR reading file" << endl;
		return false;
	}
	*/
	infile.close();

	/*
	if (infile.fail())
	{
		cout << "ERROR closing file" << endl;
		return false;
	}
	*/
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
		cout << nd->ch << " with huffcode: " << huffCode << endl;
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
	/*
	if (infile.fail())
	{
		cout << "ERROR closing input file!" << endl;
		return false;
	}
	*/

	ofstream outfile(outputFile.c_str(), ios::binary);

	if (!outfile.is_open())
	{
		cout << "ERROR opening output file!" << endl;
		return false;
	}

	while((int)stream.length() % 8 > 0) //generate arbitrary bits to create even byte count
	{
		stream += '0';
		encodedFilePadding++;
	}

	cout << encodedFilePadding << "   file padding is" << endl;
	while ((int)stream.length() > 0)
	{
		string byte = stream.substr(0, WRITE_SIZE);

		bitset<WRITE_SIZE> segment(byte);
		const char toWrite = (unsigned char)((unsigned int)segment.to_ulong());
		outfile.write(&toWrite, sizeof(char));
		stream = stream.substr(WRITE_SIZE, stream.length() - WRITE_SIZE);
		cout << "One byte at a time: " << byte << endl;
	}

	outfile.close();
	/*
	if (outfile.fail())
	{
		cout << "ERROR closing output file!" << endl;
		return false;
	}
	*/

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

	unsigned char readMe;
	while (encodedfile >> noskipws >> readMe)
	{
		bitset<8> set((unsigned long)readMe);
		bitStream += set.to_string();
	}

	encodedfile.close();

	/*
	if (encodedfile.fail())
	{
		cout << "ERRORS reading from encoded file!" << endl;
		return false;
	}
	*/

	return true;
}

bool HuffmanTree::writeDecoded2File(string bits, string outputFile)
{
	ofstream oFile(outputFile.c_str());
	int printedChars = 0;

	if (!oFile.is_open())
	{
		cout << "ERROR opening printing file!" << endl;
		return false;
	}

	shared_ptr<node> walk = root;
	cout << encodedFilePadding << "encoded file padding" << endl;
	for (int i = 0; i < (int)bits.length()-encodedFilePadding; i++)
	{
		if (bits[i] == '0')
		{
			cout << '0';
			walk = walk->left;
		}
		else
		{
			cout << '1';
			walk = walk->right;
		}

		if (isLeaf(walk))
		{	
			++printedChars;
			oFile << walk->ch;
			cout << "   " << walk->ch << endl;
			walk = root;

		}
	}

	/*
	if (oFile.fail())
	{
		cout << "ERROR printing to output file!" << endl;
		return false;
	}
	*/

	oFile.close();
	/*
	if (oFile.fail())
	{
		cout << "ERROR closing output file!" << endl;
	}
	*/
	return true;
}