#include "HuffmanTree.h"
#include <iostream>

using namespace std;

int main()
{
	HuffmanTree a;

	a.makeCompressedFile("inputLong.txt", "encoded.txt");
	a.decompressFile("encoded.txt", "output.txt");

	return 0;
}